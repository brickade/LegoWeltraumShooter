#include "include/Space.h"

namespace sba
{
    Space* Space::g_pInstance = 0;

    // **************************************************************************
    // **************************************************************************
    Space::Space()
    {
        this->BrickManager = new TheBrick::CBrickManager();
        this->World = new ong::World();
    }

    // **************************************************************************
    // **************************************************************************
    Space::~Space()
    {

    }

    // **************************************************************************
    // **************************************************************************
    Space* Space::Instance()
    {
        if (Space::g_pInstance == nullptr)
        {
            Space::g_pInstance = new Space();
        }
        return Space::g_pInstance;
    }

    // **************************************************************************
    // **************************************************************************
    void Space::Initialize(PuRe_IGraphics& a_pGraphics, PuRe_SoundPlayer& a_pSoundPlayer)
    {
#ifdef EDITOR
        this->Renderer = new PuRe_Renderer(&a_pGraphics, PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth, a_pGraphics.GetDescription().ResolutionHeight));
#else
        this->Renderer = new PuRe_Renderer(&a_pGraphics, PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth / 2, a_pGraphics.GetDescription().ResolutionHeight));
#endif
        this->Renderer->m_pPostCamera->setNearFar(PuRe_Vector2F(0.01f, 1000.0f));
        this->m_SSAOMaterial = a_pGraphics.LoadMaterial("../data/effects/SSAO/default");
        this->Renderer->SetSSAO(this->m_SSAOMaterial, "../data/textures/ssao.jpg");
        this->BrickManager->Initialize(a_pGraphics, a_pSoundPlayer);
    }

    // **************************************************************************
    // **************************************************************************
    void Space::UpdatePhysics(PuRe_Timer* a_pTimer)
    {
        if (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate)
        {
            do
            {
                this->m_LastPhysicsUpdate += 1 / this->m_PhysicsFramerate;
                Space::Instance()->World->step(1 / this->m_PhysicsFramerate);
            } while (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate);
            Space::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances
        }
    }
}