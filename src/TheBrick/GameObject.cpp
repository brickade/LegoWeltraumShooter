#include "include/TheBrick/GameObject.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CGameObject::CGameObject()
    {

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
            a_pSerializer->Read(&brick->m_Transform, sizeof(brick->m_Transform));
            //m_pCollider

            //m_Color
            a_pSerializer->Read(&brick->m_Color, sizeof(brick->m_Color));
            //brick->m_pBrick->m_pColliderData //Create Collider
            this->m_pBricks.push_back(brick);
        }
        //m_Transform
        a_pSerializer->Read(&this->m_Transform, sizeof(this->m_Transform));
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
            a_pSerializer->Write(&brick->m_Transform, sizeof(brick->m_Transform));
            //m_Color
            a_pSerializer->Write(&brick->m_Color, sizeof(brick->m_Color));
        }
        //m_Transform
        a_pSerializer->Write(&this->m_Transform, sizeof(this->m_Transform));
        //m_pBody
    }
}