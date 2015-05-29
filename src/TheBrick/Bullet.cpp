#include "include/TheBrick/Bullet.h"

#include "include/TheBrick/Conversion.h"
#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/BrickManager.h"
#include "include/TheBrick/DebugDraw.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBullet::CBullet(CBrickManager* a_pBrickManager, PuRe_Vector3F a_Position, PuRe_Vector3F a_Speed, ong::World& a_rWorld) : CGameObject(a_rWorld, nullptr)
    {

        ////////   TESTING ONLY   ////////
        CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(0), *this, a_rWorld);
        //m_Transform
        brick->SetTransform(ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
        //m_Color
        brick->m_Color = PuRe_Color(1, 1, 1, 1);
        this->m_pBody->setPosition(TheBrick::PuReToOng(a_Position));
        this->m_pBody->applyImpulse(TheBrick::PuReToOng(a_Speed));

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
        DrawBody(this->m_pBody, a_pCamera, a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CBullet::Update(float a_DeltaTime)
    {
        this->m_lifeTime += a_DeltaTime;
    }

}