#include "include/Asteroid.h"

#include "Onager/World.h"
#include "TheBrick/BrickInstance.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CAsteroid::CAsteroid(ong::World& a_rWorld, ong::vec3 a_Position, ong::vec3 a_Velocity, ong::vec3 a_Rotation) : CGameObject(a_rWorld, nullptr)
    {
        this->m_Type = TheBrick::EGameObjectType::Object;
        this->m_pBody->setPosition(a_Position);
        this->m_pBody->applyImpulse(a_Velocity);
        this->m_pBody->applyAngularImpulse(a_Rotation);
    }

    // **************************************************************************
    // **************************************************************************
    CAsteroid::~CAsteroid()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Update(float a_DeltaTime)
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld)
    {
        CGameObject::Deserialize(a_pSerializer, a_rBricks, a_pWorld);
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Serialize(TheBrick::CSerializer& a_pSerializer)
    {
        CGameObject::Serialize(a_pSerializer);
    }
}