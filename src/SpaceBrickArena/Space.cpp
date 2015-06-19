#include "include/Space.h"
#include "include/INIReader.h"

namespace sba
{
    Space* Space::g_pInstance = 0;

    // **************************************************************************
    // **************************************************************************
    Space::Space()
    {
        this->World = new ong::World();
        this->InputManager = new sba::CInputManager();
        this->BrickManager = new sba::CBrickManager("../data/bricks/");
        this->ShipManager = new sba::CShipManager("../data/ships/");
    }

    // **************************************************************************
    // **************************************************************************
    Space::~Space()
    {
        SAFE_DELETE(this->Renderer);
        SAFE_DELETE(this->m_SSAOMaterial);
        SAFE_DELETE(this->ShipManager);
        SAFE_DELETE(this->BrickManager);
        SAFE_DELETE(this->InputManager);
        SAFE_DELETE(this->World);
    }

    // **************************************************************************
    // **************************************************************************
    void Space::Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer, PuRe_Application& a_rpApplication)
    {
        this->Renderer = new PuRe_Renderer(&a_pGraphics);
        for (int i = 0; i < 3; i++)
        {
            this->Renderer->AddTarget(PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth, a_pGraphics.GetDescription().ResolutionHeight));
        }
        if (CIniReader::Instance()->GetValue("SSAO") == "On")
        {
            this->m_SSAOMaterial = a_pGraphics.LoadMaterial("../data/effects/SSAO/default");
            this->Renderer->SetSSAO(this->m_SSAOMaterial, "../data/textures/ssao.jpg");
        }
        this->BrickManager->Initialize();
        this->InputManager->Initialize();
        this->Font = new PuRe_Font(&a_pGraphics, "../data/textures/font.png");
        this->FontMaterial = a_pGraphics.LoadMaterial("../data/effects/font/default");

        this->Application = &a_rpApplication;
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

    // **************************************************************************
    // **************************************************************************
    void Space::RenderFont(std::string a_Text, PuRe_Vector2F a_Position, float a_Size, float a_Width, unsigned int a_RendertargetIndex)
    {
        this->Renderer->Draw(a_RendertargetIndex, false, this->Font, this->FontMaterial, a_Text, PuRe_Vector3F(a_Position, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F(a_Size, a_Size, a_Size), a_Size * a_Width);
    }
}