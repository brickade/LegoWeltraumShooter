#include "include/Spaceship.h"

#include "TheBrick/Conversion.h"
#include "TheBrick/BrickInstance.h"
#include "TheBrick/Brick.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CSpaceship::CSpaceship(ong::World& a_rWorld,std::string a_Name) : CGameObject(a_rWorld, nullptr)
    {
        this->m_Type = TheBrick::EGameObjectType::Ship;
        this->m_TargetVec = ong::vec3(0.0f, 0.0f, 0.0f);
        this->m_TargetAng = ong::vec3(0.0f, 0.0f, 0.0f);
        this->m_Name = a_Name;
        this->m_Respawn = 0.0f;
    }

    // **************************************************************************
    // **************************************************************************
    CSpaceship::~CSpaceship()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Collision(ong::Collider* thisCollider, ong::Contact* contact)
    {
        CSpaceship* Ship = (CSpaceship*)thisCollider->getBody()->getUserData();
        ong::Collider* other;
        if (thisCollider == contact->colliderA)
            other = contact->colliderB;
        else
            other = contact->colliderA;

        CGameObject* object = static_cast<CGameObject*>(other->getBody()->getUserData());
        if (object->m_Type == TheBrick::EGameObjectType::Ship)
        {
            
        }
        else if (object->m_Type == TheBrick::EGameObjectType::Bullet)
        {
            CBullet* bull = static_cast<CBullet*>(object);
            if (!bull->m_Collided)
            {
                if (Ship->m_Respawn == 0.0f)
                    Ship->m_Life -= bull->m_Damage;
                bull->m_Collided = true;
            }
        }
        else
        {

        }
        
            
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::CalculateData()
    {
        float mass = 1.0f/this->m_pBody->getInverseMass();
        this->m_RotationAcceleration = PuRe_Vector3F(mass*20.0f, mass*20.0f, mass*20.0f);
        this->m_SpeedAcceleration = mass*20.0f;
        this->m_MaxRotationSpeed = PuRe_Vector3F(5.0f, 5.0f, 5.0f);
        this->m_MaxSpeed = 10.0f*(200.0f/mass);
        this->m_MaxLife = (int)(mass*10.0f);
        this->m_Life = this->m_MaxLife;


        ong::Collider* c = this->m_pBody->getCollider();
        ong::ColliderCallbacks cb;
        cb.beginContact = CSpaceship::Collision;

        while (c)
        {
            c->setCallbacks(cb);
            c = c->getNext();
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Shoot(std::vector<CBullet*>& a_rBullets)
    {
        ong::Body* b = this->m_pBody;
        ong::World* w = b->getWorld();
        PuRe_Vector3F forward = TheBrick::OngToPuRe(ong::rotate(ong::vec3(0, 0, 1), b->getOrientation()));
        PuRe_Vector3F speed = PuRe_Vector3F::Normalize(TheBrick::OngToPuRe(this->m_pBody->getLinearMomentum()));
        speed *= 100.0f;
        speed += forward*10.0f;
        a_rBullets.push_back(new CBullet(TheBrick::OngToPuRe(this->GetTransform().p) + PuRe_Vector3F(-0.5f, -0.5f, 0.0f) + forward*10.0f, speed, *w));
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Spin(float a_Spin)
    {
        if (a_Spin > 0.2f || a_Spin < -0.2f)
            this->m_TargetAng.z = a_Spin * this->m_MaxRotationSpeed.X;
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Move(PuRe_Vector2F a_Move)
    {
        if (a_Move.Length() > 0.5f)
        {
            this->m_TargetAng.x = a_Move.Y * this->m_MaxRotationSpeed.Z;
            this->m_TargetAng.y = a_Move.X * this->m_MaxRotationSpeed.Y;
        }

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Thrust(float a_Thrust)
    {
        if (a_Thrust > 0.2f || a_Thrust < -0.2f)
            this->m_TargetVec.z += a_Thrust * this->m_MaxSpeed;
    }
    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Update(float a_DeltaTime)
    {
        if (this->m_Respawn > 0.0f)
        {
            this->m_Respawn -= a_DeltaTime;
            if (this->m_Respawn < 0.0f)
            {
                this->m_Respawn = 0.0f;
                float x = (float)(std::rand() % 100);
                float y = (float)(std::rand() % 100);
                float z = (float)(std::rand() % 100);
                this->m_pBody->setPosition(ong::vec3(x, y, z));
                this->m_Life = this->m_MaxLife;
            }
        }
        else
        {
            ong::vec3 currVel = ong::rotate(this->m_pBody->getLinearVelocity(), ong::conjugate(this->m_pBody->getOrientation()));
            ong::vec3 currAng = ong::rotate(this->m_pBody->getAngularVelocity(), ong::conjugate(this->m_pBody->getOrientation()));

            float forAcc = this->m_SpeedAcceleration;
            float yawAcc = this->m_RotationAcceleration.Y;
            float pitchAcc = this->m_RotationAcceleration.Z;
            float rollAcc = this->m_RotationAcceleration.X;
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

            ong::Transform t = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        }
    }

    // **************************************************************************
    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld)
    {
        CGameObject::Deserialize(a_pSerializer, a_rBricks, a_pWorld);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Serialize(TheBrick::CSerializer& a_pSerializer)
    {
        CGameObject::Serialize(a_pSerializer);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Reset(TheBrick::CBrick& a_rStartBrick, ong::World& a_rWorld)
    {
        for (size_t i = 0; i < this->m_pBricks.size(); i++)
        {
            SAFE_DELETE(this->m_pBricks[i]);
            i--;
        }
        this->SetNameFromFilename("Banana");
        TheBrick::CBrickInstance* brickInstance = a_rStartBrick.CreateInstance(*this, a_rWorld);
        brickInstance->SetTransform(ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
        brickInstance->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, 0.0f, 0.0f));
        brickInstance->m_Color = PuRe_Color(0, 0, 1);
    }
}