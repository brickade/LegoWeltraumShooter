#include "include\Editor_Camera.h"

namespace Editor
{
    CCamera::CCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float a_FOV, float a_AspectRatio,
        PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int a_playerIdx) : PuRe_Camera(a_Position, a_Direction, a_Up, a_FOV, a_AspectRatio, a_NearFar, a_Resolution, a_UsedProjection)
    {
        this->m_playerIdx = a_playerIdx;
    }

    CCamera::CCamera(PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int a_playerIdx) : PuRe_Camera(a_Resolution, a_UsedProjection)
    {
        this->m_playerIdx = a_playerIdx;
    }

    CCamera::~CCamera()
    {

    }

    void CCamera::Initialize(PuRe_Vector3F a_InitRotation, PuRe_Vector3F a_PositionOffset, float a_MinDistance, float a_MaxDistance, float a_InitDistance, float a_FoV)
    {
        this->SetFoV(a_FoV);
        this->m_MinDistance = a_MinDistance;
        this->m_MaxDistance = a_MaxDistance;
        this->m_PositionOffset = a_PositionOffset;
        this->Move(this->m_PositionOffset * (1.0f + (((this->m_distance - this->m_MinDistance) / (this->m_MaxDistance - this->m_MinDistance))) * 4));
        this->Rotate(a_InitRotation.X, a_InitRotation.Y, a_InitRotation.Z);
        this->Move(PuRe_Vector3F(0, 0, this->m_distance = a_InitDistance));
        this->m_gamepadThreshold = 0.25f;
    }

    void CCamera::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer)
    {
        //Handle Movement
        PuRe_Vector2F MoveInput;
        float speed = a_pTimer->GetElapsedSeconds();

        //----------Gamepad
        float gamepadSpeed = speed * 250;
        float gamepadZoomSpeed = gamepadSpeed * 0.1f;
        PuRe_Vector2F gamepadInput;
        gamepadInput = a_pInput->GetGamepadRightThumb(this->m_playerIdx);
        if (std::abs(gamepadInput.X) < this->m_gamepadThreshold)
        {
            gamepadInput.X = 0;
        }
        if (std::abs(gamepadInput.Y) < this->m_gamepadThreshold)
        {
            gamepadInput.Y = 0;
        }
        MoveInput = gamepadInput * gamepadSpeed;
        //Zoom
        this->m_distance += a_pInput->GetGamepadLeftTrigger(this->m_playerIdx) * gamepadZoomSpeed;
        this->m_distance -= a_pInput->GetGamepadRightTrigger(this->m_playerIdx) * gamepadZoomSpeed;

        //----------Keyboard
        float keyboardSpeed = speed * 100;
        float keyboardZoomSpeed = keyboardSpeed * 0.6f;
        if (a_pInput->KeyIsPressed(a_pInput->D))
            MoveInput.X += keyboardSpeed;
        else if (a_pInput->KeyIsPressed(a_pInput->A))
            MoveInput.X -= keyboardSpeed;
        if (a_pInput->KeyIsPressed(a_pInput->W))
            MoveInput.Y += keyboardSpeed;
        else if (a_pInput->KeyIsPressed(a_pInput->S) && !a_pInput->KeyIsPressed(a_pInput->Ctrl))
            MoveInput.Y -= keyboardSpeed;
        //Zoom
        if (a_pInput->KeyIsPressed(a_pInput->R))
            this->m_distance -= keyboardZoomSpeed;
        else if (a_pInput->KeyIsPressed(a_pInput->F))
            this->m_distance += keyboardZoomSpeed;

        //----------Mouse
        float mouseSpeed = speed * 35;
        float mouseZoomSpeed = mouseSpeed * 5;
        PuRe_Vector2F mouseDelta = a_pInput->GetRelativeMousePosition();
        mouseDelta.X *= -1;
        if (a_pInput->MouseIsPressed(a_pInput->RightClick))
        {
            MoveInput += mouseDelta * mouseSpeed;
        }
        //Zoom
        this->m_distance -= a_pInput->GetMouseScroll() * mouseZoomSpeed;

        //----------Apply
        MoveInput *= 0.5f;
        this->SetPosition(PuRe_Vector3F(0, 0, 0));
        this->Rotate(MoveInput.Y, -MoveInput.X, 0);

        this->m_distance = PuRe_clamp(this->m_distance, this->m_MinDistance, this->m_MaxDistance);
        PuRe_Vector3F rot = this->GetRotation();
        //Align
        /*float step = 0.785398163f;
        float target = fmod(rot.Y, step);
        if (target < step / 2)
        {
            target = rot.Y - target;
        }
        else
        {
            target = rot.Y + (step - target);
        }
        rot.Y = ((rot.Y * 2 + target) / 3 - target) * speed + target;*/

        rot.X = PuRe_clamp(rot.X, -89, 89);
        this->Move(this->m_PositionOffset * (1.0f + (((this->m_distance - this->m_MinDistance) / (this->m_MaxDistance - this->m_MinDistance))) * 4));
        this->SetRotation(rot);
        this->Move(PuRe_Vector3F(0, 0, -this->m_distance));
    }
}


