#include "include\GameCamera.h"

#include "TheBrick/Conversion.h"

namespace Game
{
    CGameCamera::CGameCamera(PuRe_Vector3F a_Position, PuRe_Vector3F a_Direction, PuRe_Vector3F a_Up, float a_FOV, float a_AspectRatio,
        PuRe_Vector2F a_NearFar, PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection) : PuRe_Camera(a_Position, a_Direction, a_Up, a_FOV, a_AspectRatio, a_NearFar, a_Resolution, a_UsedProjection)
    {
    }

    CGameCamera::CGameCamera(PuRe_Vector2F a_Resolution, PuReEngine::Core::CameraProjection a_UsedProjection) : PuRe_Camera(a_Resolution, a_UsedProjection)
    {

    }

    CGameCamera::~CGameCamera()
    {

    }

    void CGameCamera::Initialize()
    {
        this->SetFoV(45.0f);
        this->m_ZOffset = 40.0f;
        this->setNearFar(PuRe_Vector2F(0.1f, 1000.0f));
        this->m_Rotation = PuRe_QuaternionF();
    }

    void CGameCamera::Update(int a_CID, TheBrick::CSpaceship* a_pPlayer, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer)
    {
        //Seconds for frame independent movement
        float Seconds = a_pTimer->GetElapsedSeconds();
        //PuRe_Vector3F CameraMove;
        //if (a_pInput->MouseIsPressed(a_pInput->LeftClick))
        //{

        //    PuRe_Vector2F speed = a_pInput->GetRelativeMousePosition();
        //    speed *= 10.0f;
        //    PuRe_Vector3F cameraLook = PuRe_Vector3F();
        //    if (speed.X != 0.0f)
        //        cameraLook.X += speed.X*Seconds;
        //    if (speed.Y != 0.0f)
        //        cameraLook.Y += speed.Y*Seconds;
        //    this->Rotate(cameraLook.Y, cameraLook.X, cameraLook.Z);
        //}
        //float speed = 10.0f*Seconds;
        ////Handle Movement
        //if (a_pInput->KeyIsPressed(a_pInput->W))
        //    CameraMove.Z += speed;
        //else if (a_pInput->KeyIsPressed(a_pInput->S))
        //    CameraMove.Z -= speed;
        //if (a_pInput->KeyIsPressed(a_pInput->D))
        //    CameraMove.X += speed;
        //else if (a_pInput->KeyIsPressed(a_pInput->A))
        //    CameraMove.X -= speed;

        //this->Move(CameraMove);

        this->m_ZOffset += a_pInput->GetMouseScroll();
        if (this->m_ZOffset < 10.0f)
            this->m_ZOffset = 10.0f;
        else if (this->m_ZOffset > 60.0f)
            this->m_ZOffset = 60.0f;

        this->SetPosition(TheBrick::OngToPuRe(a_pPlayer->m_pBody->getWorldCenter()));

        PuRe_Vector3F cameraLook = PuRe_Vector3F();

        if (a_pInput->MouseIsPressed(a_pInput->LeftClick))
        {

            PuRe_Vector2F speed = a_pInput->GetRelativeMousePosition() * 10.0f;

            if (speed.X != 0.0f)
                cameraLook.X += speed.X*Seconds;
            if (speed.Y != 0.0f)
                cameraLook.Y += speed.Y*Seconds;
        }


        PuRe_Vector2F Cam;
        std::string Key = a_pPlayer->m_pCSVFile->GetValue("Camera.X");
        bool invert = false;
        if (Key.substr(0, 1) == "-")
        {
            Key = Key.substr(1, Key.length());
            invert = true;
        }
        if (Key == "LeftThumb.X")
            Cam.X = a_pInput->GetGamepadLeftThumb(a_CID).X;
        else if (Key == "RightThumb.X")
            Cam.X = a_pInput->GetGamepadRightThumb(a_CID).X;
        else if (Key == "LeftThumb.Y")
            Cam.X = a_pInput->GetGamepadLeftThumb(a_CID).Y;
        else if (Key == "RightThumb.Y")
            Cam.X = a_pInput->GetGamepadRightThumb(a_CID).Y;
        if (invert)
            Cam.X = -Cam.X;

        Key = a_pPlayer->m_pCSVFile->GetValue("Camera.Y");
        invert = false;
        if (Key.substr(0, 1) == "-")
        {
            Key = Key.substr(1, Key.length());
            invert = true;
        }
        if (Key == "LeftThumb.X")
            Cam.Y = a_pInput->GetGamepadLeftThumb(a_CID).X;
        else if (Key == "RightThumb.X")
            Cam.Y = a_pInput->GetGamepadRightThumb(a_CID).X;
        else if (Key == "LeftThumb.Y")
            Cam.Y = a_pInput->GetGamepadLeftThumb(a_CID).Y;
        else if (Key == "RightThumb.Y")
            Cam.Y = a_pInput->GetGamepadRightThumb(a_CID).Y;
        if (invert)
            Cam.Y = -Cam.Y;

        PuRe_QuaternionF quat = TheBrick::OngToPuRe(a_pPlayer->m_pBody->getOrientation());
        if (this->m_Rotation != quat)
        {
            PuRe_QuaternionF diff = quat - this->m_Rotation;
            this->m_Rotation.X += diff.X / 100.0f;
            this->m_Rotation.Y += diff.Y / 100.0f;
            this->m_Rotation.Z += diff.Z / 100.0f;
            this->m_Rotation.W += diff.W / 100.0f;
            this->SetRotation(this->m_Rotation);
        }

        this->Move(PuRe_Vector3F(0.0f, 0.0f, -this->m_ZOffset));
    }
}