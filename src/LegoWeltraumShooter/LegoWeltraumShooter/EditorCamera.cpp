#include "include\EditorCamera.h"

namespace Game
{
  CEditorCamera::CEditorCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float32 a_FOV, float32 a_AspectRatio,
    PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection) : PuRe_Camera(a_Position, a_Direction, a_Up, a_FOV, a_AspectRatio, a_NearFar, a_Resolution, a_UsedProjection)
  {

  }

  CEditorCamera::CEditorCamera(PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection) : PuRe_Camera(a_Resolution, a_UsedProjection)
  {

  }

  CEditorCamera::~CEditorCamera()
  {

  }

  void CEditorCamera::Initialize()
  {
    this->SetFoV(45.0f);
    this->Move(PuRe_Vector3F(5.0f, 5.0f, -10.0f));
    this->Rotate(-20.0f, 20.0f, 0.0f);
  }

  void CEditorCamera::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer)
  {
    //Seconds for frame independent movement
    float32 Seconds = a_pTimer->GetElapsedSeconds();

    PuRe_Vector3F CameraMove;
    PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

    if (a_pInput->MousePressed(a_pInput->LeftClick))
    {
      this->MouseClickPosition = a_pInput->GetMousePosition();
    }
    if (a_pInput->MouseIsPressed(a_pInput->LeftClick))
    {

      PuRe_Vector2F speed = (a_pInput->GetMousePosition() - this->MouseClickPosition);
      //speed.Normalize();
      //speed *= 100.0f;
      PuRe_Vector3F cameraLook = PuRe_Vector3F();
      if (speed.X != 0.0f)
        cameraLook.X += speed.X*Seconds;
      if (speed.Y != 0.0f)
        cameraLook.Y += speed.Y*Seconds;
      this->Rotate(cameraLook.X, cameraLook.Y, cameraLook.Z);
    }
    float32 speed = 10.0f*Seconds;
    //Handle Movement
    if (a_pInput->KeyIsPressed(a_pInput->W))
      CameraMove.Z += speed;
    else if (a_pInput->KeyIsPressed(a_pInput->S))
      CameraMove.Z -= speed;
    if (a_pInput->KeyIsPressed(a_pInput->D))
      CameraMove.X += speed;
    else if (a_pInput->KeyIsPressed(a_pInput->A))
      CameraMove.X -= speed;
    if (a_pInput->KeyIsPressed(a_pInput->R))
      CameraMove.Y += speed;
    else if (a_pInput->KeyIsPressed(a_pInput->F))
      CameraMove.Y -= speed;

    this->Move(CameraMove);
  }
}