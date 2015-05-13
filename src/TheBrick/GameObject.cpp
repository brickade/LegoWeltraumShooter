#include "include/TheBrick/GameObject.h"
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

    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        ong::Transform transform = this->m_pBody->getTransform();
        for (size_t i = 0; i < this->m_pBricks.size(); i++)
        {
            this->m_pBricks[i]->Draw(a_pGraphics, a_pCamera, transform);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager, ong::World* a_pWorld)
    {
        //m_pBricks
        unsigned int bricksSize = a_pSerializer->ReadIntUnsigned();
        for (unsigned int i = 0; i < bricksSize; i++)
        {
            //m_pBrick
            int brickId = a_pSerializer->ReadInt();
            CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(brickId), a_pWorld);
            //m_Transform
            a_pSerializer->Read(&brick->GetTransform(), sizeof(*&brick->GetTransform()));
            //m_pCollider: Add Collider to Body
            for (int i = 0; i < brick->m_pCollider.size(); i++)
            {
                this->m_pBody->addCollider(brick->m_pCollider[i]);
            }
            //m_Color
            a_pSerializer->Read(&brick->m_Color, sizeof(*&brick->m_Color));
            this->m_pBricks.push_back(brick);
        }
        //m_pBody
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Serialize(CSerializer* a_pSerializer)
    {
        //m_pBricks
        a_pSerializer->Write(this->m_pBricks.size());
        for (unsigned int i = 0; i < this->m_pBricks.size(); i++)
        {
            //m_pBrick
            CBrickInstance* brick = this->m_pBricks[i];
            a_pSerializer->Write(brick->m_pBrick->GetBrickId());
            //m_Transform
            a_pSerializer->Write(&brick->GetTransform(), sizeof(*&brick->GetTransform()));
            //m_Color
            a_pSerializer->Write(&brick->m_Color, sizeof(*&brick->m_Color));
        }
        //m_pBody
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::AddBrickInstance(const CBrickInstance& a_BrickInstance)
    {
        CBrickInstance* brickInstance = new CBrickInstance(a_BrickInstance);
        for (int i = 0; i < brickInstance->m_pCollider.size(); i++)
        {
            this->m_pBody->addCollider(brickInstance->m_pCollider[i]);
        }
        this->m_pBricks.push_back(brickInstance);
    }
}