#include "include/BrickWorker.h"
namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CBrickWorker::CBrickWorker(int32 a_playerIdx)
    {
        this->m_playerIdx = a_playerIdx;
    }


    // **************************************************************************
    // **************************************************************************
    CBrickWorker::~CBrickWorker()
    {
        SAFE_DELETE(this->m_pCurrentBrick);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickWorker::Initialize(PuRe_IGraphics* a_pGraphics)
    {
        this->m_currentPosition = PuRe_Vector2F(0, 0);
        this->m_currentHeight = 0;
        
        this->m_pTmpBrickMat = a_pGraphics->LoadMaterial("../data/effects/editor/default");
        this->m_pCurrentBrick = new PuRe_Model(a_pGraphics, this->m_pTmpBrickMat, "../data/models/brick1.obj");
        
        this->m_pGridMaterial = a_pGraphics->LoadMaterial("../data/effects/editor/grid");
        this->m_pGridBrick = new PuRe_Model(a_pGraphics, this->m_pGridMaterial, "../data/models/brick1.obj");
        
        this->m_maxBrickDistance = 15;

        this->lastInputIsGamepad = false;
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickWorker::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, PuRe_Vector3F a_cameraLook)
    {
        //Handle Movement
        PuRe_Vector2F MoveInput;
        float32 speed = a_pTimer->GetElapsedSeconds();

        //----------Gamepad
        float32 gamepadSpeed = speed * 10;
        PuRe_Vector2F gamepadInput;
        if (a_pInput->GamepadIsPressed(a_pInput->DPAD_Right, this->m_playerIdx))
        {
            gamepadInput.X += 1;
        }
        if (a_pInput->GamepadIsPressed(a_pInput->DPAD_Left, this->m_playerIdx))
        {
            gamepadInput.X -= 1;
        }
        if (a_pInput->GamepadIsPressed(a_pInput->DPAD_Up, this->m_playerIdx))
        {
            gamepadInput.Y += 1;
        }
        if (a_pInput->GamepadIsPressed(a_pInput->DPAD_Down, this->m_playerIdx))
        {
            gamepadInput.Y -= 1;
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
        if (!a_pInput->MouseIsPressed(a_pInput->LeftClick) && !a_pInput->MouseIsPressed(a_pInput->RightClick))
        {
            float32 mouseSpeed = speed;
            PuRe_Vector2F mouseDelta = a_pInput->GetRelativeMousePosition();
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
            float32 alpha = 45;
            if (PuRe_Vector2F::Dot(forwardN.Side(), forwardRaw) > 0) //Detect right direction
            {
                alpha *= -1;
            }
            float32 cosAlpha = cos(alpha * 0.0174532925f);
            float32 sinAlpha = sin(alpha * 0.0174532925f);
            forward = PuRe_Vector2F(cosAlpha*forward.X - sinAlpha*forward.Y, sinAlpha*forward.X + cosAlpha*forward.Y); //Rotate
            //Align
            //forward.Normalize();
            //forward.X = round(forward.X);
            //forward.Y = round(forward.Y);
        }

        //Set Input in the according forward direction
        this->m_currentPosition += forward * MoveInput.Y;
        this->m_currentPosition += PuRe_Vector2F(forward.Y, -forward.X) * MoveInput.X;
        this->m_currentPosition.X = clamp(this->m_currentPosition.X, -this->m_maxBrickDistance, this->m_maxBrickDistance);
        this->m_currentPosition.Y = clamp(this->m_currentPosition.Y, -this->m_maxBrickDistance, this->m_maxBrickDistance);

        this->m_currentBrickPosition = this->m_currentPosition;

        //Snap to grid
        this->m_currentBrickPosition.X = this->m_currentBrickPosition.X - fmod(this->m_currentBrickPosition.X, BRICK_WIDTH);
        this->m_currentBrickPosition.Y = this->m_currentBrickPosition.Y - fmod(this->m_currentBrickPosition.Y, BRICK_WIDTH);

        //printf("brickpos:%f,%f\n", this->m_currentBrickPosition.X, this->m_currentBrickPosition.Y);

        //----------------------------
        //----------Rotation----------
        //----------------------------

        float32 rotationPerClick = 90.0f * 0.0174532925f;
        //----------Gamepad
        if (a_pInput->GamepadPressed(a_pInput->Right_Shoulder, this->m_playerIdx))
        {
            this->m_currentRotation += rotationPerClick;
        }
        if (a_pInput->GamepadPressed(a_pInput->Left_Shoulder, this->m_playerIdx))
        {
            this->m_currentRotation -= rotationPerClick;
        }

        //----------Keyboard
        if (a_pInput->KeyPressed(a_pInput->E))
        {
            this->m_currentRotation += rotationPerClick;
        }
        if (a_pInput->KeyPressed(a_pInput->Q))
        {
            this->m_currentRotation -= rotationPerClick;
        }

        this->m_currentBrickRotation = this->m_currentRotation;
        //float32 rotationSpeed = speed * 1;
        //this->m_currentBrickRotation += round(clamp(this->m_currentBrickRotation - this->m_currentRotation, 0, 1)) * rotationSpeed;
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickWorker::Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        //Grid
        this->m_pGridMaterial->Apply();
        this->m_pGridMaterial->SetVector3(PuRe_Vector3F(0.7f, 0.2f, 0.2f), "brickColor");
        for (int x = -this->m_maxBrickDistance; x < this->m_maxBrickDistance; x++)
        {
            for (int z = -this->m_maxBrickDistance; z < this->m_maxBrickDistance; z++)
            {
                this->m_pGridBrick->Draw(a_pCamera, PuRe_Primitive::Triangles, PuRe_Vector3F(x*BRICK_WIDTH, -BRICK_HEIGHT + PLATE_HEIGHT, z*BRICK_WIDTH), PuRe_Vector3F(1.0f, 1.0f, 1.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f));
            }
        }

        //Current Brick
        this->m_pTmpBrickMat->Apply();
        this->m_pTmpBrickMat->SetVector3(PuRe_Vector3F(0.5f, 0.6f, 1.0f), "brickColor");
        this->m_pCurrentBrick->Draw(a_pCamera, PuRe_Primitive::Triangles, PuRe_Vector3F(this->m_currentBrickPosition.X, this->m_currentHeight, this->m_currentBrickPosition.Y), PuRe_Vector3F(1.0f, 1.0f, 1.0f), PuRe_Vector3F(0.0f, this->m_currentBrickRotation, 0.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f));
    }
}