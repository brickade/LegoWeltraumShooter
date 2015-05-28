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
}