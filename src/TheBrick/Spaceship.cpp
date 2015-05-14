#include "include/TheBrick/Spaceship.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CSpaceship::CSpaceship()
    {

    }

    // **************************************************************************
    // **************************************************************************
    CSpaceship::~CSpaceship()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        for (unsigned int i = 0; i<this->m_pBricks.size(); i++)
        {
            this->m_pBricks[i]->m_Transform = this->m_pBody->getTransform();
            this->m_pBricks[i]->Draw(a_pGraphics, a_pCamera);
        }
        DrawBody(this->m_pBody,a_pCamera,a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::HandleInput(PuRe_Camera* a_pCamera, PuRe_IInput* a_pInput, float a_DeltaTime)
    {
        PuRe_Vector3F forward = a_pCamera->GetForward();

        ong::vec3 currVel = ong::rotate(this->m_pBody->getLinearVelocity(), ong::conjugate(this->m_pBody->getOrientation()));
        ong::vec3 targetVel = ong::vec3(0, 0, 2.0f);

        if (a_pInput->GetGamepadRightTrigger(0) > 0.2f)
            this->m_pBody->applyRelativeForce(ong::hadamardProduct(ong::vec3(0, 0, 1.0f), ong::normalize(targetVel - currVel)), a_DeltaTime);

        PuRe_Vector2F leftThumb = a_pInput->GetGamepadLeftThumb(0);
        if (leftThumb.Length() > 1.0f)
        {
            this->m_pBody->applyAngularImpulse(ong::vec3(0.0f, leftThumb.X / 10.0f,0.0f));
        }

        this->m_Transform = this->m_pBody->getTransform();
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Update(float a_DeltaTime)
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager, ong::World* a_pWorld)
    {
        CGameObject::Deserialize(a_pSerializer, a_pBrickManager, a_pWorld);

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Serialize(CSerializer* a_pSerializer)
    {
        //GameObject
        reinterpret_cast<CGameObject*>(this)->Serialize(a_pSerializer);

    }
}