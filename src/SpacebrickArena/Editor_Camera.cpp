#include "include\Editor_Camera.h"
#include "include/Space.h"
#include "include/InputManager.h"

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
    }

    void CCamera::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_Timer* a_pTimer, bool a_IgnoreInput)
    {   
        float speed = a_pTimer->GetElapsedSeconds() * 100;
        float zoomSpeed = speed * 0.5f;

        //Rotation
        PuRe_Vector2F MoveInput = sba_Input->Direction(sba_Direction::EditorCameraRotate, this->m_playerIdx) * speed * -1.0f;

        //Zoom
        this->m_distance += sba_Input->Axis(sba_Axis::EditorCameraZoom, this->m_playerIdx) * zoomSpeed * -1.0f;

        if (a_IgnoreInput)
        {
            MoveInput = PuRe_Vector2F::Zero();
            this->m_distance = this->m_MinDistance + (this->m_MaxDistance - this->m_MinDistance) * 0.5f;
        }

        //----------Apply
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


