#include "include/TheBrick/Asteroid.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CAsteroid::CAsteroid(ong::World& a_rWorld, ong::vec3 a_Position) : CGameObject(a_rWorld, nullptr)
    {
        //m_pBody
        ong::BodyDescription bdesc;
        bdesc.transform = ong::Transform(a_Position, ong::Quaternion(ong::vec3(0, 0, 0), 1));
        bdesc.type = ong::BodyType::Dynamic;
        bdesc.angularMomentum = ong::vec3(0, 0, 0); //rotation speed
        bdesc.linearMomentum = ong::vec3(0, 0, 0);  //movement speed
        this->m_pBody = a_rWorld.createBody(bdesc);
    }

    // **************************************************************************
    // **************************************************************************
    CAsteroid::~CAsteroid()
    {
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
        //GameObject
        reinterpret_cast<CGameObject*>(this)->Serialize(a_pSerializer);
    }
}