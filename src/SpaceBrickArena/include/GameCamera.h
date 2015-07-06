#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Spaceship.h"

namespace sba
{
  class CGameCamera : public PuRe_Camera
  {

  private:
      float m_ZOffset;
      PuRe_QuaternionF m_QRotation;
      PuRe_QuaternionF m_QAim;
      PuRe_Vector3F m_CamRotation;
      float m_TimeToRotate;

  public:

    CGameCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float a_FOV, float a_AspectRatio,
      PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection);
    CGameCamera(PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection);
    ~CGameCamera();

    void Initialize();
    void UpdateData(int a_CID, sba::CSpaceship* a_pPlayer, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer);

    float getZOffset();

    PuRe_QuaternionF GetAim();


  };
}

#endif /* _GAMECAMERA_H_ */