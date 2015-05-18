#include "include/TheBrick/Asteroid.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CAsteroid::CAsteroid()
    {
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
        for (unsigned int i = 0; i<this->m_pBricks.size(); i++)
        {
            this->m_pBricks[i]->m_Transform = ong::transformTransform(this->m_pBricks[i]->m_Transform, this->m_pBody->getTransform());
            this->m_pBricks[i]->Draw(a_pGraphics, a_pCamera);
            this->m_pBricks[i]->m_Transform = ong::invTransformTransform(this->m_pBricks[i]->m_Transform, this->m_pBody->getTransform());
        }
        DrawBody(this->m_pBody, a_pCamera, a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Update(float a_DeltaTime)
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager, ong::World* a_pWorld)
    {
        //m_pBody
        ong::BodyDescription bdesc;
        bdesc.transform = ong::Transform(ong::vec3(0.0f, 0.0f, 10.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        bdesc.type = ong::BodyType::Dynamic;
        bdesc.angularMomentum = ong::vec3(0, 0, 0); //rotation speed
        bdesc.linearMomentum = ong::vec3(0, 0, 1);  //movement speed
        this->m_pBody = a_pWorld->createBody(bdesc);

        ////////   TESTING ONLY   ////////
        for (int a=0;a<1;a++)
        {
            CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(0), a_pWorld);
            brick->m_Transform = ong::Transform(ong::vec3((a % 2)*1.55f, 0.0f, (a / 2)*1.55f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
            brick->m_Color = PuRe_Color(1, 1, 1, 1);
            for (int i = 0; i<brick->m_pCollider.size(); i++)
            {
                brick->m_pCollider[i]->setTransform(brick->m_Transform);
                this->m_pBody->addCollider(brick->m_pCollider[i]);
            }
            this->m_pBricks.push_back(brick);
        }

        CGameObject::Deserialize(a_pSerializer, a_pBrickManager, a_pWorld);

    }

    // **************************************************************************
    // **************************************************************************
    void CAsteroid::Serialize(CSerializer* a_pSerializer)
    {
        //GameObject
        reinterpret_cast<CGameObject*>(this)->Serialize(a_pSerializer);

    }
}