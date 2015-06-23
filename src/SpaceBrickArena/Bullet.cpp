#include "include/Bullet.h"

#include "TheBrick/Conversion.h"
#include "TheBrick/BrickInstance.h"

#include "include/Player.h"
#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CBullet::CBullet(PuRe_Vector3F a_Position, PuRe_Vector3F a_Speed, ong::World& a_rWorld,SPlayer* a_pOwner) : CGameObject(a_rWorld, nullptr)
    {
        this->m_pOwner = a_pOwner;
        this->m_Type = TheBrick::EGameObjectType::Bullet;
        TheBrick::CBrickInstance* brick = new TheBrick::CBrickInstance(*sba_BrickManager->GetBrickArray()[0], *this, *sba_World);
        this->m_pBody->setPosition(TheBrick::PuReToOng(a_Position));
        this->m_pBody->applyImpulse(TheBrick::PuReToOng(a_Speed));

        this->m_lifeTime = 0.0f;
        this->m_Damage = 10;
        this->m_Collided = false;
    }   

    // **************************************************************************
    // **************************************************************************
    CBullet::~CBullet()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CBullet::Update(float a_DeltaTime)
    {
        this->m_lifeTime += a_DeltaTime;
    }

}