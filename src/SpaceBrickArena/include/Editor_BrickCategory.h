#ifndef _EDITORBRICKCATEGORY_H_
#define _EDITORBRICKCATEGORY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/Brick.h>
#include <TheBrick/BrickTimer.h>

#include "Navigation.h"

namespace Editor
{
    class CBrickCategory
    {

    public:
        static const int CATEGORY_WEAPONS;
        static const int CATEGORY_ENGINES;
        static const int CATEGORY_COCKPITS;



        int m_Id;
        TheBrick::CBrick** m_Bricks;
        sba::CNavigation* m_pNavigation;

    private:
        float m_ElementSize = 30;
        PuRe_Vector2F m_ListStart = PuRe_Vector2F(300, 150);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(150, 140);

        float m_Pitch = 0.5f;
        float m_Rotation = 0.4f;

        int m_ListScrollStart = 3;
        int m_ListShowMax = 6;
        int m_ListScrollCurrent = 0;

        float m_TabSize = 20;
        PuRe_Vector2F m_TabStart = PuRe_Vector2F(100, 120);
        PuRe_Vector2F m_TabStep = PuRe_Vector2F(0, 80);

    public:
        CBrickCategory(int a_Id);
        ~CBrickCategory();

        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, int a_PlayerIdx, float a_RotationSpeed);
        void Render(PuRe_IGraphics& a_pGraphics, float a_Visibility);
        void RenderTab(PuRe_IGraphics& a_pGraphics, float a_TabRotation, bool a_IsSelected, float a_Visibility);

        TheBrick::CBrick* GetFocusedBrick() const;
    };
}

#endif /* _EDITORBRICKCATEGORY_H_ */