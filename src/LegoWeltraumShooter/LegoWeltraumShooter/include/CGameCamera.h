#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>


namespace Game
{
  class CGameCamera : public PuRe_Camera
  {
  public:

    CGameCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float32 a_FOV, float32 a_AspectRatio,
      PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection);
    ~CGameCamera();

    void Initialize();
    void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer);

  private:
    PuRe_Vector2F MouseClickPosition;

  };
}

#endif /* _GAMECAMERA_H_ */