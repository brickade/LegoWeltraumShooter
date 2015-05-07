#include "include/BrickSupervisor.h"
namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CBrickSupervisor::CBrickSupervisor(int a_PlayerIdx)
    {
        this->m_PlayerIdx = a_PlayerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    CBrickSupervisor::~CBrickSupervisor()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Initialize()
    {
        int catCount = BrickBozz::Instance()->BrickManager->GetCategoryCount();
        assert(this->m_Categories.max_size() >= catCount);
        for (int i = 0; i < catCount; i++)
        {
            this->m_Categories[i] = new CBrickCategory(i);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        this->m_pActiveCategory->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer, this->m_PlayerIdx);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        this->m_pActiveCategory->Render(a_pGraphics, a_pCamera);
    }
}