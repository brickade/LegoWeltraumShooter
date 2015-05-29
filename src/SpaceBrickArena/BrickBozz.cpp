#include "include/BrickBozz.h"

namespace Game
{
    BrickBozz* BrickBozz::g_pInstance = 0;

    // **************************************************************************
    // **************************************************************************
    BrickBozz::BrickBozz()
    {
        this->BrickManager = new TheBrick::CBrickManager();
        this->World = new ong::World();
    }

    // **************************************************************************
    // **************************************************************************
    BrickBozz::~BrickBozz()
    {

    }

    // **************************************************************************
    // **************************************************************************
    BrickBozz* BrickBozz::Instance()
    {
        if (BrickBozz::g_pInstance == nullptr)
        {
            BrickBozz::g_pInstance = new BrickBozz();
        }
        return BrickBozz::g_pInstance;
    }

    // **************************************************************************
    // **************************************************************************
    void BrickBozz::Initialize(PuRe_IGraphics& a_pGraphics, PuRe_SoundPlayer& a_pSoundPlayer)
    {
        this->Renderer = new PuRe_Renderer(&a_pGraphics, PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth, a_pGraphics.GetDescription().ResolutionHeight));
        this->Renderer->m_pPostCamera->setNearFar(PuRe_Vector2F(0.01f, 1000.0f));
        this->m_SSAOMaterial = a_pGraphics.LoadMaterial("../data/effects/SSAO/default");
        this->Renderer->SetSSAO(this->m_SSAOMaterial, "../data/textures/ssao.jpg");
        this->BrickManager->Initialize(a_pGraphics, a_pSoundPlayer);
    }

    // **************************************************************************
    // **************************************************************************
    void BrickBozz::UpdatePhysics(PuRe_Timer* a_pTimer)
    {
        if (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate)
        {
            do
            {
                this->m_LastPhysicsUpdate += 1 / this->m_PhysicsFramerate;
                BrickBozz::Instance()->World->step(1 / this->m_PhysicsFramerate);
            } while (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate);
            BrickBozz::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances
        }
    }
}