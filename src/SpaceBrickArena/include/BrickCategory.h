#ifndef _BRICKCATEGORY_H_
#define _BRICKCATEGORY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/Brick.h>
#include <TheBrick/BrickTimer.h>

#include "BrickBozz.h"
#include "Navigation.h"

namespace Game
{
    class CBrickCategory
    {

    public:
        int m_Id;
        TheBrick::CBrick** m_Bricks;
        CNavigation* m_pNavigation;

    private:
        float m_ElementSize = 50;
        PuRe_Vector2F m_ListStart = PuRe_Vector2F(100, 150);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(150, 150);

        float m_Pitch = 0.5f;
        float m_RotationSpeed = 1;
        float m_Rotation;

    public:
        CBrickCategory(int a_Id);
        ~CBrickCategory();

        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, int a_PlayerIdx);
        void Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);
    };
}

#endif /* _BRICKCATEGORY_H_ */