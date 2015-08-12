#include "include/Static.h"

#include "Onager/World.h"
#include "TheBrick/BrickInstance.h"
#include "include/Spaceship.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CStatic::CStatic(ong::World& a_rWorld, ong::BodyDescription* a_desc)
        : TheBrick::CGameObject(a_rWorld, a_desc)
    {
        this->m_Type = TheBrick::EGameObjectType::Object;
    }

    // **************************************************************************
    // **************************************************************************
    CStatic::~CStatic()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CStatic::Update(float a_DeltaTime)
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CStatic::Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld)
    {
        TheBrick::CGameObject::Deserialize(a_pSerializer, a_rBricks, a_pWorld);
    }

    // **************************************************************************
    // **************************************************************************
    void CStatic::Serialize(TheBrick::CSerializer& a_pSerializer)
    {
        TheBrick::CGameObject::Serialize(a_pSerializer);
    }
}