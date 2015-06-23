#include "include\GameCamera.h"

#include "TheBrick/Conversion.h"

namespace sba
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
        this->m_QRotation = PuRe_QuaternionF();
    }

    void CGameCamera::UpdateData(int a_CID, sba::CSpaceship* a_pPlayer, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer)
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

        float speed = TheBrick::OngToPuRe(a_pPlayer->m_pBody->getLinearVelocity()).Length();

        this->m_ZOffset = 50.0f;
        float zOffset = speed / a_pPlayer->GetMaxSpeed();
        zOffset = zOffset*zOffset;
        zOffset *= speed/50.0f;
        this->m_ZOffset += 25.0f*(zOffset);

        if (a_pInput->GamepadPressed(a_pInput->Right_Thumb,a_CID))
            this->m_TimeToRotate = 0.0f;



        this->SetPosition(TheBrick::OngToPuRe(a_pPlayer->m_pBody->getWorldCenter()));


        PuRe_Vector2F rightStick = a_pInput->GetGamepadRightThumb(a_CID);
        if (rightStick.Length() > 0.2f)
        {
            this->m_CamRotation.X += rightStick.Y/50.0f;
            this->m_CamRotation.Y += rightStick.X / 50.0f;
            if (this->m_CamRotation.Y > PuRe_PI * 2)
                this->m_CamRotation.Y = this->m_CamRotation.Y - PuRe_PI * 2;
            if (this->m_CamRotation.Y < -PuRe_PI * 2)
                this->m_CamRotation.Y = PuRe_PI * 2 + this->m_CamRotation.Y;
            if (this->m_CamRotation.X > PuRe_PI * 2)
                this->m_CamRotation.X = this->m_CamRotation.X - PuRe_PI * 2;
            if (this->m_CamRotation.X < -PuRe_PI * 2)
                this->m_CamRotation.X = PuRe_PI * 2 + this->m_CamRotation.X;
            this->m_TimeToRotate = 3.0f;
        }
        else
        {
            if (this->m_TimeToRotate <= 0.0f)
            {
                float rotSpeed = 0.05f;
                if (this->m_CamRotation.Y > PuRe_PI)
                {
                    this->m_CamRotation.Y += rotSpeed;
                    if (this->m_CamRotation.Y > PuRe_PI * 2)
                        this->m_CamRotation.Y = 0.0f;
                }
                else if (this->m_CamRotation.Y < -PuRe_PI)
                {
                    this->m_CamRotation.Y -= rotSpeed;
                    if (this->m_CamRotation.Y < -PuRe_PI * 2)
                        this->m_CamRotation.Y = 0.0f;
                }
                else if (this->m_CamRotation.Y > 0.0f)
                {
                    this->m_CamRotation.Y -= rotSpeed;
                    if (this->m_CamRotation.Y < 0.0f)
                        this->m_CamRotation.Y = 0.0f;
                }
                else
                {
                    this->m_CamRotation.Y += rotSpeed;
                    if (this->m_CamRotation.Y > 0.0f)
                        this->m_CamRotation.Y = 0.0f;
                }

                if (this->m_CamRotation.X > PuRe_PI)
                {
                    this->m_CamRotation.X += rotSpeed;
                    if (this->m_CamRotation.X > PuRe_PI * 2)
                        this->m_CamRotation.X = 0.0f;
                }
                else if (this->m_CamRotation.X < -PuRe_PI)
                {
                    this->m_CamRotation.X -= rotSpeed;
                    if (this->m_CamRotation.X < -PuRe_PI * 2)
                        this->m_CamRotation.X = 0.0f;
                }
                else if (this->m_CamRotation.X > 0.0f)
                {
                    this->m_CamRotation.X -= rotSpeed;
                    if (this->m_CamRotation.X < 0.0f)
                        this->m_CamRotation.X = 0.0f;
                }
                else
                {
                    this->m_CamRotation.X += rotSpeed;
                    if (this->m_CamRotation.X > 0.0f)
                        this->m_CamRotation.X = 0.0f;
                }
                //PuRe_Vector3F Diff = PuRe_Vector3F() - this->m_CamRotation;
                //this->m_CamRotation.X += Diff.X/50.0f;
                //this->m_CamRotation.Y += Diff.Y/50.0f;
            }
            else
                this->m_TimeToRotate -= Seconds;
        }
        //std::string Key = a_pPlayer->m_pCSVFile->GetValue("Camera.X");
        //bool invert = false;
        //if (Key.substr(0, 1) == "-")
        //{
        //    Key = Key.substr(1, Key.length());
        //    invert = true;
        //}
        //if (Key == "LeftThumb.X")
        //    Cam.X = a_pInput->GetGamepadLeftThumb(a_CID).X;
        //else if (Key == "RightThumb.X")
        //    Cam.X = a_pInput->GetGamepadRightThumb(a_CID).X;
        //else if (Key == "LeftThumb.Y")
        //    Cam.X = a_pInput->GetGamepadLeftThumb(a_CID).Y;
        //else if (Key == "RightThumb.Y")
        //    Cam.X = a_pInput->GetGamepadRightThumb(a_CID).Y;
        //if (invert)
        //    Cam.X = -Cam.X;

        //Key = a_pPlayer->m_pCSVFile->GetValue("Camera.Y");
        //invert = false;
        //if (Key.substr(0, 1) == "-")
        //{
        //    Key = Key.substr(1, Key.length());
        //    invert = true;
        //}
        //if (Key == "LeftThumb.X")
        //    Cam.Y = a_pInput->GetGamepadLeftThumb(a_CID).X;
        //else if (Key == "RightThumb.X")
        //    Cam.Y = a_pInput->GetGamepadRightThumb(a_CID).X;
        //else if (Key == "LeftThumb.Y")
        //    Cam.Y = a_pInput->GetGamepadLeftThumb(a_CID).Y;
        //else if (Key == "RightThumb.Y")
        //    Cam.Y = a_pInput->GetGamepadRightThumb(a_CID).Y;
        //if (invert)
        //    Cam.Y = -Cam.Y;

        PuRe_QuaternionF quat = TheBrick::OngToPuRe(a_pPlayer->m_pBody->getOrientation());
        if (this->m_QRotation != quat)
        {
            PuRe_QuaternionF diff = quat - this->m_QRotation;
            this->m_QRotation.X += diff.X / 5.0f;
            this->m_QRotation.Y += diff.Y / 5.0f;
            this->m_QRotation.Z += diff.Z / 5.0f;
            this->m_QRotation.W += diff.W / 5.0f;
        }
        PuRe_QuaternionF camrot = PuRe_QuaternionF(this->m_CamRotation.X, this->m_CamRotation.Y, 0.0f);
        this->SetRotation(this->m_QRotation*camrot);

        this->Move(PuRe_Vector3F(0.0f, 10.0f, -this->m_ZOffset));
    }
}