#include "include/Editor_Worker.h"

#include <algorithm>
#include "TheBrick/BrickInstance.h"
#include "TheBrick/Brick.h"
#include "TheBrick/Spaceship.h"
#include "include/Space.h"
#include "include/Editor_History.h"
#include <TheBrick/Conversion.h>
#include "include\Editor_ShipWorker.h"

#include "TheBrick/DebugDraw.h"

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
        this->m_pCamera->Initialize(PuRe_Vector3F(20, 135, 0), PuRe_Vector3F(0,0,0));
        this->m_currentPosition = PuRe_Vector2F(0, 0);
        this->m_currentHeight = 0;

        /*this->m_pGridMaterial = a_pGraphics.LoadMaterial("../data/effects/editor/grid");
        this->m_pGridBrick = new PuRe_Model(&a_pGraphics, "../data/models/Brick1X1.obj");*/

        this->m_maxBrickDistance = 15;

        this->lastInputIsGamepad = false;

        this->m_pHistory = new CHistory(300, 100);

        this->m_placeBelow = false;

        this->m_pShipWorker = new CShipWorker();
        //Load Ship from file
        this->m_pShipWorker->LoadShipFromFile("../data/ships/banana.ship");

        this->m_pCurrentBrickObject = new TheBrick::CGameObject(*sba::Space::Instance()->World, nullptr);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_IInput& a_pInput, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick, PuRe_Color& a_rCurrentColor)
    {
        this->m_currentBrickColor = a_rCurrentColor;
        if (this->m_pCurrentBrick == nullptr || this->m_pCurrentBrick->m_pBrick != a_pCurrentBrick)
        {
            if (this->m_pCurrentBrick != nullptr)
            {
                SAFE_DELETE(this->m_pCurrentBrick);
            }
            this->m_pCurrentBrick = a_pCurrentBrick->CreateInstance(*this->m_pCurrentBrickObject, *sba::Space::Instance()->World); //Create Instance
        }
        this->m_pCamera->Update(&a_pGraphics, &a_pWindow, &a_pInput, &a_pTimer);
        this->UpdateTranslation(a_pInput, this->m_pCamera->GetForward(), a_pTimer.GetElapsedSeconds());
        this->UpdateRotation(a_pInput, 90.0f * 0.0174532925f);
        this->UpdateHeight(a_pInput);
        this->ApplyToCurrentBrick();
        this->UpdatePlacement(a_pInput);

        //Safe Ship to file
        if (a_pInput.KeyIsPressed(a_pInput.Ctrl) && a_pInput.KeyPressed(a_pInput.S))
        {
            this->m_pShipWorker->SaveShipToFile("../data/ships/banana.ship");
        }

        //Reset/Delete Ship
        if (a_pInput.KeyPressed(a_pInput.Backspace))
        {
            this->m_pShipWorker->ResetShip();
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::Render()
    {
        PuRe_Renderer* renderer = sba::Space::Instance()->Renderer;
        //Grid
        /*for (int x = -this->m_maxBrickDistance; x < this->m_maxBrickDistance; x++)
        {
            for (int z = -this->m_maxBrickDistance; z < this->m_maxBrickDistance; z++)
            {
                renderer->Draw(this->m_pGridBrick, PuRe_Primitive::Triangles, this->m_pGridMaterial, PuRe_Vector3F(x* TheBrick::CBrick::SEGMENT_WIDTH, 0, z* TheBrick::CBrick::SEGMENT_WIDTH), PuRe_MatrixF::Identity(), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f), PuRe_Color(0.7f, 0.2f, 0.2f));
            }
        }*/
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateTranslation(PuRe_IInput& a_pInput, PuRe_Vector3F a_cameraLook, float a_speed)
    {
        PuRe_Vector2F MoveInput;

        //----------Gamepad
        float gamepadSpeed = a_speed * 10;
        PuRe_Vector2F gamepadInput = a_pInput.GetGamepadLeftThumb(this->m_playerIdx);
        if (gamepadInput.Length() < 0.25f)
        {
            gamepadInput = PuRe_Vector2F::Zero();
        }
        MoveInput = gamepadInput * gamepadSpeed;
        if (gamepadInput.Length() > 0)
        {
            this->lastInputIsGamepad = true;
        }
        else
        {
            this->lastInputIsGamepad = false;
        }

        //----------Mouse
        if (/*!a_pInput->MouseIsPressed(a_pInput->LeftClick) && */!a_pInput.MouseIsPressed(a_pInput.RightClick))
        {
            float mouseSpeed = a_speed * 1.5f;
            PuRe_Vector2F mouseDelta = a_pInput.GetRelativeMousePosition();
            mouseDelta.Y *= -1;
            //Apply
            MoveInput += mouseDelta * mouseSpeed;
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
        if (this->lastInputIsGamepad && abs(abs(forward.X) - abs(forward.Y)) < 0.1f)
        {
            float alpha = 45;
            if (PuRe_Vector2F::Dot(forwardN.Side(), forwardRaw) > 0) //Detect right direction
            {
                alpha *= -1;
            }
            float cosAlpha = cos(alpha * 0.0174532925f);
            float sinAlpha = sin(alpha * 0.0174532925f);
            /*PuRe_Vector3F tmp = PuRe_Vector3F(forward.X, 0, forward.Y);
            tmp *= PuRe_QuaternionF(0, alpha, 0);
            forward = PuRe_Vector2F(tmp.X, tmp.Z);*/
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

        this->m_currentBrickPosition = this->m_currentPosition;

        //Snap to grid
        this->m_currentBrickPosition.X = this->m_currentBrickPosition.X - fmod(this->m_currentBrickPosition.X, TheBrick::CBrick::SEGMENT_WIDTH);
        this->m_currentBrickPosition.Y = this->m_currentBrickPosition.Y - fmod(this->m_currentBrickPosition.Y, TheBrick::CBrick::SEGMENT_WIDTH);
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateRotation(PuRe_IInput& a_pInput, float a_rotationPerClick)
    {
        //----------Gamepad
        if (a_pInput.GamepadPressed(a_pInput.Right_Shoulder, this->m_playerIdx))
        {
            this->m_currentRotation += a_rotationPerClick;
        }
        if (a_pInput.GamepadPressed(a_pInput.Left_Shoulder, this->m_playerIdx))
        {
            this->m_currentRotation -= a_rotationPerClick;
        }

        //----------Keyboard
        if (a_pInput.KeyPressed(a_pInput.E))
        {
            this->m_currentRotation += a_rotationPerClick;
        }
        if (a_pInput.KeyPressed(a_pInput.Q))
        {
            this->m_currentRotation -= a_rotationPerClick;
        }

        this->m_currentBrickRotation = this->m_currentRotation;
        this->m_currentBrickRotation = this->m_currentBrickRotation - fmod(this->m_currentBrickRotation, 1.57079633f); //Snap Rotation
    }

    // **************************************************************************
    // **************************************************************************
    void CWorker::UpdateHeight(PuRe_IInput& a_pInput)
    {
        if (a_pInput.GamepadPressed(a_pInput.Pad_B, this->m_playerIdx) || a_pInput.KeyPressed(a_pInput.Space))
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
        this->m_canPlaceHere = false; //Block placing

        const ong::Transform& brickTransform = ong::transformTransform(this->m_pCurrentBrick->GetTransform(), this->m_pCurrentBrick->GetGameObject()->GetTransform());
        std::vector<TheBrick::SNub>& nubs = this->m_pCurrentBrick->m_pBrick->GetNubs(); //Get currentBrick Nubs
        ong::RayQueryResult hitResult;
        hitResult.t = FLT_MAX;

        ong::vec3 dockingNubDirection = ong::vec3(0, -1, 0);
        bool dockingNubShouldBeMale = false;
        if (this->m_placeBelow)
        {
            dockingNubShouldBeMale = true;
            dockingNubDirection.y *= -1;
        }
        ong::Body& shipBody = *this->m_pShipWorker->GetCurrentSpaceShip()->m_pBody;
        bool hit = false;
        for (size_t i = 0; i < nubs.size(); i++) //Go through currentBrick Nubs
        {
            const ong::vec3 rayDir = TheBrick::PuReToOng(this->m_pCurrentBrick->DirToWorldSpace(nubs[i].Direction)); //Transform nubDirection to WorldSpace
            if (nubs[i].isMale != dockingNubShouldBeMale || dot(ong::normalize(rayDir), dockingNubDirection) < 0.99f)
            { //Wrong Direction
                continue;
            }
            const ong::vec3 rayOrigin = TheBrick::PuReToOng(this->m_pCurrentBrick->PosToWorldSpace(nubs[i].Position)); //Transform nubPosition to WorldSpace

            ong::RayQueryResult hs = { 0 }; //Allocate
            hs.t = FLT_MAX;
            //--------------------------------------------------------------------------------
            if (shipBody.queryRay(rayOrigin, rayDir, &hs)) //Cast Ray
            { //Ray Hit
                hit = true;
                //printf("RayPointHeight %i: %i\n", i, (int)(hs.point.y / TheBrick::CBrick::SEGMENT_HEIGHT));
                if (hs.t < hitResult.t)
                { //This Hit is nearer to the brick than the saved one
                    hitResult = hs;
                }
            }
        }
        if (!hit)
        { //Nothing hit
            this->m_currentHeight = 0;
            return;
        }
        TheBrick::CBrickInstance* brickInstance = reinterpret_cast<TheBrick::CBrickInstance*>(hitResult.collider->getUserData()); //Get hit BrickInstance
        //Transform hitResult direction to brickInstance BrickSpace
        ong::vec3 dockingdir = TheBrick::PuReToOng(brickInstance->DirToBrickSpace(TheBrick::OngToPuRe(hitResult.normal)));
        //Get hitBrick Nub at that position
        TheBrick::SNub* nub = brickInstance->GetNubAtWorldPos(TheBrick::OngToPuRe(hitResult.point), this->m_nubDockThreshold);
        assert(nub != nullptr);
        if (nub->isMale == dockingNubShouldBeMale || dot(ong::normalize(TheBrick::PuReToOng(nub->Direction)), dockingdir) < 0.99f)
        { //Wrong Direction
            return;
        }
        this->m_canPlaceHere = true; //Enable placing
        float heightOffset = 0;
        if (this->m_placeBelow)
        {
            heightOffset = -TheBrick::CBrick::SEGMENT_HEIGHT * 3;
        }
        this->m_currentHeight = TheBrick::OngToPuRe(hitResult.point).Y + heightOffset; //Set Brick Height;
        this->m_currentHeight = this->m_currentHeight - fmod(this->m_currentHeight, TheBrick::CBrick::SEGMENT_HEIGHT); //Snap Height
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
        transform.p = ong::vec3(this->m_currentBrickPosition.X, this->m_currentHeight, this->m_currentBrickPosition.Y);
        transform.q = ong::Quaternion(ong::vec3(0, 0, 0), 1);
        this->m_pCurrentBrick->SetTransform(transform);
        this->m_pCurrentBrick->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, this->m_currentBrickRotation, 0.0f));
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
    void CWorker::UpdatePlacement(PuRe_IInput& a_pInput)
    {
        if (a_pInput.GamepadPressed(a_pInput.Pad_Y, this->m_playerIdx) || (a_pInput.KeyIsPressed(a_pInput.Ctrl) && a_pInput.KeyPressed(a_pInput.Z)))
        { //Undo
            SHistoryStep* step = this->m_pHistory->Undo();
            if (step != nullptr)
            {
                delete step->BrickInstance;
            }
        }
        else if (a_pInput.GamepadPressed(a_pInput.Pad_X, this->m_playerIdx) || (a_pInput.KeyIsPressed(a_pInput.Ctrl) && a_pInput.KeyPressed(a_pInput.Y)))
        { //Redo
            SHistoryStep* step = this->m_pHistory->Redo();
            if (step != nullptr)
            {
                step->BrickInstance = new TheBrick::CBrickInstance(*step->Brick, *this->m_pCurrentBrickObject, *sba::Space::Instance()->World, step->Color);
                step->BrickInstance->SetTransform(step->Transform);
            }
        }
        if (!this->m_canPlaceHere)
        {
            return;
        }
        //Gamepad & Mouse
        if (a_pInput.GamepadPressed(a_pInput.Pad_A, this->m_playerIdx) || a_pInput.MousePressed(a_pInput.LeftClick))
        {
            this->m_pCurrentBrick->m_Color = this->m_currentBrickColor; //Apply right color
            this->m_pHistory->CutRedos();
            SHistoryStep step;
            step.BrickInstance = this->m_pShipWorker->AddBrickInstanceToShip(*this->m_pCurrentBrick);;
            step.Brick = step.BrickInstance->m_pBrick;
            step.Transform = this->m_pCurrentBrick->GetTransform();
            step.Color = this->m_currentBrickColor;
            this->m_pHistory->AddStep(step);
        }
    }
}