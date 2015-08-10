#include "include/Asteroid.h"

#include "Onager/World.h"
#include "TheBrick/BrickInstance.h"
#include "include/Spaceship.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CAsteroid::CAsteroid(ong::World& a_rWorld, ong::BodyDescription* a_desc)
        : CDestructibleObject(a_rWorld, a_desc)
    {
        this->m_Type = TheBrick::EGameObjectType::Object;
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
		CDestructibleObject::Deserialize(a_pSerializer, a_rBricks, a_pWorld);
        Build();
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Serialize(TheBrick::CSerializer& a_pSerializer)
    {
		CDestructibleObject::Serialize(a_pSerializer);
    }
}