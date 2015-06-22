#include "include/Bullet.h"

#include "TheBrick/Conversion.h"
#include "TheBrick/BrickInstance.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CBullet::CBullet(PuRe_Vector3F a_Position, PuRe_Vector3F a_Speed, ong::World& a_rWorld) : CGameObject(a_rWorld, nullptr)
    {
        this->m_Type = TheBrick::EGameObjectType::Bullet;
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