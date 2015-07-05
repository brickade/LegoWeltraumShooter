#include "include/Bullet.h"

#include "TheBrick/Conversion.h"
#include "TheBrick/BrickInstance.h"

#include "include/Player.h"
#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CBullet::CBullet(ong::BodyDescription* a_desc, ong::World& a_rWorld, SPlayer* a_pOwner) : CGameObject(a_rWorld, a_desc)
    {
        this->m_pOwner = a_pOwner;
        this->m_Type = TheBrick::EGameObjectType::Bullet;
        TheBrick::CBrickInstance* brick = new TheBrick::CBrickInstance(*sba_BrickManager->GetBrickArray()[900], *this, *sba_World);

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