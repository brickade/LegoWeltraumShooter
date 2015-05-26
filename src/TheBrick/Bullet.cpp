#include "include/TheBrick/Bullet.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBullet::CBullet(CBrickManager* a_pBrickManager, PuRe_Vector3F a_Position, PuRe_Vector3F a_Speed, ong::World& a_rWorld) : CGameObject(a_rWorld, nullptr)
    {
        //m_pBody
        ong::BodyDescription bdesc;
        bdesc.transform = ong::Transform(TheBrick::PuReToOng(a_Position), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        bdesc.type = ong::BodyType::Dynamic;
        bdesc.angularMomentum = ong::vec3(0, 0, 0); //rotation speed
        bdesc.linearMomentum = TheBrick::PuReToOng(a_Speed);  //movement speed

        this->m_pBody = a_pWorld->createBody(bdesc);
        CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(2), a_pWorld);
        brick->m_Transform = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        brick->m_Color = PuRe_Color(1, 1, 1, 1);
        for (int i = 0; i<brick->m_pCollider.size(); i++)
        {
            brick->m_pCollider[i]->setTransform(brick->m_Transform);
            this->m_pBody->addCollider(brick->m_pCollider[i]);
        }
        this->m_pBricks.push_back(brick);

        this->m_lifeTime = 0.0f;
    }

    // **************************************************************************
    // **************************************************************************
    CBullet::~CBullet()
    {
        for (int i = 0; i < this->m_pBricks.size(); i++)
        {
            SAFE_DELETE(this->m_pBricks[i]);
            this->m_pBricks.erase(this->m_pBricks.begin()+i);
        }
        this->m_pBody->getWorld()->destroyBody(this->m_pBody);
        this->m_pBody = nullptr;
    }

    // **************************************************************************
    // **************************************************************************
    void CBullet::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
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
    void CBullet::Update(float a_DeltaTime)
    {
        this->m_lifeTime += a_DeltaTime;
    }

}