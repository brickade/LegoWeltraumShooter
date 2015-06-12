#include "include/UIElementFader.h"

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
    void CUIElementFader::Update(PuRe_Timer& a_rTimer)
    {
        float speed = a_rTimer.GetElapsedSeconds() * this->m_FadingSpeed;
        if (!this->m_Visible)
        {
            speed *= -1;
        }
        this->m_Value += speed * (0.5f - abs(this->m_Value - 0.5f));
        this->m_Value = PuRe_clamp(this->m_Value, 0, 1);
    }
}