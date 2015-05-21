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
    void CSpaceship::HandleInput(PuRe_Camera* a_pCamera, PuRe_IInput* a_pInput, float a_DeltaTime, std::vector<CBullet*>& a_rBullets, CBrickManager* a_pManager)
    {
        PuRe_Vector3F forward = a_pCamera->GetForward();

        if (a_pInput->KeyPressed(a_pInput->R))
        {
            SAFE_DELETE(this->m_pCSVFile);
            this->m_pCSVFile = new CCSVParser("../data/player.csv");
        }


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
        {
            ong::Body* b = this->m_pBody;
            ong::World* w = b->getWorld();
            PuRe_Vector3F forward = TheBrick::OngToPuRe(ong::rotate(ong::vec3(0, 0, 1), b->getOrientation()));
            a_rBullets.push_back(new TheBrick::CBullet(a_pManager, TheBrick::OngToPuRe(this->m_Transform.p) + PuRe_Vector3F(-0.5f, -0.5f, 0.0f) + forward*10.0f, forward*50.0f, w));
        }

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
        //apply thrust now
        if (thrust > 0.2f||thrust < -0.2f)
            targetVel.z += thrust * maxSpeed;

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
        if (Move.Length() > 0.5f)
        {
            targetPitchSpeed = Move.Y * maxPitchSpeed;
            targetYawSpeed = Move.X * maxYawSpeed;
        }


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

        if (Spin > 0.2F||Spin < -0.2f)
            targetRollSpeed = SpinL+SpinR;

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
        //m_pBody
        ong::BodyDescription bdesc;
        bdesc.transform = ong::Transform(ong::vec3(10.0f, 10.0f, 10.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        bdesc.type = ong::BodyType::Dynamic;
        bdesc.angularMomentum = ong::vec3(0, 0, 0); //rotation speed
        bdesc.linearMomentum = ong::vec3(0, 0, 0);  //movement speed
        this->m_pBody = a_pWorld->createBody(bdesc);

        ////////   TESTING ONLY   ////////

        CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(1), a_pWorld);
        brick->m_Transform = bdesc.transform;
        brick->m_Color = PuRe_Color(1, 1, 1, 1);
        for (int i = 0; i<brick->m_pCollider.size(); i++)
            this->m_pBody->addCollider(brick->m_pCollider[i]);
        this->m_pBricks.push_back(brick);
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