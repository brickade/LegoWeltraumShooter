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
    void CGameObject::Update(float a_DeltaTime)
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager, ong::World* a_pWorld)
    {
        //m_pBody
        ong::BodyDescription bdesc;
        bdesc.transform = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        bdesc.type = ong::BodyType::Dynamic;
        bdesc.angularMomentum = ong::vec3(0, 0, 0); //rotation speed
        bdesc.linearMomentum = ong::vec3(0, 0, 0);  //movement speed
        this->m_pBody = a_pWorld->createBody(bdesc);

        ////////   TESTING ONLY   ////////

        CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(0), a_pWorld);
        brick->m_Transform = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        brick->m_Color = PuRe_Color(1,1,1,1);
            for (int i = 0; i<brick->m_pCollider.size();i++)
                this->m_pBody->addCollider(brick->m_pCollider[i]);
        this->m_pBricks.push_back(brick);

        ////////////////////////////////
        //unsigned int bricksSize = a_pSerializer->ReadIntUnsigned();
        //for (unsigned int i = 0; i < bricksSize; i++)
        //{
        //    //m_pBrick
        //    int brickId = a_pSerializer->ReadInt();
        //    CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(brickId), a_pWorld);
        //    //m_Transform
        //    a_pSerializer->Read(&brick->m_Transform, sizeof(brick->m_Transform));
        //    //m_pCollider

        //    //m_Color
        //    a_pSerializer->Read(&brick->m_Color, sizeof(brick->m_Color));
        //    //brick->m_pBrick->m_pColliderData //Create Collider
        //    this->m_pBricks.push_back(brick);
        //    //Add coliders to body
        //    for (int i = 0; i<brick->m_pCollider.size();i++)
        //        this->m_pBody->addCollider(brick->m_pCollider[i]);
        //}
        ////m_Transform
        //a_pSerializer->Read(&this->m_Transform, sizeof(this->m_Transform));
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