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

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickWorker::Initialize()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickWorker::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, PuRe_Vector3F a_cameraLook)
    {
        //Handle Movement
        PuRe_Vector2F MoveInput;
        float32 speed = a_pTimer->GetElapsedSeconds();

        //----------Gamepad
        float32 gamepadSpeed = speed * 250;
        PuRe_Vector2F gamepadInput;
        gamepadInput = a_pInput->GetGamepadLeftThumb(this->m_playerIdx);
        if (std::abs(gamepadInput.X) < this->m_gamepadThreshold)
        {
            gamepadInput.X = 0;
        }
        if (std::abs(gamepadInput.Y) < this->m_gamepadThreshold)
        {
            gamepadInput.Y = 0;
        }
        MoveInput = gamepadInput * gamepadSpeed;

        //----------Mouse
        if (!a_pInput->MouseIsPressed(a_pInput->LeftClick) && !a_pInput->MouseIsPressed(a_pInput->RightClick))
        {
            float32 mouseSpeed = speed * 35;
            PuRe_Vector2F mouseDelta = a_pInput->GetRelativeMousePosition();
            MoveInput += mouseDelta * mouseSpeed;
        }

        //----------Apply
        PuRe_Vector2F forward = PuRe_Vector2F(a_cameraLook.X, a_cameraLook.Z);
        //forward.Normalize();
        //forward.X = (int)forward.X;
        //forward.Y = (int)forward.Y;
        //forward.Normalize();
        printf("BrickWorker_Look: %f, %f\n", forward.X, forward.Y);


    }

    // **************************************************************************
    // **************************************************************************
    void CBrickWorker::Render(PuRe_IGraphics* a_pGraphics)
    {

    }
}