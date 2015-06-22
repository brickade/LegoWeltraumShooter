#include "include/Editor_Worker.h"

#include <algorithm>
#include "TheBrick/BrickInstance.h"
#include "TheBrick/Spaceship.h"
#include "include/Space.h"
#include "include/Editor_History.h"
#include <TheBrick/Conversion.h>
#include "include/Editor_ShipHandler.h"

#include "TheBrick/DebugDraw.h"
#include "include/Editor_Assistant.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CWorker::CWorker(int a_playerIdx)
    {
        this->m_playerIdx = a_playerIdx;
    }


    // **************************************************************************
    // **************************************************************************
    CWorker::~CWorker()
    {
        SAFE_DELETE(this->m_pCamera);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Initialize(PuRe_IGraphics& a_pGraphics)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics.GetDescription();
        this->m_pCamera = new CCamera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective, this->m_playerIdx);
        this->m_pCamera->Initialize(PuRe_Vector3F(20, 135, 0), PuRe_Vector3F(-1, 0, 0));
        this->m_currentPosition = PuRe_Vector2F(0, 0);
        this->m_currentBrickPosition = PuRe_Vector2I(0, 0);
        this->m_currentHeight = 0;
        this->m_maxBrickDistance = 15 * 3;
        this->m_pHistory = new CHistory(300, 100);
        this->m_placeBelow = false;
        this->m_pCurrentBrickObject = new TheBrick::CGameObject(*sba_World, nullptr);
        //this->m_pShipWorker = new CShipHandler();
        //this->m_pShipWorker->LoadShipFromFile("../data/ships/Banana.ship"); //Load Ship from file
        
        /*this->m_pGridMaterial = a_pGraphics.LoadMaterial("../data/effects/editor/grid");
        this->m_pGridBrick = new PuRe_Model(&a_pGraphics, "../data/models/Brick1X1.obj");*/
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick, PuRe_Color& a_rCurrentColor, CShipHandler& a_rShipHandler)
    {
        this->m_currentBrickColor = a_rCurrentColor;
        if (this->m_pCurrentBrick == nullptr || this->m_pCurrentBrick->m_pBrick != a_pCurrentBrick)
        {
            if (this->m_pCurrentBrick != nullptr)
            {
                SAFE_DELETE(this->m_pCurrentBrick);
            }
            this->m_pCurrentBrick = a_pCurrentBrick->CreateInstance(*this->m_pCurrentBrickObject, *sba_World); //Create Instance
        }
        this->m_pCamera->Update(&a_pGraphics, &a_pWindow, &a_pTimer);
        this->UpdateTranslation(this->m_pCamera->GetForward(), a_pTimer.GetElapsedSeconds() * 3.0f);
        this->UpdateRotation();
        this->UpdateHeight(a_rShipHandler);
        this->ApplyToCurrentBrick();
        this->UpdatePlacement(a_rShipHandler);

        //Safe Ship to file
        if (sba_Input->ButtonPressed(sba_Button::EditorSaveShip, this->m_playerIdx))
        {
            a_rShipHandler.SaveCurrentShip();
        }

        //Reset/Delete Ship
        if (sba_Input->ButtonPressed(sba_Button::EditorResetShip, this->m_playerIdx))
        {
            a_rShipHandler.ResetCurrentShip();
            this->m_pHistory->Clear();
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Render(CShipHandler& a_rShipHandler)
    {
        TheBrick::CSpaceship& ship = *a_rShipHandler.GetCurrentSpaceShip();
        sba_Space->RenderFont(std::to_string(ship.m_pBricks.size()) + "/100 Bricks", PuRe_Vector2F(sba_Width - 300.0f, sba_Height - 50.0f), 18);
        sba_Space->RenderFont("Ship: " + ship.GetName(), PuRe_Vector2F(sba_Width / 2 - 200.0f, sba_Height - 50.0f), 18);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateTranslation(PuRe_Vector3F a_cameraLook, float a_speed)
    {
        PuRe_Vector2F MoveInput = sba_Input->Direction(sba_Direction::EditorMoveBrick, this->m_playerIdx) * a_speed;
        if (a_cameraLook.Y > 0.6f)
        {
            MoveInput.Y *= -1;
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
            //Align
            //forward.Normalize();
            //forward.X = round(forward.X);
            //forward.Y = round(forward.Y);
        }

        //Set Input in the according forward direction
        this->m_currentPosition += forward * MoveInput.Y;
        this->m_currentPosition += PuRe_Vector2F(forward.Y, -forward.X) * MoveInput.X;
        this->m_currentPosition.X = PuRe_clamp(this->m_currentPosition.X, -this->m_maxBrickDistance, this->m_maxBrickDistance);
        this->m_currentPosition.Y = PuRe_clamp(this->m_currentPosition.Y, -this->m_maxBrickDistance, this->m_maxBrickDistance);

        this->m_currentBrickPosition = PuRe_Vector2I(round(this->m_currentPosition.X), round(this->m_currentPosition.Y)); //Snap to grid
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateRotation()
    {
        if (sba_Input->ButtonPressed(sba_Button::EditorRotateBrickRight, this->m_playerIdx))
        {
            this->m_currentBrickRotation++;
        }
        if (sba_Input->ButtonPressed(sba_Button::EditorRotateBrickLeft, this->m_playerIdx))
        {
            this->m_currentBrickRotation--;
        }
        this->m_currentBrickRotation += 4; //Avoid negative numbers
        this->m_currentBrickRotation = this->m_currentBrickRotation % 4; //Snap Rotation
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateHeight(CShipHandler& a_rShipHandler)
    {
        //Update Placement Direction
        if (sba_Input->ButtonPressed(sba_Button::EditorTogglePlacementSide, this->m_playerIdx))
        {
            this->m_placeBelow = !this->m_placeBelow;
        }

        //Set Brick up
        this->m_currentHeight = this->m_maxBrickDistance;
        if (this->m_placeBelow)
        {
            this->m_currentHeight *= -1;
        }
        this->ApplyToCurrentBrick();

        //Block placing
        this->m_canPlaceHere = false;

        const ong::Transform& brickTransform = ong::transformTransform(this->m_pCurrentBrick->GetTransform(), this->m_pCurrentBrick->GetGameObject()->GetTransform());

        //Casting Nub Requirements
        ong::vec3 nubToCastFromDirection = ong::vec3(0, -1, 0);
        bool nubToCastFromShouldBeMale = false;
        if (this->m_placeBelow)
        {
            nubToCastFromShouldBeMale = true;
            nubToCastFromDirection.y *= -1;
        }

        //Get RayCastHit
        std::vector<ong::RayQueryResult> hitResults;
        std::vector<ong::vec3> hitResultRayOrigins;
        if (!CAssistant::GetClosestHitsFromBrickInstanceNubs(*this->m_pCurrentBrick, *a_rShipHandler.GetCurrentSpaceShip(), nubToCastFromShouldBeMale, nubToCastFromDirection, &hitResults, &hitResultRayOrigins))
        { //Nothing hit
            this->m_currentHeight = 0;
            printf("Nothing hit\n");
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
                    this->m_canPlaceHere = true;
                    this->m_currentHeight = round((hitResultRayOrigins[i].y + maxCollisionFreeDelta.y + (brickTransform.p.y - hitResultRayOrigins[i].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);
                    printf("Collision free: delta(%f, %f, %f)\n", maxCollisionFreeDelta.x, maxCollisionFreeDelta.y, maxCollisionFreeDelta.z);
                    return;
                }
                //Handle docking test success but not collision free
                this->m_currentHeight = round((hitResultRayOrigins[i].y + maxCollisionFreeDelta.y + (brickTransform.p.y - hitResultRayOrigins[i].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);
                printf("NOT Collision free: delta(%f, %f, %f)\n", maxCollisionFreeDelta.x, maxCollisionFreeDelta.y, maxCollisionFreeDelta.z);
                return;
            }
        }
        //Can't dock
        printf("Can't dock\n");
        this->m_currentHeight = round((hitResults[0].point.y + (brickTransform.p.y - hitResultRayOrigins[0].y)) / TheBrick::CBrick::SEGMENT_HEIGHT);
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
        transform.p = ong::vec3(this->m_currentBrickPosition.X * TheBrick::CBrick::SEGMENT_WIDTH, this->m_currentHeight * TheBrick::CBrick::SEGMENT_HEIGHT, this->m_currentBrickPosition.Y * TheBrick::CBrick::SEGMENT_WIDTH);
        transform.q = ong::Quaternion(ong::vec3(0, 0, 0), 1);
        this->m_pCurrentBrick->SetTransform(transform);
        this->m_pCurrentBrick->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, this->m_currentBrickRotation * 90.0f * 0.0174532925f, 0.0f));
        //#define ALPHAREADY
        if (this->m_canPlaceHere)
        {
#ifdef ALPHAREADY
            this->m_pCurrentBrick->m_Color = PuRe_Color(this->m_currentBrickColor.R, this->m_currentBrickColor.G, this->m_currentBrickColor.B, this->m_currentBrickColor.A * 0.6f);
#else
            this->m_pCurrentBrick->m_Color = PuRe_Color(this->m_currentBrickColor.R * 1.5f, this->m_currentBrickColor.G * 1.5f, this->m_currentBrickColor.B * 1.5f, 1);
#endif
        }
        else
        {
#ifdef ALPHAREADY
            this->m_pCurrentBrick->m_Color = PuRe_Color(1, 0, 0, 0.6f);
#else
            this->m_pCurrentBrick->m_Color = PuRe_Color(1, 0, 0, 1);
#endif
    }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdatePlacement(CShipHandler& a_rShipHandler)
    {

        if (sba_Input->ButtonIsPressed(sba_Button::EditorUndoRedoHold, this->m_playerIdx) && sba_Input->ButtonPressed(sba_Button::EditorUndo, this->m_playerIdx))
        { //Undo
            SHistoryStep* step = this->m_pHistory->Undo();
            if (step != nullptr)
            {
                delete step->BrickInstance;
            }
        }
        else if (sba_Input->ButtonIsPressed(sba_Button::EditorUndoRedoHold, this->m_playerIdx) && sba_Input->ButtonPressed(sba_Button::EditorRedo, this->m_playerIdx))
        { //Redo
            SHistoryStep* step = this->m_pHistory->Redo();
            if (step != nullptr)
            {
                step->BrickInstance = new TheBrick::CBrickInstance(*step->Brick, *a_rShipHandler.GetCurrentSpaceShip(), *sba_World, step->Color);
                step->BrickInstance->SetTransform(step->Transform);
            }
        }
        if (!this->m_canPlaceHere)
        {
            return;
        }

        if (sba_Input->ButtonPressed(sba_Button::EditorPlaceBrick, this->m_playerIdx) && a_rShipHandler.GetCurrentSpaceShip()->m_pBricks.size() < 100)
        { //Place BrickInstance
            this->m_pCurrentBrick->m_Color = this->m_currentBrickColor; //Apply right color
            this->m_pHistory->CutRedos();
            SHistoryStep step;
            step.BrickInstance = a_rShipHandler.AddBrickInstanceToCurrentShip(*this->m_pCurrentBrick); //Add to ship
            step.Brick = step.BrickInstance->m_pBrick;
            step.Transform = this->m_pCurrentBrick->GetTransform();
            step.Color = this->m_currentBrickColor;
            this->m_pHistory->AddStep(step); //Add History step
        }
    }
}