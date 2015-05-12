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
        SAFE_DELETE(this->m_pCamera);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Initialize(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
        this->m_pCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
        this->m_pCamera->setNearFar(PuRe_Vector2F(0.1f, 1000));
        this->m_CategoriesCount = BrickBozz::Instance()->BrickManager->GetCategoryCount();
        assert(this->m_Categories.max_size() >= this->m_CategoriesCount);
        for (int i = 0; i < this->m_CategoriesCount; i++)
        {
            this->m_Categories[i] = new CBrickCategory(i);
        }
        this->m_pActiveCategory = this->m_Categories[0];
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        this->m_pActiveCategory->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer, this->m_PlayerIdx);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Render(PuRe_IGraphics* a_pGraphics)
    {
        for (int i = 0; i < this->m_CategoriesCount; i++)
        {
            bool selected = false;
            if (this->m_Categories[i] == this->m_pActiveCategory)
            {
                selected = true;
            }
            this->m_Categories[i]->RenderTab(a_pGraphics, this->m_pCamera, selected);
        }
        this->m_pActiveCategory->Render(a_pGraphics, this->m_pCamera);
    }
}