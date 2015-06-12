#ifndef _UIELEMENTFADER_H_
#define _UIELEMENTFADER_H_

#include <PuReEngine/Defines.h>

#include "Space.h"

namespace sba
{
    class CUIElementFader
    {

    public:

    private:
        int m_PlayerIdx;
        sba_Button::Type m_ToggleButton;

        float m_FadingSpeed;

        float m_Value;
        bool m_Visible;

    public:
        float GetVisibility() const
        {
            return this->m_Value;
        }

        void SetVisibility(float a_Visibility)
        {
            this->m_Value = a_Visibility;
        }

        bool IsVisible() const
        {
            return this->m_Visible;
        }

        void Hide()
        {
            this->m_Visible = false;
        }
        
        void Show()
        {
            this->m_Visible = true;
        }

        void Toggle()
        {
            this->m_Visible = !this->m_Visible;
        }

    public:
        CUIElementFader(sba_Button::Type a_ToggleButton, int a_PlayerIdx, bool a_InitVisible = false, float a_FadingSpeed = 1.0f);
        ~CUIElementFader();

        void Update(PuRe_Timer& a_rTimer);
    };
}

#endif /* _UIELEMENTFADER_H_ */