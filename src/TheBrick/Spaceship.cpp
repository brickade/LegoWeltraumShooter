#include "include/TheBrick/Spaceship.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CSpaceship::CSpaceship(ong::World& a_rWorld) : CGameObject(a_rWorld, nullptr)
    {
        this->m_pCSVFile = new CCSVParser("../data/player.csv");
        this->m_TargetVec = ong::vec3(0.0f, 0.0f, 0.0f);
        this->m_TargetAng = ong::vec3(0.0f, 0.0f, 0.0f);
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
        DrawBody(this->m_pBody,a_pCamera,a_pGraphics);
    }

// **************************************************************************
    // **************************************************************************
    void CSpaceship::Shoot(std::vector<CBullet*>& a_rBullets, CBrickManager* a_pManager, float a_DeltaTime)
    {
        ong::Body* b = this->m_pBody;
        ong::World* w = b->getWorld();
        PuRe_Vector3F forward = TheBrick::OngToPuRe(ong::rotate(ong::vec3(0, 0, 1), b->getOrientation()));
        a_rBullets.push_back(new TheBrick::CBullet(a_pManager, TheBrick::OngToPuRe(this->GetTransform().p) + PuRe_Vector3F(-0.5f, -0.5f, 0.0f) + forward*10.0f, forward*50.0f, *w));
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Spin(float a_Spin, float a_DeltaTime)
    {
        float maxRollSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxRollSpeed").c_str());
        if (a_Spin > 0.2f || a_Spin < -0.2f)
            this->m_TargetAng.z = a_Spin * maxRollSpeed;
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Move(PuRe_Vector2F a_Move, float a_DeltaTime)
    {
        float maxYawSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxYawSpeed").c_str());
        float maxPitchSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxPitchSpeed").c_str());
        if (a_Move.Length() > 0.5f)
        {
            this->m_TargetAng.x = a_Move.Y * maxPitchSpeed;
            this->m_TargetAng.y = a_Move.X * maxYawSpeed;
        }

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Thrust(float a_Thrust,float a_DeltaTime)
    {
        float maxSpeed = (float)atof(this->m_pCSVFile->GetValue("MaxSpeed").c_str());
        if (a_Thrust > 0.2f || a_Thrust < -0.2f)
            this->m_TargetVec.z += a_Thrust * maxSpeed;
    }



    // **************************************************************************
    // **************************************************************************
    void CSpaceship::HandleInput(PuRe_IInput* a_pInput, float a_DeltaTime, std::vector<CBullet*>& a_rBullets, CBrickManager* a_pManager)
    {
        if (a_pInput->KeyPressed(a_pInput->R))
        {
            SAFE_DELETE(this->m_pCSVFile);
            this->m_pCSVFile = new CCSVParser("../data/player.csv");
        }


        std::string Key = this->m_pCSVFile->GetValue("Shoot");
        bool shoot = false;
        if (Key == "A")
            shoot = a_pInput->GamepadPressed(a_pInput->Pad_A, 0);
        else if (Key == "X")
            shoot = a_pInput->GamepadPressed(a_pInput->Pad_X, 0);
        else if (Key == "Y")
            shoot = a_pInput->GamepadPressed(a_pInput->Pad_Y, 0);
        else if (Key == "B")
            shoot = a_pInput->GamepadPressed(a_pInput->Pad_B, 0);
        else if (Key == "LB")
            shoot = a_pInput->GamepadPressed(a_pInput->Left_Shoulder, 0);
        else if (Key == "RB")
            shoot = a_pInput->GamepadPressed(a_pInput->Right_Shoulder, 0);
        else if (Key == "RT")
            shoot = a_pInput->GetGamepadRightTrigger(0) > 0.2f;
        else if (Key == "LT")
            shoot = a_pInput->GetGamepadLeftTrigger(0) > 0.2f;

        if (shoot)
            this->Shoot(a_rBullets, a_pManager, a_DeltaTime);

        //handle thrust from file
        Key = this->m_pCSVFile->GetValue("Thrust");

        bool invert = false;
        if (Key.substr(0, 1) == "-")
        {
            Key = Key.substr(1, Key.length());
            invert = true;
        }

        float thrust = 0.0f;
        if (Key == "RT")
            thrust = a_pInput->GetGamepadRightTrigger(0);
        else if (Key == "LT")
            thrust = a_pInput->GetGamepadLeftTrigger(0);
        else if (Key == "LB"&&a_pInput->GamepadIsPressed(a_pInput->Left_Shoulder, 0))
            thrust = 1.0f;
        else if (Key == "RB"&&a_pInput->GamepadIsPressed(a_pInput->Right_Shoulder, 0))
            thrust = 1.0f;
        else if (Key == "LeftThumb.X")
            thrust = a_pInput->GetGamepadLeftThumb(0).X;
        else if (Key == "RightThumb.X")
            thrust = a_pInput->GetGamepadRightThumb(0).X;
        else if (Key == "LeftThumb.Y")
            thrust = a_pInput->GetGamepadLeftThumb(0).Y;
        else if (Key == "RightThumb.Y")
            thrust = a_pInput->GetGamepadRightThumb(0).Y;
        if (invert)
            thrust = -thrust;

        this->Thrust(thrust,a_DeltaTime);

        //handle X/YRotation from file
        PuRe_Vector2F Move;
        Key = this->m_pCSVFile->GetValue("Move.X");
        invert = false;
        if (Key.substr(0, 1) == "-")
        {
            Key = Key.substr(1, Key.length());
            invert = true;
        }
        if (Key == "LeftThumb.X")
            Move.X = a_pInput->GetGamepadLeftThumb(0).X;
        else if (Key == "RightThumb.X")
            Move.X = a_pInput->GetGamepadRightThumb(0).X;
        else if (Key == "LeftThumb.Y")
            Move.X = a_pInput->GetGamepadLeftThumb(0).Y;
        else if (Key == "RightThumb.Y")
            Move.X = a_pInput->GetGamepadRightThumb(0).Y;
        if (invert)
            Move.X = -Move.X;

        Key = this->m_pCSVFile->GetValue("Move.Y");

        invert = false;
        if (Key.substr(0, 1) == "-")
        {
            Key = Key.substr(1, Key.length());
            invert = true;
        }

        if (Key == "LeftThumb.X")
            Move.Y = a_pInput->GetGamepadLeftThumb(0).X;
        else if (Key == "RightThumb.X")
            Move.Y = a_pInput->GetGamepadRightThumb(0).X;
        else if (Key == "LeftThumb.Y")
            Move.Y = a_pInput->GetGamepadLeftThumb(0).Y;
        else if (Key == "RightThumb.Y")
            Move.Y = a_pInput->GetGamepadRightThumb(0).Y;
        if (invert)
            Move.Y = -Move.Y;
        //apply X/Y Rotation via Input

        this->Move(Move,a_DeltaTime);


        //handle LeftSpin from file
        Key = this->m_pCSVFile->GetValue("SpinLeft");
        invert = false;
        if (Key.substr(0, 1) == "-")
        {
            Key = Key.substr(1, Key.length());
            invert = true;
        }
        float SpinL = 0.0f;
        float SpinR = 0.0f;
        if (Key == "RT")
            SpinL += a_pInput->GetGamepadRightTrigger(0);
        else if (Key == "LT")
            SpinL += a_pInput->GetGamepadLeftTrigger(0);
        else if (Key == "LB"&&a_pInput->GamepadIsPressed(a_pInput->Left_Shoulder, 0))
            SpinL += 1.0f;
        else if (Key == "RB"&&a_pInput->GamepadIsPressed(a_pInput->Right_Shoulder, 0))
            SpinL += 1.0f;
        else if (Key == "LeftThumb.X")
            SpinL += a_pInput->GetGamepadLeftThumb(0).X;
        else if (Key == "RightThumb.X")
            SpinL += a_pInput->GetGamepadRightThumb(0).X;
        else if (Key == "LeftThumb.Y")
            SpinL += a_pInput->GetGamepadLeftThumb(0).Y;
        else if (Key == "RightThumb.Y")
            SpinL += a_pInput->GetGamepadRightThumb(0).Y;
        if (invert)
            SpinL = -SpinL;
        //handle RightSpin from file
        Key = this->m_pCSVFile->GetValue("SpinRight");
        invert = false;
        if (Key.substr(0, 1) == "-")
        {
            Key = Key.substr(1, Key.length());
            invert = true;
        }
        if (Key == "RT")
            SpinR += a_pInput->GetGamepadRightTrigger(0);
        else if (Key == "LT")
            SpinR += a_pInput->GetGamepadLeftTrigger(0);
        else if (Key == "LB"&&a_pInput->GamepadIsPressed(a_pInput->Left_Shoulder, 0))
            SpinR += 1.0f;
        else if (Key == "RB"&&a_pInput->GamepadIsPressed(a_pInput->Right_Shoulder, 0))
            SpinR += 1.0f;
        else if (Key == "LeftThumb.X")
            SpinR += a_pInput->GetGamepadLeftThumb(0).X;
        else if (Key == "RightThumb.X")
            SpinR += a_pInput->GetGamepadRightThumb(0).X;
        else if (Key == "LeftThumb.Y")
            SpinR += a_pInput->GetGamepadLeftThumb(0).Y;
        else if (Key == "RightThumb.Y")
            SpinR += a_pInput->GetGamepadRightThumb(0).Y;
        if (invert)
            SpinR = -SpinR;
        float Spin = SpinR+SpinL;

        this->Spin(Spin,a_DeltaTime);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Update(float a_DeltaTime)
    {
        ong::vec3 currVel = ong::rotate(this->m_pBody->getLinearVelocity(), ong::conjugate(this->m_pBody->getOrientation()));
        ong::vec3 currAng = ong::rotate(this->m_pBody->getAngularVelocity(), ong::conjugate(this->m_pBody->getOrientation()));

        float forAcc = (float)atof(this->m_pCSVFile->GetValue("SpeedAcceleration").c_str());
        float yawAcc = (float)atof(this->m_pCSVFile->GetValue("YawAcceleration").c_str());
        float pitchAcc = (float)atof(this->m_pCSVFile->GetValue("PitchAcceleration").c_str());
        float rollAcc = (float)atof(this->m_pCSVFile->GetValue("RollAcceleration").c_str());
        // negate rotational velocity
        this->m_pBody->applyRelativeImpulse(1.0f / this->m_pBody->getInverseMass() * ong::vec3(this->m_TargetVec.x - currVel.x, this->m_TargetVec.y - currVel.y, 0));

        // apply thrust
        if (lengthSq(currVel - this->m_TargetVec) > 0.1f * 0.1f)
            this->m_pBody->applyRelativeForce(hadamardProduct(ong::vec3(0, 0, forAcc), normalize(this->m_TargetVec - currVel)), a_DeltaTime);

        //apply rotation
        if (ong::lengthSq(currAng - this->m_TargetAng) > 0.0f)
        {

            ong::vec3 f = ong::hadamardProduct(ong::vec3(pitchAcc, yawAcc, rollAcc), ong::normalize(this->m_TargetAng - currAng));
            ong::vec3 dAng = this->m_pBody->getInverseMass() * a_DeltaTime* f;

            if (ong::lengthSq(dAng) > ong::lengthSq(currAng - this->m_TargetAng))
            {
                ong::vec3 dAngularMomentum = 1.0f / this->m_pBody->getInverseMass() * (this->m_TargetAng - currAng);
                this->m_pBody->applyRelativeAngularImpulse(dAngularMomentum);
            }
            else
            {
                this->m_pBody->applyRelativeTorque(f, a_DeltaTime);
            }
        }

        //this->m_Transform = this->m_pBody->getTransform();
        this->m_TargetVec = ong::vec3(0.0f,0.0f,0.0f);
        this->m_TargetAng = ong::vec3(0.0f, 0.0f, 0.0f);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld)
    {
        CGameObject::Deserialize(a_pSerializer, a_pBrickManager, a_pWorld);

		//m_pBody
        ong::BodyDescription bdesc;
        bdesc.transform = ong::Transform(ong::vec3(10.0f, 10.0f, 10.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        bdesc.type = ong::BodyType::Dynamic;
        bdesc.angularMomentum = ong::vec3(0, 0, 0); //rotation speed
        bdesc.linearMomentum = ong::vec3(0, 0, 0);  //movement speed
        this->m_pBody = a_pWorld.createBody(bdesc);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Serialize(CSerializer& a_pSerializer)
    {
        //GameObject
        reinterpret_cast<CGameObject*>(this)->Serialize(a_pSerializer);

    }
}