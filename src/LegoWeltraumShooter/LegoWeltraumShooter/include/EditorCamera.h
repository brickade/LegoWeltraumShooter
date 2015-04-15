#ifndef _EDITORCAMERA_H_
#define _EDITORCAMERA_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <math.h>

namespace Game
{
    class CEditorCamera : public PuRe_Camera
    {
    public:

        CEditorCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float32 a_FOV, float32 a_AspectRatio,
            PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int32 m_playerIdx);
        CEditorCamera(PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection, int32 m_playerIdx);
        ~CEditorCamera();

        void Initialize();
        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer);

    private:
        int32 m_playerIdx;
        float32 m_gamepadThreshold;
        float32 m_distance;
    };
}

#endif /* _EDITORCAMERA_H_ */