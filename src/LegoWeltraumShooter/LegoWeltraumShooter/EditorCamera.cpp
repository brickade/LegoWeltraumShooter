#include "include\EditorCamera.h"

namespace Game
{
    CEditorCamera::CEditorCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float32 a_FOV, float32 a_AspectRatio,
        PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int32 a_playerIdx) : PuRe_Camera(a_Position, a_Direction, a_Up, a_FOV, a_AspectRatio, a_NearFar, a_Resolution, a_UsedProjection)
    {
        this->m_playerIdx = a_playerIdx;
    }

    CEditorCamera::CEditorCamera(PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int32 a_playerIdx) : PuRe_Camera(a_Resolution, a_UsedProjection)
    {
        this->m_playerIdx = a_playerIdx;
    }

    CEditorCamera::~CEditorCamera()
    {

    }

    void CEditorCamera::Initialize()
    {
        this->SetFoV(45.0f);
        this->Move(PuRe_Vector3F(5.0f, 5.0f, -10.0f));
        this->Rotate(-20.0f, 20.0f, 0.0f);
        this->m_gamepadThreshold = 0.25f;
        this->m_distance = 10;
    }

    void CEditorCamera::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //Handle Movement
        PuRe_Vector2F MoveInput;
        float32 speed = a_pTimer->GetElapsedSeconds();

        //Gamepad
        float32 gamepadSpeed = speed * 1;
        MoveInput = a_pInput->GetGamepadRightThumb(this->m_playerIdx) * gamepadSpeed;
        if (std::abs(MoveInput.X) < this->m_gamepadThreshold)
        {
            MoveInput.X = 0;
        }
        if (std::abs(MoveInput.Y) < this->m_gamepadThreshold)
        {
            MoveInput.Y = 0;
        }

        //Keyboard
        float32 keyboardSpeed = speed * 100;
        if (a_pInput->KeyIsPressed(a_pInput->D))
            MoveInput.X += keyboardSpeed;
        else if (a_pInput->KeyIsPressed(a_pInput->A))
            MoveInput.X -= keyboardSpeed;
        if (a_pInput->KeyIsPressed(a_pInput->W))
            MoveInput.Y += keyboardSpeed;
        else if (a_pInput->KeyIsPressed(a_pInput->S))
            MoveInput.Y -= keyboardSpeed;

        //Mouse
        float32 mouseSpeed = speed * 10;
        PuRe_Vector2F pos = a_pInput->GetMousePosition();
        pos.X *= -1;
        if (a_pInput->MouseIsPressed(a_pInput->RightClick))
        {
            MoveInput += (pos - this->m_lastMousePosition) * mouseSpeed;
        }
        this->m_lastMousePosition = pos;

        //Apply
        MoveInput *= 0.5f;
        this->SetPosition(PuRe_Vector3F(0, 0, 0));
        this->Rotate(MoveInput.Y, -MoveInput.X, 0);
        this->Move(this->GetDirection() * -this->m_distance);
    }
}


