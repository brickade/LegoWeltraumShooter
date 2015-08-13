#ifndef _EDITORCAMERA_H_
#define _EDITORCAMERA_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <math.h>

namespace Editor
{
    class CCamera : public PuRe_Camera
    {
    public:

    private:
        int m_playerIdx;
        float m_MaxDistance;
        float m_MinDistance;
        float m_distance;
        PuRe_Vector3F m_PositionOffset;

    public:
        void SetDistance(float a_Distance)
        {
            this->m_distance = a_Distance;
        }

    public:
        CCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float a_FOV, float a_AspectRatio,
            PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int m_playerIdx);
        CCamera(PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int m_playerIdx);
        ~CCamera();

        void Initialize(PuRe_Vector3F a_InitRotation = PuRe_Vector3F(0, 0, 0), PuRe_Vector3F a_PositionOffset = PuRe_Vector3F(5.0f, 5.0f, -10.0f), float a_MinDistance = 10.0f, float a_MaxDistance = 65.0f, float a_InitDistance = 35.0f, float a_FoV = 45.0f);
        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_Timer* a_pTimer, bool a_IgnoreInput = false);
    };
}

#endif /* _EDITORCAMERA_H_ */