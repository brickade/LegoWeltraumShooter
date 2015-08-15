#include "include/Editor_BrickSupervisor.h"

#include "include/Space.h"
#include "include/InputManager.h"

namespace Editor
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
        SAFE_DELETE(this->m_pNavigation);
        for (int i = 0; i < this->m_CategoriesCount; i++)
        {
            SAFE_DELETE(this->m_Categories[i]);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Initialize(PuRe_IGraphics& a_pGraphics)
    {
        
        this->m_CategoriesCount = sba_BrickManager->GetCategoryCount()-1; // not 900

        assert(this->m_Categories.max_size() >= (size_t)this->m_CategoriesCount);
        for (int i = 0; i < this->m_CategoriesCount; i++)
        {
            this->m_Categories[i] = new CBrickCategory(i);
        }
        this->m_pNavigation = new sba::CNavigation(1, this->m_CategoriesCount - 1);
        this->m_pActiveCategory = this->m_Categories[0];
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer)
    {
        
        if (sba_Input->ButtonIsPressed(sba_Button::EditorToggleNavigateCategories, this->m_PlayerIdx))
        {
            this->m_NavigateTabs = true;
        }
        else
        {
            this->m_NavigateTabs = false;
            this->m_TabRotation = 0.4f;
        }
        if (this->m_NavigateTabs)
        {
            this->m_pNavigation->Update(a_pTimer, sba_Input->Direction(sba_Direction::EditorChooseElement, this->m_PlayerIdx));
            this->m_pActiveCategory = this->m_Categories[this->m_pNavigation->GetFocusedElementId()];
            this->m_TabRotation += a_pTimer.GetElapsedSeconds() * this->m_RotationSpeed;
            this->m_TabRotation = fmod(this->m_TabRotation, 6.28318531f);
        }
        else
        {
            this->m_pActiveCategory->Update(a_pGraphics, a_pWindow, a_pTimer, a_pSoundPlayer, this->m_PlayerIdx, this->m_RotationSpeed);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Render(PuRe_IGraphics& a_pGraphics, sba::CSpriteReader& a_rSpriteReader, float a_Visibility, const PuRe_Color& a_rCurrentColor)
    {
        float inset = 380;
        a_rSpriteReader.Draw(1, sba_Renderer, "editor_brickselect", sba_Space->FontMaterial, PuRe_Vector3F(inset - (1.0f - a_Visibility) * 750, 1080/2 + 50, 0), PuRe_Vector3F::Zero(), -1, PuRe_Vector2F(0.7f, 0.7f)); //I need to render the UI with the font material? WHAT THE F%*$&!?!? And the position is defined form the center? Double WTF
        for (int i = 0; i < this->m_CategoriesCount; i++)
        {
            bool selected = false;
            if (this->m_Categories[i] == this->m_pActiveCategory)
            {
                selected = true;
            }
            this->m_Categories[i]->RenderTab(a_pGraphics, this->m_TabRotation, selected, a_Visibility);
        }
        this->m_pActiveCategory->Render(a_pGraphics, a_Visibility, a_rCurrentColor);
        sba_Space->RenderFont("Hold   to change category", PuRe_Vector2F(50 - (1.0f - a_Visibility) * 750, 140), 14.0f);
        if (sba_Input->FirstPlayerHasGamepad())
        {
            sba_ButtonsDraw("right_stick", PuRe_Vector2F(134 - (1.0f - a_Visibility) * 750, 143), 0.12f);
        }
        else
        {
            sba_Space->RenderFont("X", PuRe_Vector2F(122 - (1.0f - a_Visibility) * 750, 140), 14.0f);
        }
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrick* CBrickSupervisor::GetSelectedBrick() const
    {
        return this->m_pActiveCategory->GetFocusedBrick();
    }
}