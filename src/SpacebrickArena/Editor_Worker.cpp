#include "include/Editor_Worker.h"

#include <algorithm>
#include "TheBrick/BrickInstance.h"
#include "include/Space.h"
#include "include/Editor_History.h"
#include <TheBrick/Conversion.h>
#include "include/Editor_ShipHandler.h"

#include "TheBrick/DebugDraw.h"
#include "include/Editor_Assistant.h"
#include "include/Editor_BrickCategory.h"
#include "include/DestructionManager.h"
#include "include/ShipManager.h"
#include "include/Editor_Grid.h"
#include "include/BrickManager.h"
#include "include/InputManager.h"

namespace Editor
{
//#define EDITOR_DEBUG
    // **************************************************************************
    // **************************************************************************
    CWorker::CWorker(int a_playerIdx)
    {
        this->m_PlayerIdx = a_playerIdx;
    }


    // **************************************************************************
    // **************************************************************************
    CWorker::~CWorker()
    {
        SAFE_DELETE(this->m_pGrid);
        SAFE_DELETE(this->m_pCurrentBrickObject);
        SAFE_DELETE(this->m_pHistory);
        SAFE_DELETE(this->m_pCamera);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Initialize(PuRe_IGraphics& a_rGraphics)
    {
        PuRe_GraphicsDescription gdesc = a_rGraphics.GetDescription();
        this->m_pCamera = new CCamera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective, this->m_PlayerIdx);

        this->m_CurrentPositionCache = PuRe_Vector2F(0, 0);
        this->m_CurrentPosition = PuRe_Vector2I(0, 0);
        this->m_CurrentHeight = 0;
#ifdef EDITOR_DEV
        this->m_pCamera->Initialize(PuRe_Vector3F(20, 135, 0), PuRe_Vector3F(-1, 0, 0), 10.0f, 120.0f, 50.0f);
#else
        this->m_pCamera->Initialize(PuRe_Vector3F(20, 135, 0), PuRe_Vector3F(-1, 0, 0));
#endif
        this->m_pHistory = new CHistory();
        this->m_PlaceBelow = false;
        this->m_pCurrentBrickObject = new TheBrick::CGameObject(*sba_World, nullptr);
        this->m_CurrentHeightIsInvalid = true;

        this->m_pGrid = new CGrid(a_rGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick, PuRe_Color& a_rCurrentColor, CShipHandler& a_rShipHandler, bool a_Delete)
    {
        if (a_Delete)
        {
            this->m_CurrentColor = PuRe_Color(1, 1, 1);
            this->m_CanPlaceHere = false;
        }
        else
        {
            this->m_CurrentColor = a_rCurrentColor;
            if (this->m_BrickToDelete.BrickInstance != nullptr)
            {
                this->m_BrickToDelete.BrickInstance->m_Color = this->m_BrickToDelete.Color;
                this->m_BrickToDelete.BrickInstance = nullptr;
            }
            this->RestoreAdhesiveBricksToDelete();
            this->m_AdhesiveBricksToDelete.clear();
        }
        //-----First Brick Placement-----
        bool firstBrick = false;
        if (a_rShipHandler.GetCurrentSpaceShip()->m_pBricks.size() == 0)
        { //First Brick
            firstBrick = true;
            this->m_CurrentPositionCache = PuRe_Vector2F::Zero();
            this->m_CurrentPosition = PuRe_Vector2I::Zero();
            this->m_CanPlaceHere = true;
            this->m_PlaceBelow = false;
            this->m_CurrentHeight = 0;
        }
        //-----Set Current Brick-----
        if (a_Delete && (this->m_pCurrentBrick == nullptr || this->m_pCurrentBrick->m_pBrick->GetBrickId() != 2))
        {
            if (this->m_pCurrentBrick != nullptr)
            {
                SAFE_DELETE(this->m_pCurrentBrick);
            }
            this->m_pCurrentBrick = sba_BrickManager->GetBrick(2).CreateInstance(*this->m_pCurrentBrickObject, *sba_World); //Create Instance
            this->m_CurrentHeightIsInvalid = true;
        }
        else if (!a_Delete && (this->m_pCurrentBrick == nullptr || this->m_pCurrentBrick->m_pBrick != a_pCurrentBrick))
        {
            if (this->m_pCurrentBrick != nullptr)
            {
                SAFE_DELETE(this->m_pCurrentBrick);
            }
            this->m_pCurrentBrick = a_pCurrentBrick->CreateInstance(*this->m_pCurrentBrickObject, *sba_World); //Create Instance
            this->m_CurrentHeightIsInvalid = true;
        }
        //-----Camera-----
        this->m_pCamera->Update(&a_pGraphics, &a_pWindow, &a_pTimer);
        //-----Rotation-----
        if (!a_Delete)
        {
            this->UpdateRotation();
        }
        if (!firstBrick)
        {
            //-----Translation-----
            this->UpdateTranslation(this->m_pCamera->GetForward(), a_pTimer.GetElapsedSeconds() * 9);
            //-----Placement Direction-----
            if (sba_Input->ButtonPressed(sba_Button::EditorTogglePlacementSide, this->m_PlayerIdx))
            {
                sba_SoundPlayer->PlaySound("editor_change", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                this->m_PlaceBelow = !this->m_PlaceBelow;
                this->m_CurrentHeightIsInvalid = true;
            }
            //-----Height-----
            if (this->m_CurrentHeightIsInvalid)
            {
                if (a_Delete)
                {
                    this->UpdateDeleteHeight(a_rShipHandler);
                }
                else
                {
                    this->UpdateHeight(a_rShipHandler);
                    if (this->m_CanPlaceHere)
                    { //Check for max special bricks count
                        const CShipHandler::ShipDataCache& shipData = a_rShipHandler.GetCurrentShipData();
                        int categoryID = this->m_pCurrentBrick->m_pBrick->GetCategoryId();
                        if ((categoryID == CBrickCategory::CATEGORY_COCKPITS && shipData.Cockpits >= sba::CSpaceship::MAX_COCKPITS)
                            || (categoryID == CBrickCategory::CATEGORY_WEAPONS && shipData.Weapons >= sba::CSpaceship::MAX_WEAPONS)
                            || (categoryID == CBrickCategory::CATEGORY_ENGINES && shipData.Engines >= sba::CSpaceship::MAX_ENGINES))
                        {
                            this->m_CanPlaceHere = false;
#ifdef EDITOR_DEBUG
                            printf("MaxSpecialRequirements NOT passed\n");
#endif
                        }
                        if ((categoryID == CBrickCategory::CATEGORY_COCKPITS
                            || categoryID == CBrickCategory::CATEGORY_WEAPONS
                            || categoryID == CBrickCategory::CATEGORY_ENGINES)
                            && shipData.Total() >= sba::CSpaceship::MAX_PERKS)
                        {
                            this->m_CanPlaceHere = false;
#ifdef EDITOR_DEBUG
                            printf("MaxPerksRequirements NOT passed\n");
#endif
                        }
                    }
                }
                this->m_CurrentHeightIsInvalid = false;
            }
        }
        //-----Apply-----
        this->ApplyToCurrentBrick();
        //-----Placement-----
        if (a_Delete)
        {
            this->UpdateDelete(a_rShipHandler);
        }
        else
        {
            this->UpdatePlacement(a_rShipHandler);
        }
        //-----Miscellaneous-----
        this->UpdateMiscellaneous(a_rShipHandler, a_pCurrentBrick);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Render(CShipHandler& a_rShipHandler, sba::CSpriteReader& a_rSpriteReader)
    {
        //Draw Grid Arrow when no engines placed
        std::vector<TheBrick::CBrickInstance**> engines;
        a_rShipHandler.GetCurrentSpaceShip()->GetEngines(engines);
        this->m_pGrid->Draw(engines.size() == 0);

        sba::CSpaceship& ship = *a_rShipHandler.GetCurrentSpaceShip();
        sba_Space->RenderFont(std::to_string(ship.m_pBricks.size()) + "/" + std::to_string(sba::CSpaceship::MAX_BRICK_COUNT) + " Bricks", PuRe_Vector2F(sba_Width - 300.0f, sba_Height - 50.0f), 18);
        sba_Space->RenderFont("Ship: " + ship.GetName(), PuRe_Vector2F(sba_Width / 2 - 200.0f, sba_Height - 50.0f), 18);
        const CShipHandler::ShipDataCache& shipData = a_rShipHandler.GetCurrentShipData();
        sba_Space->RenderFont(std::to_string(sba::CSpaceship::MAX_PERKS - shipData.Total()) + " Perks left", PuRe_Vector2F(sba_Width - 300.0f, sba_Height - 100.0f), 18);
        sba_Space->RenderFont(std::to_string(shipData.Cockpits) + " Cockpits [1-" + std::to_string(sba::CSpaceship::MAX_COCKPITS) + "]", PuRe_Vector2F(sba_Width - 300.0f, sba_Height - 140.0f), 14);
        sba_Space->RenderFont(std::to_string(shipData.Engines) + " Engines [1-" + std::to_string(sba::CSpaceship::MAX_ENGINES) + "]", PuRe_Vector2F(sba_Width - 300.0f, sba_Height - 180.0f), 14);
        sba_Space->RenderFont(std::to_string(shipData.Weapons) + " Weapons [1-" + std::to_string(sba::CSpaceship::MAX_WEAPONS) + "]", PuRe_Vector2F(sba_Width - 300.0f, sba_Height - 220.0f), 14);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Suspend()
    {
        //Delete
        if (this->m_pCurrentBrick != nullptr)
        {
            SAFE_DELETE(this->m_pCurrentBrick);
            this->m_pCurrentBrick = nullptr;
        }
        if (this->m_BrickToDelete.BrickInstance != nullptr)
        {
            SAFE_DELETE(this->m_BrickToDelete.BrickInstance);
            this->m_BrickToDelete.BrickInstance = nullptr;
        }
        for (std::vector<BrickToDeleteCache>::iterator it = this->m_AdhesiveBricksToDelete.begin(); it != this->m_AdhesiveBricksToDelete.end(); ++it)
        {
            SAFE_DELETE(it->BrickInstance);
            it->BrickInstance = nullptr;
        }
        this->m_AdhesiveBricksToDelete.clear();

        //Clear History
        this->m_pHistory->Clear();
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Resume(CShipHandler& a_rShipHandler)
    {
        //Rebuild History
        this->m_pHistory->Clear();
        std::vector<TheBrick::CBrickInstance*>& bricks = a_rShipHandler.GetCurrentSpaceShip()->m_pBricks;
        for (size_t i = 0; i < bricks.size(); i++)
        {
            this->m_pHistory->AddStep(bricks[i]);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateTranslation(PuRe_Vector3F a_cameraLook, float a_speed)
    {
        PuRe_Vector2F MoveInput = sba_Input->Direction(sba_Direction::EditorMoveBrick, this->m_PlayerIdx) * a_speed;
        if (a_cameraLook.Y > 0.8f)
        {
            MoveInput.Y *= -1;
        }

        //SpeedupTimer
        if (MoveInput.Length() > FLT_EPSILON * 2)
        {
            this->m_GamepadMovementSpeedupTimer += sba_Application->GetTimer()->GetElapsedSeconds();
        }
        else
        {
            this->m_GamepadMovementSpeedupTimer = 0;
        }
        if (sba_Input->LastInputWasGamepad())
        {
            MoveInput *= PuRe_clamp((this->m_GamepadMovementSpeedupTimer) * 4 - 1, 0.75f, 5);
        }

        //----------Apply
        //Force Forward in 8 Directions
        PuRe_Vector2F forwardRaw = PuRe_Vector2F(a_cameraLook.X, a_cameraLook.Z);
        forwardRaw.Normalize();
        PuRe_Vector2F forward = forwardRaw;
        forward.X = round(forward.X);
        forward.Y = round(forward.Y);
        //Only 4 Directions when using gamepad else stay with 8
        PuRe_Vector2F forwardN = forward;
        forwardN.Normalize();
        if (sba_Input->LastInputWasGamepad() && abs(abs(forward.X) - abs(forward.Y)) < 0.1f)
        {
            float alpha = 45;
            if (PuRe_Vector2F::Dot(forwardN.Side(), forwardRaw) > 0) //Detect right direction
            {
                alpha *= -1;
            }
            float cosAlpha = cos(alpha * 0.0174532925f);
            float sinAlpha = sin(alpha * 0.0174532925f);
            forward = PuRe_Vector2F(cosAlpha*forward.X - sinAlpha*forward.Y, sinAlpha*forward.X + cosAlpha*forward.Y); //Rotate
        }

        //Set Input in the according forward direction
        this->m_CurrentPositionCache += forward * MoveInput.Y;
        this->m_CurrentPositionCache += PuRe_Vector2F(forward.Y, -forward.X) * MoveInput.X;
        this->m_CurrentPositionCache.X = PuRe_clamp(this->m_CurrentPositionCache.X, -(float)sba::CSpaceship::MAX_BRICK_WIDTH, (float)sba::CSpaceship::MAX_BRICK_WIDTH);
        this->m_CurrentPositionCache.Y = PuRe_clamp(this->m_CurrentPositionCache.Y, -(float)sba::CSpaceship::MAX_BRICK_WIDTH, (float)sba::CSpaceship::MAX_BRICK_WIDTH);

        PuRe_Vector2I posCache = this->m_CurrentPosition;
        this->m_CurrentPosition = PuRe_Vector2I((int)round(this->m_CurrentPositionCache.X), (int)round(this->m_CurrentPositionCache.Y)); //Snap to grid
        if (this->m_CurrentPosition.X != posCache.X || this->m_CurrentPosition.Y != posCache.Y)
        { //Position changed
            sba_SoundPlayer->PlaySound("editor_move", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            this->m_CurrentHeightIsInvalid = true;
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateRotation()
    {
        if (this->m_pCurrentBrick->m_pBrick->GetCategoryId() == CBrickCategory::CATEGORY_ENGINES)
        {
            this->m_CurrentRotation = 0;
            return;
        }
        if (sba_Input->ButtonPressed(sba_Button::EditorRotateBrickRight, this->m_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("editor_change", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            this->m_CurrentRotation++;
            this->m_CurrentHeightIsInvalid = true;
        }
        if (sba_Input->ButtonPressed(sba_Button::EditorRotateBrickLeft, this->m_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("editor_change", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            this->m_CurrentRotation--;
            this->m_CurrentHeightIsInvalid = true;
        }
        this->m_CurrentRotation += 4; //Avoid negative numbers
        this->m_CurrentRotation = this->m_CurrentRotation % 4; //Snap Rotation
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateHeight(CShipHandler& a_rShipHandler)
    {
        //Set Brick up
        this->m_CurrentHeight = sba::CSpaceship::MAX_BRICK_HEIGHT;
        if (this->m_PlaceBelow)
        {
            this->m_CurrentHeight *= -1;
        }
        this->ApplyToCurrentBrick();

        //Block placing
        this->m_CanPlaceHere = false;

        const ong::Transform& brickTransform = ong::transformTransform(this->m_pCurrentBrick->GetTransform(), this->m_pCurrentBrick->GetGameObject()->GetTransform());

        //Casting Nub Requirements
        ong::vec3 nubToCastFromDirection = ong::vec3(0, -1, 0);
        bool nubToCastFromShouldBeMale = false;
        if (this->m_PlaceBelow)
        {
            nubToCastFromShouldBeMale = true;
            nubToCastFromDirection.y *= -1;
        }

        //Get RayCastHit
        std::vector<ong::RayQueryResult> hitResults;
        std::vector<ong::vec3> hitResultRayOrigins;
        if (!CAssistant::GetClosestHitsFromBrickInstanceNubs(*this->m_pCurrentBrick, *a_rShipHandler.GetCurrentSpaceShip(), nubToCastFromShouldBeMale, nubToCastFromDirection, &hitResults, &hitResultRayOrigins))
        { //Nothing hit
            this->m_CurrentHeight = 0;
#ifdef EDITOR_DEBUG
            printf("Nothing hit\n");
#endif
            return;
        }
        assert(hitResults.size() == hitResultRayOrigins.size());

        //Docking Nub Requirements
        ong::vec3 nubRequestedDirection = -nubToCastFromDirection;
        bool nubRequestedGenderIsMale = !nubToCastFromShouldBeMale;
        for (size_t i = 0; i < hitResults.size(); i++)
        {
            ong::vec3 hitDelta = hitResults[i].point - hitResultRayOrigins[i];

            //Docking test
            if (CAssistant::CanDockAtHit(hitResults[i], nubRequestedGenderIsMale, nubRequestedDirection))
            {
                //Check for other Collision
                ong::vec3 maxCollisionFreeDelta = ong::vec3(0, 0, 0);
                if (CAssistant::MovementDeltaIsCollisionFree(*this->m_pCurrentBrick, *a_rShipHandler.GetCurrentSpaceShip(), hitDelta, TheBrick::CBrick::SEGMENT_HEIGHT, &maxCollisionFreeDelta))
                { //CollisionFree
                    if (CAssistant::SpecialCategoryRequirementsPassed(*this->m_pCurrentBrick, *a_rShipHandler.GetCurrentSpaceShip()))
                    {
                        this->m_CanPlaceHere = true;
                        this->m_CurrentHeight = (int)round((hitResultRayOrigins[i].y + maxCollisionFreeDelta.y + (brickTransform.p.y - hitResultRayOrigins[i].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);
#ifdef EDITOR_DEBUG
                        printf("Passed all tests: delta(%f, %f, %f)\n", maxCollisionFreeDelta.x, maxCollisionFreeDelta.y, maxCollisionFreeDelta.z);
#endif
                        return;
                    }
                    this->m_CurrentHeight = (int)round((hitResultRayOrigins[i].y + maxCollisionFreeDelta.y + (brickTransform.p.y - hitResultRayOrigins[i].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);
#ifdef EDITOR_DEBUG
                    printf("SpecialCategoryRequirements NOT passed: delta(%f, %f, %f)\n", maxCollisionFreeDelta.x, maxCollisionFreeDelta.y, maxCollisionFreeDelta.z);
#endif
                    return;
                }
                //Handle docking test success but not collision free
                this->m_CurrentHeight = (int)round((hitResultRayOrigins[i].y + maxCollisionFreeDelta.y + (brickTransform.p.y - hitResultRayOrigins[i].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);
#ifdef EDITOR_DEBUG
                printf("NOT Collision free: delta(%f, %f, %f)\n", maxCollisionFreeDelta.x, maxCollisionFreeDelta.y, maxCollisionFreeDelta.z);
#endif
                return;
            }
        }
        //Can't dock
#ifdef EDITOR_DEBUG
        printf("Can't dock\n");
#endif
        this->m_CurrentHeight = (int)round((hitResults[0].point.y + (brickTransform.p.y - hitResultRayOrigins[0].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateDeleteHeight(CShipHandler& a_rShipHandler)
    {
        //Set cursor above all bricks
        this->m_CurrentHeight = sba::CSpaceship::MAX_BRICK_HEIGHT + 3;
        if (this->m_PlaceBelow)
        {
            this->m_CurrentHeight *= -1;
        }
        this->ApplyToCurrentBrick();

        //Casting Nub Requirements
        ong::vec3 nubToCastFromDirection = ong::vec3(0, -1, 0);
        bool nubToCastFromShouldBeMale = false;
        if (this->m_PlaceBelow)
        {
            nubToCastFromShouldBeMale = true;
            nubToCastFromDirection.y *= -1;
        }

        //Get RayCastHit
        std::vector<ong::RayQueryResult> hitResults;
        std::vector<ong::vec3> hitResultRayOrigins;
        if (!CAssistant::GetClosestHitsFromBrickInstanceNubs(*this->m_pCurrentBrick, *a_rShipHandler.GetCurrentSpaceShip(), nubToCastFromShouldBeMale, nubToCastFromDirection, &hitResults, &hitResultRayOrigins))
        { //Nothing hit
            this->m_CurrentHeight = 0;
#ifdef EDITOR_DEBUG
            printf("Nothing hit\n");
#endif
            if (this->m_BrickToDelete.BrickInstance != nullptr)
            {
                this->m_BrickToDelete.BrickInstance->m_Color = this->m_BrickToDelete.Color; //Restore color
                this->m_BrickToDelete.BrickInstance = nullptr;
            }
            //Restore old state adhesive bricks
            this->RestoreAdhesiveBricksToDelete();
            this->m_AdhesiveBricksToDelete.clear();
            //Refresh graph
            return;
        }
        assert(hitResults.size() == hitResultRayOrigins.size());
        assert(hitResults.size() == 1);

        //Set cursor on hit brick
        const ong::Transform& brickTransform = ong::transformTransform(this->m_pCurrentBrick->GetTransform(), this->m_pCurrentBrick->GetGameObject()->GetTransform());
        this->m_CurrentHeight = (int)round((hitResults[0].point.y + (brickTransform.p.y - hitResultRayOrigins[0].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);

        TheBrick::CBrickInstance* hitBrick = reinterpret_cast<TheBrick::CBrickInstance*>(hitResults[0].collider->getUserData());
        if (this->m_BrickToDelete.BrickInstance == hitBrick)
        { //Hit the same brick, state still valid
            return;
        }

        //Restore old state adhesive bricks
        this->RestoreAdhesiveBricksToDelete();
        this->m_AdhesiveBricksToDelete.clear();
        //Set new brick to delete
        if (this->m_BrickToDelete.BrickInstance != nullptr)
        { //Restore color
            this->m_BrickToDelete.BrickInstance->m_Color = this->m_BrickToDelete.Color;
        }
        this->m_BrickToDelete.BrickInstance = hitBrick;
        this->m_BrickToDelete.Color = hitBrick->m_Color;
        //Display brick to delete
        this->m_BrickToDelete.BrickInstance->m_Color = PuRe_Color(0.05f, 0.05f, 0.05f);

        sba::CSpaceship* ship = a_rShipHandler.GetCurrentSpaceShip();
        sba_Space->DestructionManager->BuildDestruction(ship, ship->m_pBricks.data(), ship->m_pBricks.size());

        /*BrickToDeleteCache brickToDelete;
        brickToDelete.BrickInstance = hitBrick;
        this->m_AdhesiveBricksToDelete.push_back(brickToDelete);*/

        if (this->m_BrickToDelete.BrickInstance == ship->m_pBricks[0])
        { //BrickToDelete is center brick = no need to detect adhesive bricks
            for (auto brick : ship->m_pBricks)
            {
                if (brick == ship->m_pBricks[0])
                {
                    continue;
                }
                //Build adhesive brick storage
                BrickToDeleteCache cache;
                cache.BrickInstance = brick;
                cache.Color = brick->m_Color;
                this->m_AdhesiveBricksToDelete.push_back(cache);
                //Display adhesive bricks
                brick->m_Color = PuRe_Color(0.05f, 0.05f, 0.05f);
            }
            return;
        }

        //Cut connections from BrickToDelete to other bricks so that BrickToDelete is a dead end
        this->m_BrickToDelete.BrickInstance->GetDestructionInstance()->numConnections = 0;;

        //Start from center brick and traverse all bricks
        TraverseBricks(*ship->m_pBricks[0]->GetDestructionInstance());

        //Collect adhesive bricks
        for (auto brick : ship->m_pBricks)
        {
            if (brick->GetDestructionInstance()->tick != 0)
            {
                //Build adhesive brick storage
                BrickToDeleteCache cache;
                cache.BrickInstance = brick;
                cache.Color = brick->m_Color;
                this->m_AdhesiveBricksToDelete.push_back(cache);
                //Display adhesive bricks
                brick->m_Color = PuRe_Color(0.05f, 0.05f, 0.05f);
            }
        }

        //Reset data generated for destruction (resets ticks)
        sba_Space->DestructionManager->Reset();
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::TraverseBricks(sba::SBrickDestruction& a_rStartBrickDestruction)
    {
        if (a_rStartBrickDestruction.tick == 0)
        {
            return;
        }
        a_rStartBrickDestruction.tick = 0;
        for (unsigned int i = 0; i < a_rStartBrickDestruction.numConnections; i++)
        {
            TraverseBricks(*a_rStartBrickDestruction.connections[i].other); //Recursion
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::ApplyToCurrentBrick()
    {
        if (this->m_pCurrentBrick == nullptr)
        {
            return;
        }
        ong::Transform transform = this->m_pCurrentBrick->GetTransform();
        transform.p = ong::vec3(this->m_CurrentPosition.X * TheBrick::CBrick::SEGMENT_WIDTH, this->m_CurrentHeight * TheBrick::CBrick::SEGMENT_HEIGHT, this->m_CurrentPosition.Y * TheBrick::CBrick::SEGMENT_WIDTH);
        transform.q = ong::Quaternion(ong::vec3(0, 0, 0), 1);
        this->m_pCurrentBrick->SetTransform(transform);
        this->m_pCurrentBrick->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, this->m_CurrentRotation * 90.0f * 0.0174532925f, 0.0f));
        //#define ALPHAREADY
        if (this->m_CanPlaceHere || this->m_BrickToDelete.BrickInstance != nullptr) //Can place or can delete
        {
#ifdef ALPHAREADY
            this->m_pCurrentBrick->m_Color = PuRe_Color(this->m_currentBrickColor.R, this->m_currentBrickColor.G, this->m_currentBrickColor.B, this->m_currentBrickColor.A * 0.6f);
#else
            //this->m_pCurrentBrick->m_Color = PuRe_Color(this->m_CurrentColor.R * 1.5f, this->m_CurrentColor.G * 1.5f, this->m_CurrentColor.B * 1.5f, 1);
            this->m_pCurrentBrick->m_Color = this->m_CurrentColor;
#endif
        }
        else
        {
#ifdef ALPHAREADY
            this->m_pCurrentBrick->m_Color = PuRe_Color(1, 0, 0, 0.6f);
#else
            this->m_pCurrentBrick->m_Color = PuRe_Color(0, 0, 0, 1);
#endif
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdatePlacement(CShipHandler& a_rShipHandler)
    {
        //Check if can place
        if (!this->m_CanPlaceHere)
        {
            if (sba_Input->ButtonPressed(sba_Button::EditorPlaceBrick, this->m_PlayerIdx))
            {
                sba_SoundPlayer->PlaySound("editor_set_brick_fail", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            }
            return;
        }

        if (sba_Input->ButtonPressed(sba_Button::EditorPlaceBrick, this->m_PlayerIdx) && a_rShipHandler.GetCurrentSpaceShip()->m_pBricks.size() < (size_t)sba::CSpaceship::MAX_BRICK_COUNT)
        { //Place BrickInstance
            sba_SoundPlayer->PlaySound("editor_set_brick", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            this->m_pCurrentBrick->m_Color = this->m_CurrentColor; //Apply right color
            this->m_pHistory->AddStep(a_rShipHandler.AddBrickInstanceToCurrentShip(*this->m_pCurrentBrick)); //Add History step and add new instance to ship
            a_rShipHandler.UpdateCurrentShipData();
            this->m_CurrentHeightIsInvalid = true;
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateDelete(CShipHandler& a_rShipHandler)
    {
        if (this->m_BrickToDelete.BrickInstance == nullptr)
        {
            if (sba_Input->ButtonPressed(sba_Button::EditorDeleteBrick, this->m_PlayerIdx))
            {
                sba_SoundPlayer->PlaySound("editor_set_brick_fail", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            }
            return;
        }
        if (sba_Input->ButtonPressed(sba_Button::EditorDeleteBrick, this->m_PlayerIdx))
        { //Delete BrickInstances
            sba_SoundPlayer->PlaySound("editor_remove_block", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            //Restore Color
            this->RestoreAdhesiveBricksToDelete();
            this->m_BrickToDelete.BrickInstance->m_Color = this->m_BrickToDelete.Color;

            //Add History step
            std::vector<TheBrick::CBrickInstance*>* adhesiveInstances = new std::vector<TheBrick::CBrickInstance*>(this->m_AdhesiveBricksToDelete.size());
            for (size_t i = 0; i < adhesiveInstances->size(); i++)
            {
                (*adhesiveInstances)[i] = this->m_AdhesiveBricksToDelete[i].BrickInstance;
            }
            this->m_pHistory->AddStep(this->m_BrickToDelete.BrickInstance, adhesiveInstances, true);
            SAFE_DELETE(adhesiveInstances);
            //Set states
            a_rShipHandler.UpdateCurrentShipData();
            this->m_CurrentHeightIsInvalid = true;

            //Delete
            SAFE_DELETE(this->m_BrickToDelete.BrickInstance);
            this->m_BrickToDelete.BrickInstance = nullptr;
            for (std::vector<BrickToDeleteCache>::iterator it = this->m_AdhesiveBricksToDelete.begin(); it != this->m_AdhesiveBricksToDelete.end(); ++it)
            {
                SAFE_DELETE(it->BrickInstance);
                //it->BrickInstance = nullptr;
            }
            this->m_AdhesiveBricksToDelete.clear();
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateMiscellaneous(CShipHandler& a_rShipHandler, TheBrick::CBrick* a_pCurrentBrick)
    {
        //SpeedupTimer
        if (sba_Input->ButtonIsPressed(sba::Input::EButton::EditorUndoRedoHold, this->m_PlayerIdx))
        {
            if (sba_Input->ButtonIsPressed(sba_Button::EditorUndo, this->m_PlayerIdx))
            { //Undo timer < 0
                if (this->m_UndoRedoScrollTimer > 0)
                {
                    this->m_UndoRedoScrollTimer = 0;
                }
                this->m_UndoRedoScrollTimer -= sba_Application->GetTimer()->GetElapsedSeconds();
            }
            else if (sba_Input->ButtonIsPressed(sba_Button::EditorRedo, this->m_PlayerIdx))
            { //Redo timer > 0
                if (this->m_UndoRedoScrollTimer < 0)
                {
                    this->m_UndoRedoScrollTimer = 0;
                }
                this->m_UndoRedoScrollTimer += sba_Application->GetTimer()->GetElapsedSeconds();
            }
            else
            {
                this->m_UndoRedoScrollTimer = 0;
            }
            /*if (sba_Input->LastInputWasGamepad())
            {
            MoveInput *= PuRe_clamp((this->m_GamepadMovementSpeedupTimer) * 4 - 1, 0.75f, 5);
            }*/
            if (this->m_UndoRedoScrollTimer < -this->m_ScrollingStart || sba_Input->ButtonPressed(sba_Button::EditorUndo, this->m_PlayerIdx))
            { //Undo
                if (this->m_UndoRedoScrollTimer < -this->m_ScrollingStart)
                {
                    this->m_UndoRedoScrollTimer += this->m_ScrollingStep;
                }
                SHistoryStep* step = this->m_pHistory->Undo();
                if (step != nullptr)
                {
                    if (step->Delete)
                    { //Undo Brick Delete
                        sba_SoundPlayer->PlaySound("editor_set_brick", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                        CHistory::RecreateBrick(step->DeleteBrick_Step, *a_rShipHandler.GetCurrentSpaceShip(), *sba_World);
                        for (size_t i = 0; i < step->DeleteAdhesiveBricks_Steps.size(); i++)
                        {
                            CHistory::RecreateBrick(step->DeleteAdhesiveBricks_Steps[i], *a_rShipHandler.GetCurrentSpaceShip(), *sba_World);
                        }
                        a_rShipHandler.UpdateCurrentShipData();
                        this->m_CurrentHeightIsInvalid = true;
                    }
                    else
                    { //Undo Brick Placement
                        sba_SoundPlayer->PlaySound("editor_remove_block", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                        SAFE_DELETE(step->Brick.BrickInstance);
                        step->Brick.BrickInstance = nullptr;
                        a_rShipHandler.UpdateCurrentShipData();
                        this->m_CurrentHeightIsInvalid = true;
                    }
                }
            }
            else if (this->m_UndoRedoScrollTimer > this->m_ScrollingStart || sba_Input->ButtonPressed(sba_Button::EditorRedo, this->m_PlayerIdx))
            { //Redo
                if (this->m_UndoRedoScrollTimer > this->m_ScrollingStart)
                {
                    this->m_UndoRedoScrollTimer -= this->m_ScrollingStep;
                }
                SHistoryStep* step = this->m_pHistory->Redo();
                if (step != nullptr)
                {
                    if (step->Delete)
                    { //Redo Brick Delete
                        sba_SoundPlayer->PlaySound("editor_remove_block", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                        SAFE_DELETE(step->DeleteBrick_Step->Brick.BrickInstance);
                        step->DeleteBrick_Step->Brick.BrickInstance = nullptr;
                        for (size_t i = 0; i < step->DeleteAdhesiveBricks_Steps.size(); i++)
                        {
                            SAFE_DELETE(step->DeleteAdhesiveBricks_Steps[i]->Brick.BrickInstance);
                            step->DeleteAdhesiveBricks_Steps[i]->Brick.BrickInstance = nullptr;
                        }
                        a_rShipHandler.UpdateCurrentShipData();
                        this->m_CurrentHeightIsInvalid = true;
                    }
                    else
                    { //Redo Brick Placement
                        sba_SoundPlayer->PlaySound("editor_set_brick", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                        CHistory::RecreateBrick(step, *a_rShipHandler.GetCurrentSpaceShip(), *sba_World);
                        a_rShipHandler.UpdateCurrentShipData();
                        this->m_CurrentHeightIsInvalid = true;
                    }
                }
            }
        }

        if (sba_Input->ButtonPressed(sba_Button::EditorSaveShip, this->m_PlayerIdx))
        { //Safe Ship to file
            sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            if (this->m_pCurrentBrick != nullptr) //Delete CurrentBrick
            {
                SAFE_DELETE(this->m_pCurrentBrick);
                this->m_pCurrentBrick = nullptr;
            }
            a_rShipHandler.SaveCurrentShip(); //Save ship & preview
            //Recreate CurrentBrick
            this->m_pCurrentBrick = a_pCurrentBrick->CreateInstance(*this->m_pCurrentBrickObject, *sba_World); //Create Instance
            this->ApplyToCurrentBrick();
        }

#ifdef EDITOR_DEV
        if (sba_Application->GetInput()->KeyPressed(PuRe_IInput::F6))
        { //Safe Ship to file as object
            sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            SAFE_DELETE(this->m_pCurrentBrick);
            sba_ShipManager->SaveShipToFileAsObject(*a_rShipHandler.GetCurrentSpaceShip());
        }
#endif

        /*if (sba_Input->ButtonPressed(sba_Button::EditorResetShip, this->m_playerIdx))
        {         //Reset/Delete Ship
        a_rShipHandler.ResetCurrentShip();
        this->m_pHistory->Clear();
        a_rShipHandler.UpdateCurrentShipData();
        this->m_CurrentBrickHeightIstInvalid = true;
        }*/
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::RestoreAdhesiveBricksToDelete()
    {
        for (std::vector<BrickToDeleteCache>::iterator it = this->m_AdhesiveBricksToDelete.begin(); it != this->m_AdhesiveBricksToDelete.end(); ++it)
        {
            it->BrickInstance->m_Color = it->Color;
        }
    }
}