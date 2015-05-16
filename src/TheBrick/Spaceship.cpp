#include "include/TheBrick/Spaceship.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CSpaceship::CSpaceship()
    {
        this->m_pCSVFile = new CCSVParser("../data/player.csv");
    }

    // **************************************************************************
    // **************************************************************************
    CSpaceship::~CSpaceship()
    {
        SAFE_DELETE(this->m_pCSVFile);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        for (unsigned int i = 0; i<this->m_pBricks.size(); i++)
        {
            this->m_pBricks[i]->m_Transform = this->m_pBody->getTransform();
            this->m_pBricks[i]->Draw(a_pGraphics, a_pCamera);
        }
        DrawBody(this->m_pBody,a_pCamera,a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::HandleInput(PuRe_Camera* a_pCamera, PuRe_IInput* a_pInput, float a_DeltaTime)
    {
        PuRe_Vector3F forward = a_pCamera->GetForward();

        ong::vec3 currVel = ong::rotate(this->m_pBody->getLinearVelocity(), ong::conjugate(this->m_pBody->getOrientation()));
        ong::vec3 currAng = ong::rotate(this->m_pBody->getAngularVelocity(), ong::conjugate(this->m_pBody->getOrientation()));

        float targetRollSpeed = 0.0f;

        float targetPitchSpeed = 0.0f;
        float targetYawSpeed = 0.0f;

        float forAcc = (float)atof(this->m_pCSVFile->GetValue("SpeedAcceleration").c_str());
        float yawAcc = (float)atof(this->m_pCSVFile->GetValue("YawAcceleration").c_str());
        float pitchAcc = (float)atof(this->m_pCSVFile->GetValue("PitchAcceleration").c_str());
        float rollAcc = (float)atof(this->m_pCSVFile->GetValue("RollAcceleration").c_str());

        float maxSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxSpeed").c_str());
        float maxYawSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxYawSpeed").c_str());
        float maxPitchSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxPitchSpeed").c_str());
        float maxRollSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxRollSpeed").c_str());


        ong::vec3 targetVel = ong::vec3(0, 0, 0.0f);
        ong::vec3 targetAng = ong::vec3(0.0f, 0.0f, 0.0f);

        //handle thrust from file
        std::string Key = this->m_pCSVFile->GetValue("Thrust");
        float thrust = 0.0f;
        if (Key == "RT")
            thrust = a_pInput->GetGamepadRightTrigger(0);
        else if (Key == "LT")
            thrust = a_pInput->GetGamepadLeftTrigger(0);
        else if (Key == "LB"&&a_pInput->GamepadIsPressed(a_pInput->Left_Shoulder, 0))
            thrust = 1.0f;
        else if (Key == "RB"&&a_pInput->GamepadIsPressed(a_pInput->Right_Shoulder, 0))
            thrust = 1.0f;
        else if (Key == "LeftThumb")
            thrust = a_pInput->GetGamepadLeftThumb(0).Y;
        else if (Key == "RightThumb")
            thrust = a_pInput->GetGamepadRightThumb(0).Y;
        //apply thrust now
        if (thrust > 0.2f)
            targetVel.z += thrust * maxSpeed;

        //handle X/YRotation from file
        Key = this->m_pCSVFile->GetValue("Move");
        PuRe_Vector2F Move;
        if (Key == "LeftThumb")
            Move = a_pInput->GetGamepadLeftThumb(0);
        else if (Key == "RightThumb")
            Move = a_pInput->GetGamepadRightThumb(0);
        //apply X/Y Rotation via Input
        if (Move.Length() > 0.5f)
        {
            targetPitchSpeed = -Move.Y * maxPitchSpeed;
            targetYawSpeed = Move.X * maxYawSpeed;
        }


        //handle LeftSpin from file
        Key = this->m_pCSVFile->GetValue("SpinLeft");
        float Spin = 0.0f;
        if (Key == "RT")
            Spin += a_pInput->GetGamepadRightTrigger(0);
        else if (Key == "LT")
            Spin += a_pInput->GetGamepadLeftTrigger(0);
        else if (Key == "LB"&&a_pInput->GamepadIsPressed(a_pInput->Left_Shoulder, 0))
            Spin += 1.0f;
        else if (Key == "RB"&&a_pInput->GamepadIsPressed(a_pInput->Right_Shoulder, 0))
            Spin += 1.0f;
        else if (Key == "LeftThumb")
            Spin += a_pInput->GetGamepadLeftThumb(0).X;
        else if (Key == "RightThumb")
            Spin += a_pInput->GetGamepadRightThumb(0).X;
        //handle RightSpin from file
        Key = this->m_pCSVFile->GetValue("SpinRight");
        if (Key == "RT")
            Spin -= a_pInput->GetGamepadRightTrigger(0);
        else if (Key == "LT")
            Spin -= a_pInput->GetGamepadLeftTrigger(0);
        else if (Key == "LB"&&a_pInput->GamepadIsPressed(a_pInput->Left_Shoulder, 0))
            Spin -= 1.0f;
        else if (Key == "RB"&&a_pInput->GamepadIsPressed(a_pInput->Right_Shoulder, 0))
            Spin -= 1.0f;
        else if (Key == "LeftThumb")
            Spin -= a_pInput->GetGamepadLeftThumb(0).X;
        else if (Key == "RightThumb")
            Spin -= a_pInput->GetGamepadRightThumb(0).X;

        targetRollSpeed = Spin;

        //Set Rotation
        targetAng.x = targetPitchSpeed;
        targetAng.y = targetYawSpeed;
        targetAng.z = targetRollSpeed;


        // negate rotational velocity
        this->m_pBody->applyRelativeImpulse(1.0f / this->m_pBody->getInverseMass() * ong::vec3(targetVel.x - currVel.x, targetVel.y - currVel.y, 0));

        // apply thrust
        if (lengthSq(currVel - targetVel) > 0.1f * 0.1f)
            this->m_pBody->applyRelativeForce(hadamardProduct(ong::vec3(0, 0, forAcc), normalize(targetVel - currVel)), a_DeltaTime); 

        //apply rotation
        if (ong::lengthSq(currAng - targetAng) > 0.0f)
        {

            ong::vec3 f = ong::hadamardProduct(ong::vec3(pitchAcc, yawAcc, rollAcc), ong::normalize(targetAng - currAng));
            ong::vec3 dAng = this->m_pBody->getInverseMass() * a_DeltaTime* f;

            if (ong::lengthSq(dAng) > ong::lengthSq(currAng - targetAng))
            {
                ong::vec3 dAngularMomentum = 1.0f / this->m_pBody->getInverseMass() * (targetAng - currAng);
                this->m_pBody->applyRelativeAngularImpulse(dAngularMomentum);
            }
            else
            {
                this->m_pBody->applyRelativeTorque(f, a_DeltaTime);
            }
        }

        this->m_Transform = this->m_pBody->getTransform();
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Update(float a_DeltaTime)
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager, ong::World* a_pWorld)
    {
        CGameObject::Deserialize(a_pSerializer, a_pBrickManager, a_pWorld);

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Serialize(CSerializer* a_pSerializer)
    {
        //GameObject
        reinterpret_cast<CGameObject*>(this)->Serialize(a_pSerializer);

    }
}