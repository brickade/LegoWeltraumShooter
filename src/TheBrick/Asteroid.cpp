#include "include/TheBrick/Asteroid.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CAsteroid::CAsteroid(CBrickManager* a_pBrickManager, ong::World& a_rWorld, ong::vec3 a_Position) : CGameObject(a_rWorld, nullptr)
    {
        ////////   TESTING ONLY   ////////
        for (int i=0;i<2;i++)
        {
            CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(2), a_rWorld);
            //m_Transform
            ong::AABB aabb = brick->m_pCollider[0]->getAABB();
            brick->SetTransform(ong::Transform(ong::vec3(0.0f, i*(aabb.c.y+0.005f)*2.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
            //m_pCollider: Add Collider to Body
            for (int i = 0; i < brick->m_pCollider.size(); i++)
            {
                this->m_pBody->addCollider(brick->m_pCollider[i]);
            }
            brick->m_Color = PuRe_Color(1, 1, 1, 1);
            this->m_pBricks.push_back(brick);
        }
        //m_Color
        this->m_pBody->setPosition(a_Position);
    }

    // **************************************************************************
    // **************************************************************************
    CAsteroid::~CAsteroid()
    {
        for (int i = 0; i < this->m_pBricks.size(); i++)
        {
            SAFE_DELETE(this->m_pBricks[i]);
            this->m_pBricks.erase(this->m_pBricks.begin() + i);
        }
        this->m_pBody->getWorld()->destroyBody(this->m_pBody);
        this->m_pBody = nullptr;
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