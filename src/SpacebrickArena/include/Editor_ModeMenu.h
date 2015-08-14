#ifndef _EDITORMODEMENU_H_
#define _EDITORMODEMENU_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "SpriteReader.h"

namespace Editor
{
    class CModeMenu
    {
    public:

    private:
        int m_PlayerIdx;
        sba::CNavigation* m_pNavigation;

        PuRe_Vector2F m_ListStart = PuRe_Vector2F(1920 - 280, 1080 - 350);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(0, -60);

        PuRe_Vector2F m_InfoStart = PuRe_Vector2F(1920 - 730, 1080 - 520);
        PuRe_Vector2F m_InfoStep = PuRe_Vector2F(0, -40);

        std::vector<std::string> m_InfoText;
        std::vector<std::string> m_InfoControlGamepad;
        std::vector<std::string> m_InfoControlKeyboard;

    public:
        int SelectedMode() const
        {
            return this->m_pNavigation->GetFocusedElementId();
        }
    protected:
        void AddInfo(const std::string& a_rText, std::string a_rControlGamepad, std::string a_rControlKeyboard)
        {
            this->m_InfoText.push_back(a_rText);
            this->m_InfoControlGamepad.push_back(a_rControlGamepad);
            this->m_InfoControlKeyboard.push_back(a_rControlKeyboard);
        }
        void AddInfoSpace()
        {
            this->m_InfoText.push_back("");
            this->m_InfoControlGamepad.push_back("");
            this->m_InfoControlKeyboard.push_back("");
        }

    public:
        CModeMenu(int a_playerIdx);
        ~CModeMenu();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer);
        void Render(PuRe_IGraphics& a_pGraphics, sba::CSpriteReader& a_rSpriteReader, float a_Visibility);
    };
}

#endif /* _EDITORMODEMENU_H_ */