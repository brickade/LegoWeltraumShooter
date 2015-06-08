#include "include/Space.h"
#include "include/INIReader.h"

namespace sba
{
    Space* Space::g_pInstance = 0;

    // **************************************************************************
    // **************************************************************************
    Space::Space()
    {
        this->BrickManager = new TheBrick::CBrickManager();
        this->World = new ong::World();
        this->InputManager = new sba::CInputManager();
    }

    // **************************************************************************
    // **************************************************************************
    Space::~Space()
    {
        SAFE_DELETE(this->Renderer);
        SAFE_DELETE(this->m_SSAOMaterial);
        SAFE_DELETE(this->BrickManager);
        SAFE_DELETE(this->World);
    }

    // **************************************************************************
    // **************************************************************************
    void Space::Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer)
    {
        this->Renderer = new PuRe_Renderer(&a_pGraphics);
#ifdef EDITOR
        this->Renderer->AddTarget(PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth, a_pGraphics.GetDescription().ResolutionHeight));
#else
        this->Renderer->AddTarget(PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth / 2, a_pGraphics.GetDescription().ResolutionHeight));
#endif
        this->Renderer->AddTarget(PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth, a_pGraphics.GetDescription().ResolutionHeight));
        if (CIniReader::Instance()->GetValue("SSAO") == "On")
        {
            this->m_SSAOMaterial = a_pGraphics.LoadMaterial("../data/effects/SSAO/default");
            this->Renderer->SetSSAO(this->m_SSAOMaterial, "../data/textures/ssao.jpg");
        }
        this->BrickManager->Initialize(a_pGraphics, a_pSoundPlayer);
        this->InputManager->Initialize(&a_pInput);
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