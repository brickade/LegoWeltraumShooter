#include "include/UIElementFader.h"
#include "include/InputManager.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CUIElementFader::CUIElementFader(sba_Button::Type a_ToggleButton, int a_PlayerIdx, bool a_InitVisible, float a_FadingSpeed)
    {
        this->m_ToggleButton = a_ToggleButton;
        this->m_PlayerIdx = a_PlayerIdx;
        this->m_FadingSpeed = a_FadingSpeed;
        this->m_Visible = a_InitVisible;
        this->m_Value = a_InitVisible ? 1.0f : 0.0f;
    }

    // **************************************************************************
    // **************************************************************************
    CUIElementFader::~CUIElementFader()
    {

    }

    // **************************************************************************
    // **************************************************************************
    bool CUIElementFader::Update(PuRe_Timer& a_rTimer)
    {
        bool stateChanged = false;
        if (sba_Input->ButtonIsPressed(sba_Button::EditorFadeHold, this->m_PlayerIdx) && sba_Input->ButtonPressed(this->m_ToggleButton, this->m_PlayerIdx))
        {
            this->Toggle();
            stateChanged = true;
            if (this->m_Visible)
            {
                sba_SoundPlayer->PlaySound("collect", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            }
        }

        float speed = a_rTimer.GetElapsedSeconds() * this->m_FadingSpeed;
        if (!this->m_Visible)
        {
            speed *= -1;
        }
        this->m_Value += speed * (1.0f - abs(this->m_Value - 0.5f));
        this->m_Value = PuRe_clamp(this->m_Value, 0, 1);
        return stateChanged;
    }
}