#ifndef _EDITORBRICKCATEGORY_H_
#define _EDITORBRICKCATEGORY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/Brick.h>
#include <TheBrick/BrickTimer.h>

#include "Space.h"
#include "Navigation.h"

namespace Editor
{
    class CBrickCategory
    {

    public:
        int m_Id;
        TheBrick::CBrick** m_Bricks;
        sba::CNavigation* m_pNavigation;

    private:
        float m_ElementSize = 30;
        PuRe_Vector2F m_ListStart = PuRe_Vector2F(300, 150);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(150, 150);

        float m_Pitch = 0.5f;
        float m_Rotation = 0.4f;

        float m_TabSize = 30;
        PuRe_Vector2F m_TabStart = PuRe_Vector2F(100, 150);
        PuRe_Vector2F m_TabStep = PuRe_Vector2F(0, 150);

    public:
        CBrickCategory(int a_Id);
        ~CBrickCategory();

        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_IInput& a_pInput, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, int a_PlayerIdx, float a_RotationSpeed);
        void Render(PuRe_IGraphics& a_pGraphics, PuRe_Camera& a_pCamera);
        void RenderTab(PuRe_IGraphics& a_pGraphics, PuRe_Camera& a_pCamera, float a_TabRotation, bool a_IsSelected);

        TheBrick::CBrick* GetFocusedBrick() const;
    };
}

#endif /* _EDITORBRICKCATEGORY_H_ */