#include "include\GameCamera.h"
#include "include/Space.h"
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
        if (a_pPlayer->Ship->m_Shake != 0.0f)
        {
            xshake = sin(Seconds*100.0f) * 0.8f * a_pPlayer->Ship->m_Shake * a_pPlayer->Ship->m_Shake;
            yshake = cos(Seconds*100.0f) * 0.8f * a_pPlayer->Ship->m_Shake * a_pPlayer->Ship->m_Shake;
        }

        PuRe_Vector2F Move;
        if (sba_Controls[a_CID].Move == 1)
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_1, a_CID);
        else if (sba_Controls[a_CID].Move == 2)
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_2, a_CID);
        else if (sba_Controls[a_CID].Move == 3)
        {
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_1, a_CID);
            Move.Y = -Move.Y;
        }
        else if (sba_Controls[a_CID].Move == 4)
        {
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_2, a_CID);
            Move.Y = -Move.Y;
        }
        if (Move.Length() > 1.0f)
        {
            Move.X *= abs(Move.X);
            Move.Y *= abs(Move.Y);
        }
        else
            Move = PuRe_Vector2F(0.0f,0.0f);

        float leng = ong::length(a_pPlayer->Ship->m_pBody->getLinearVelocity()) / a_pPlayer->Ship->GetMaxSpeed();
        leng *= leng;
        float max = 20.0f; //max movement
        float x = Move.X*max*leng;
        float y = (Move.Y*max*leng) + 8.0f*(1.0f - abs(Move.Y));

        float secs = Seconds*10.0f; //speed it changes

        //X Axis
        if (this->m_CamMove.X != x)
        {
            if (this->m_CamMove.X > x) //Cam.X is greater than X
            {
                if (x > 0.0f) //X is greater than 0 , so Cam.X is between X and MAX
                    this->m_CamMove.X -= secs;
                else // X is less than 0 , so the player move the stick to the other side
                    this->m_CamMove.X -= secs*4.0f;
                if (this->m_CamMove.X < x)
                    this->m_CamMove.X = x;
            }
            else if (this->m_CamMove.X < x)
            {
                if (x < 0.0f) //X is less than 0 , so Cam.X is between X and -MAX
                    this->m_CamMove.X += secs;
                else // X is greater , so the player move the stick to the other side
                    this->m_CamMove.X += secs*4.0f;
                if (this->m_CamMove.X > x)
                    this->m_CamMove.X = x;
            }
        }

        //Y Axis
        if (this->m_CamMove.Y != y)
        {
            if (this->m_CamMove.Y > y) //Cam.X is greater than X
            {
                if (y > 0.0f) //X is greater than 0 , so Cam.X is between X and MAX
                    this->m_CamMove.Y -= secs;
                else // X is less than 0 , so the player move the stick to the other side
                    this->m_CamMove.Y -= secs*2.0f;
                if (this->m_CamMove.Y < y)
                    this->m_CamMove.Y = y;
            }
            else if (this->m_CamMove.Y < y)
            {
                if (y < 0.0f) //X is less than 0 , so Cam.X is between X and -MAX
                    this->m_CamMove.Y += secs;
                else // X is greater , so the player move the stick to the other side
                    this->m_CamMove.Y += secs*2.0f;
                if (this->m_CamMove.Y > y)
                    this->m_CamMove.Y = y;
            }
        }
/*
        this->m_CamRotation.X = -Move.Y*leng*0.5f;
        this->m_CamRotation.Y = -Move.X*leng*0.5f;*/

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
        x = xshake + this->m_CamMove.X;
        y = yshake + this->m_CamMove.Y;
        this->Move(PuRe_Vector3F(x, y, -this->m_ZOffset));
    }

    PuRe_QuaternionF CGameCamera::GetAim()
    {
        return this->m_QRotation * PuRe_QuaternionF(this->m_CamRotation.X, this->m_CamRotation.Y, 0.0f);
    }

}