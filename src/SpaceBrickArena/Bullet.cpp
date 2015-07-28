#include "include/Bullet.h"

#include "TheBrick/Conversion.h"
#include "TheBrick/BrickInstance.h"

#include "include/Player.h"
#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CBullet::CBullet(ong::BodyDescription* a_desc, ong::World& a_rWorld, SPlayer* a_pOwner,PuRe_Color a_Color,unsigned int a_ID) : CGameObject(a_rWorld, a_desc)
    {
        this->m_Speed = ong::length(a_desc->linearMomentum);
        this->m_ID = a_ID;
        this->m_pOwner = a_pOwner;
        this->m_Type = TheBrick::EGameObjectType::Bullet;
        TheBrick::CBrickInstance* brick = new TheBrick::CBrickInstance(*sba_BrickManager->GetBrickArray()[a_ID], *this, *sba_World);
        brick->m_Color = a_Color;
        this->m_Color = a_Color;
        this->m_lifeTime = 0.0f;
        this->m_Collided = false;
        this->m_pEmitter = new PuRe_ParticleEmitter(TheBrick::OngToPuRe(this->m_pBody->getWorldCenter()), TheBrick::OngToPuRe(this->m_pBody->getTransform().q));
        std::string sound;
        switch (this->m_ID)
        {

        case TheBrick::Laser: //Laser
            sound = "laser_v2";
            this->m_Damage = 20;
           break;

        case TheBrick::MG: //MG
            sound = "mg";
            this->m_Damage = 10;
            break;

        case TheBrick::Mine: //Mine
            sound = "mine";
            this->m_Damage = 100;
            break;

        case TheBrick::Rocket: //Rocket
            sound = "torpedo";
            this->m_Damage = 25;
            break;

        case TheBrick::Torpedo: //Torpedo
            sound = "torpedo";
            this->m_Damage = 50;
            break;
        }
        this->m_SoundChannel = sba_SoundPlayer->PlaySound(sound.c_str(), false, false, std::stof(sba_Options->GetValue("SoundVolume")), TheBrick::OngToPuRe(a_desc->transform.p), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector2F(1.0f, 10.0f));

        ong::Collider* c = this->m_pBody->getCollider();
        while (c)
        {
            ong::ColliderCallbacks cb = c->getColliderCallbacks();
            cb.beginContact = CBullet::Collision;

            c->setCallbacks(cb);
            c = c->getNext();
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBullet::Collision(ong::Collider* thisCollider, ong::Contact* contact)
    {
        CBullet* bullet = (CBullet*)thisCollider->getBody()->getUserData();
        ong::Collider* other;
        if (thisCollider == contact->colliderA)
            other = contact->colliderB;
        else
            other = contact->colliderA;

        CGameObject* object = static_cast<CGameObject*>(other->getBody()->getUserData());
        if (object->m_Type != TheBrick::EGameObjectType::Bullet)
        {
            bullet->m_lifeTime = 4.9f;
            CSpaceship* ship = static_cast<CSpaceship*>(object);
            //if it's a ship with a shield, we die
            if (ship->m_Shield > 0.0f)
                bullet->m_lifeTime = 10.0f;
        }
    }

    // **************************************************************************
    // **************************************************************************
    CBullet::~CBullet()
    {
        SAFE_DELETE(this->m_pEmitter);
    }

    // **************************************************************************
    // **************************************************************************
    void CBullet::DrawEmitter(PuRe_Sprite* a_pSprite, PuRe_IMaterial* a_pMaterial, PuRe_PointLight* a_pLight, PuRe_IMaterial* a_pLightMaterial)
    {
        sba_Renderer->Draw(0, true, this->m_pEmitter, a_pMaterial, a_pSprite,-1,0.15f);
    }

    // **************************************************************************
    // **************************************************************************
    void CBullet::Update(float a_DeltaTime)
    {
        this->m_lifeTime += a_DeltaTime;


        if (this->m_ID == TheBrick::Rocket)
        {
            ong::vec3 pos = this->m_pBody->getWorldCenter();
            ong::vec3 gpos = this->m_pBody->getWorldCenter();
            float length = 999999999.0f;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                if (this->m_pOwner != sba_Players[i])
                {
                    ong::vec3 tpos = sba_Players[i]->Ship->m_pBody->getWorldCenter();
                    float len = ong::lengthSq(pos - tpos);
                    if (len < length)
                    {
                        len = length;
                        gpos = tpos;
                    }
                }
                    
            }
            //now points to the player
            pos = ong::normalize(gpos - pos);
            ong::vec3 forw = ong::rotate(ong::vec3(0, 0, 1), this->m_pBody->getOrientation());

            float diff = ong::dot(forw,pos);

            ong::vec3 force = (ong::cross(forw, pos));
            this->m_pBody->applyTorque(force, a_DeltaTime);

            ong::vec3 currVel = ong::rotate(this->m_pBody->getLinearVelocity(), ong::conjugate(this->m_pBody->getOrientation()));
            ong::vec3 targetVec = ong::vec3(0.0f, 0.0f, this->m_Speed);
            this->m_pBody->applyRelativeImpulse(1.0f / this->m_pBody->getInverseMass() * ong::vec3(targetVec.x - currVel.x, targetVec.y - currVel.y, 0));
        }


        if (this->m_ID != TheBrick::Laser&&this->m_ID != TheBrick::Mine) //laser and mine no particles 
        {
            this->m_pEmitter->m_Position = TheBrick::OngToPuRe(this->m_pBody->getWorldCenter());
            this->m_pEmitter->m_Rotation = TheBrick::OngToPuRe(this->m_pBody->getOrientation());
            PuRe_Vector3F pos = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
            PuRe_Vector3F size = PuRe_Vector3F(1.0f, 1.0f, 1.0f);
            PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
            PuRe_Vector3F velocity = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
            float life = 0.3f;

            pos.X = ((std::rand() % 100) / 100.0f) - 1.0f;
            pos.Y = ((std::rand() % 100) / 100.0f) - 1.0f;
            pos.Z = (std::rand() % 100) / 100.0f;

            float rsize = (std::rand() % 10) / 100.0f;
            size.X *= rsize;
            size.Y *= rsize;
            size.Z *= rsize;
            this->m_pEmitter->Spawn(0.3f, pos, size, velocity, this->m_pEmitter->m_Rotation,color);
            this->m_pEmitter->Update(a_DeltaTime);
        }
        sba_SoundPlayer->SetPosition(this->m_SoundChannel, TheBrick::OngToPuRe(this->m_pBody->getWorldCenter()),PuRe_Vector3F());
    }

}