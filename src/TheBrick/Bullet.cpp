#include "include/TheBrick/Bullet.h"

#include "include/TheBrick/Conversion.h"
#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/DebugDraw.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBullet::CBullet(/*CBrickManager* a_pBrickManager, */PuRe_Vector3F a_Position, PuRe_Vector3F a_Speed, ong::World& a_rWorld) : CGameObject(a_rWorld, nullptr)
    {

        ////////   TESTING ONLY   ////////
        /*Tmp comment out: compile
        CBrickInstance* brick = new CBrickInstance(a_pBrickManager->GetBrick(0), *this, a_rWorld);
        //m_Transform
        brick->SetTransform(ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
        //m_Color
        brick->m_Color = PuRe_Color(0.3f, 0, 0, 1);*/
        this->m_pBody->setPosition(TheBrick::PuReToOng(a_Position));
        this->m_pBody->applyImpulse(TheBrick::PuReToOng(a_Speed));

        this->m_lifeTime = 0.0f;
    }

    // **************************************************************************
    // **************************************************************************
    CBullet::~CBullet()
    {
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