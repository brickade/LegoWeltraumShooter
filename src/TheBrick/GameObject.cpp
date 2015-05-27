#include "include/TheBrick/GameObject.h"

#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/Serializer.h"
#include "include/TheBrick/BrickManager.h"
#include "include/TheBrick/Brick.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CGameObject::CGameObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc)
    {
        if (a_pBodyDesc == nullptr)
        {
            ong::BodyDescription body;
            body.transform = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
            body.type = ong::BodyType::Dynamic;
            body.angularMomentum = ong::vec3(0, 0, 0);
            body.linearMomentum = ong::vec3(0, 0, 0);
            a_pBodyDesc = &body;
        }
        this->m_pBody = a_rWorld.createBody(*a_pBodyDesc);
    }

    // **************************************************************************
    // **************************************************************************
    CGameObject::~CGameObject()
    {
        SAFE_DELETE_ARRAY(this->m_pBricks.data()); //Delete BrickInstances
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::Update(float a_DeltaTime)
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld)
    {
        //m_pBricks
        unsigned int bricksSize = a_pSerializer.ReadIntUnsigned();
        for (unsigned int i = 0; i < bricksSize; i++)
        {
            //m_pBrick
            int brickId = a_pSerializer.ReadInt();
            CBrickInstance* brick = new CBrickInstance(a_pBrickManager.GetBrick(brickId), *this, a_pWorld);
            //m_Transform
            ong::Transform transform;
            a_pSerializer.Read(&transform, sizeof(transform));
            brick->SetTransform(transform);
            //m_pCollider: Add Collider to Body
            for (int i = 0; i < brick->m_pCollider.size(); i++)
            {
                this->m_pBody->addCollider(brick->m_pCollider[i]);
            }
            //m_Color
            a_pSerializer.Read(&brick->m_Color, sizeof(*&brick->m_Color));
            this->m_pBricks.push_back(brick);
        }
        //m_pBody
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Serialize(CSerializer& a_pSerializer)
    {
        //m_pBricks
        a_pSerializer.Write(this->m_pBricks.size());
        for (unsigned int i = 0; i < this->m_pBricks.size(); i++)
        {
            //m_pBrick
            CBrickInstance* brick = this->m_pBricks[i];
            a_pSerializer.Write(brick->m_pBrick->GetBrickId());
            //m_Transform
            a_pSerializer.Write(&brick->GetTransform(), sizeof(*&brick->GetTransform()));
            //m_Color
            a_pSerializer.Write(&brick->m_Color, sizeof(*&brick->m_Color));
        }
        //m_pBody
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::AddBrickInstance(CBrickInstance* a_pBrickInstance, ong::World& a_rWorld)
    {
        for (size_t i = 0; i < a_pBrickInstance->m_pCollider.size(); i++)
        {
            this->m_pBody->addCollider(a_pBrickInstance->m_pCollider[i]);
        }
        this->m_pBricks.push_back(a_pBrickInstance);
        a_pBrickInstance->SetGameObject(this);
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::RemoveBrickInstance(const CBrickInstance& a_BrickInstance)
    {
        for (size_t i = 0; i < a_BrickInstance.m_pCollider.size(); i++)
        {
            this->m_pBody->removeCollider(a_BrickInstance.m_pCollider[i]);
        }
        for (std::vector<CBrickInstance*>::iterator it = this->m_pBricks.begin(); it != this->m_pBricks.end(); ++it)
        {
            if (*it == &a_BrickInstance)
            {
                this->m_pBricks.erase(it);
                break;
            }
        }
    }
}