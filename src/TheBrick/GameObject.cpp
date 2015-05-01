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
    static CGameObject* Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager)
    {
        CGameObject* gameobject = new CGameObject();
        //m_pBricks
        for (unsigned int i = 0; i < a_pSerializer->ReadIntUnsigned(); i++)
        {
            //m_pBrick
            int brickId = a_pSerializer->ReadInt();
            CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(brickId));
            //m_Transform
            a_pSerializer->Read(&brick->m_Transform, sizeof(brick->m_Transform));
            //m_pCollider
            //brick->m_pBrick->m_pColliderData //Create Collider
            gameobject->m_pBricks.push_back(brick);
        }
        //m_Transform
        a_pSerializer->Read(&gameobject->m_Transform, sizeof(gameobject->m_Transform));
        //m_pBody

        return gameobject;
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
            a_pSerializer->Write(brick->m_pBrick->m_BrickId);
            //m_Transform
            a_pSerializer->Write(&brick->m_Transform, sizeof(brick->m_Transform));
        }
        //m_Transform
        a_pSerializer->Write(&this->m_Transform, sizeof(this->m_Transform));
        //m_pBody

    }
}