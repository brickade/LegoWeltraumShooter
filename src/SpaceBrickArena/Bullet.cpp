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
        this->m_ID = a_ID;
        this->m_pOwner = a_pOwner;
        this->m_Type = TheBrick::EGameObjectType::Bullet;
        TheBrick::CBrickInstance* brick = new TheBrick::CBrickInstance(*sba_BrickManager->GetBrickArray()[a_ID], *this, *sba_World);
        brick->m_Color = a_Color;
        this->m_Color = a_Color;
        this->m_lifeTime = 0.0f;
        this->m_Damage = 10;
        this->m_Collided = false;
        this->m_pEmitter = new PuRe_ParticleEmitter(TheBrick::OngToPuRe(this->m_pBody->getWorldCenter()), TheBrick::OngToPuRe(this->m_pBody->getTransform().q));

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
        bullet->m_lifeTime = 4.9f;
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
        if (this->m_ID != 901)
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
    }

}