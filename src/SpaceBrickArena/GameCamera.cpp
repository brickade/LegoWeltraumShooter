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

    float CGameCamera::getZOffset()
    {
        return this->m_ZOffset;
    }


    void CGameCamera::UpdateData(int a_CID, SPlayer* a_pPlayer, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer)
    {
        //Seconds for frame independent movement
        float Seconds = a_pTimer->GetElapsedSeconds();
        float speed = TheBrick::OngToPuRe(a_pPlayer->Ship->m_pBody->getLinearVelocity()).Length();

        this->m_ZOffset = 50.0f;
        float zOffset = speed / a_pPlayer->Ship->GetMaxSpeed();
        zOffset = zOffset*zOffset;
        zOffset *= speed/50.0f;
        this->m_ZOffset += 25.0f*(zOffset);

        if (a_pInput->GamepadPressed(a_pInput->Right_Thumb,a_CID))
            this->m_TimeToRotate = 0.0f;


        PuRe_Vector3F pos = TheBrick::OngToPuRe(a_pPlayer->Ship->m_pBody->getWorldCenter());
        this->SetPosition(pos);


        //PuRe_Vector2F rightStick = a_pInput->GetGamepadRightThumb(a_CID);
        //if (rightStick.Length() > 0.2f)
        //{
        //    this->m_CamRotation.X += rightStick.Y/50.0f;
        //    this->m_CamRotation.Y += rightStick.X / 50.0f;
        //    if (this->m_CamRotation.Y > PuRe_PI * 2)
        //        this->m_CamRotation.Y = this->m_CamRotation.Y - PuRe_PI * 2;
        //    if (this->m_CamRotation.Y < -PuRe_PI * 2)
        //        this->m_CamRotation.Y = PuRe_PI * 2 + this->m_CamRotation.Y;
        //    if (this->m_CamRotation.X > PuRe_PI * 2)
        //        this->m_CamRotation.X = this->m_CamRotation.X - PuRe_PI * 2;
        //    if (this->m_CamRotation.X < -PuRe_PI * 2)
        //        this->m_CamRotation.X = PuRe_PI * 2 + this->m_CamRotation.X;
        //    this->m_TimeToRotate = 3.0f;
        //}
        //else
        //{
        //    if (this->m_TimeToRotate <= 0.0f)
        //    {
        //        float rotSpeed = 0.05f;
        //        if (this->m_CamRotation.Y > PuRe_PI)
        //        {
        //            this->m_CamRotation.Y += rotSpeed;
        //            if (this->m_CamRotation.Y > PuRe_PI * 2)
        //                this->m_CamRotation.Y = 0.0f;
        //        }
        //        else if (this->m_CamRotation.Y < -PuRe_PI)
        //        {
        //            this->m_CamRotation.Y -= rotSpeed;
        //            if (this->m_CamRotation.Y < -PuRe_PI * 2)
        //                this->m_CamRotation.Y = 0.0f;
        //        }
        //        else if (this->m_CamRotation.Y > 0.0f)
        //        {
        //            this->m_CamRotation.Y -= rotSpeed;
        //            if (this->m_CamRotation.Y < 0.0f)
        //                this->m_CamRotation.Y = 0.0f;
        //        }
        //        else
        //        {
        //            this->m_CamRotation.Y += rotSpeed;
        //            if (this->m_CamRotation.Y > 0.0f)
        //                this->m_CamRotation.Y = 0.0f;
        //        }

        //        if (this->m_CamRotation.X > PuRe_PI)
        //        {
        //            this->m_CamRotation.X += rotSpeed;
        //            if (this->m_CamRotation.X > PuRe_PI * 2)
        //                this->m_CamRotation.X = 0.0f;
        //        }
        //        else if (this->m_CamRotation.X < -PuRe_PI)
        //        {
        //            this->m_CamRotation.X -= rotSpeed;
        //            if (this->m_CamRotation.X < -PuRe_PI * 2)
        //                this->m_CamRotation.X = 0.0f;
        //        }
        //        else if (this->m_CamRotation.X > 0.0f)
        //        {
        //            this->m_CamRotation.X -= rotSpeed;
        //            if (this->m_CamRotation.X < 0.0f)
        //                this->m_CamRotation.X = 0.0f;
        //        }
        //        else
        //        {
        //            this->m_CamRotation.X += rotSpeed;
        //            if (this->m_CamRotation.X > 0.0f)
        //                this->m_CamRotation.X = 0.0f;
        //        }
        //    }
        //    else
        //        this->m_TimeToRotate -= Seconds;
        //}

        float xshake = 0.0f;
        float yshake = 0.0f;
        if (a_pPlayer->Shake != 0.0f)
        {
            xshake = sin(Seconds*100.0f) * 0.4f * a_pPlayer->Shake * a_pPlayer->Shake;
            yshake = cos(Seconds*100.0f) * 0.4f * a_pPlayer->Shake * a_pPlayer->Shake;
        }

        if (ong::length(a_pPlayer->Ship->m_pBody->getAngularVelocity()) < 10.0f)
        {
            PuRe_QuaternionF quat = TheBrick::OngToPuRe(a_pPlayer->Ship->m_pBody->getOrientation());
            if (this->m_QRotation != quat)
            {
                PuRe_QuaternionF diff = quat - this->m_QRotation;
                this->m_QRotation.X += diff.X / 5.0f;
                this->m_QRotation.Y += diff.Y / 5.0f;
                this->m_QRotation.Z += diff.Z / 5.0f;
                this->m_QRotation.W += diff.W / 5.0f;
            }
            PuRe_QuaternionF camrot = PuRe_QuaternionF(xshake / 100.0f, yshake / 100.0f, 0.0f) * PuRe_QuaternionF(this->m_CamRotation.X, this->m_CamRotation.Y, 0.0f) * PuRe_QuaternionF(0.13f, 0.0f, 0.0f);
            this->SetRotation(this->m_QRotation*camrot);
        }
        this->Move(PuRe_Vector3F(xshake, yshake+8.0f, -this->m_ZOffset));
    }

    PuRe_QuaternionF CGameCamera::GetAim()
    {
        return this->m_QRotation * PuRe_QuaternionF(this->m_CamRotation.X, this->m_CamRotation.Y, 0.0f);
    }

}