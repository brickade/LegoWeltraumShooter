#include "include/TheBrick/Asteroid.h"

#include "Onager/World.h"
#include "include/TheBrick/BrickManager.h"
#include "include/TheBrick/BrickInstance.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CAsteroid::CAsteroid(CBrickManager* a_pBrickManager, ong::World& a_rWorld, ong::vec3 a_Position) : CGameObject(a_rWorld, nullptr)
    {
        this->m_Type = EGameObjectType::Object;
        //m_Color
        this->m_pBody->setPosition(a_Position);
    }

    // **************************************************************************
    // **************************************************************************
    CAsteroid::~CAsteroid()
    {
        //for (int i = 0; i < this->m_pBricks.size(); i++)
        //{
        //    SAFE_DELETE(this->m_pBricks[i]);
        //    this->m_pBricks.erase(this->m_pBricks.begin() + i);
        //}
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        DrawBody(this->m_pBody, a_pCamera, a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Update(float a_DeltaTime)
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld)
    {
        CGameObject::Deserialize(a_pSerializer, a_pBrickManager, a_pWorld);
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Serialize(CSerializer& a_pSerializer)
    {
        CGameObject::Serialize(a_pSerializer);
    }
}