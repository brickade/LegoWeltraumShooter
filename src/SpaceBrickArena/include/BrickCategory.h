#ifndef _BRICKCATEGORY_H_
#define _BRICKCATEGORY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/Brick.h>
#include <TheBrick/BrickTimer.h>

#include "BrickBozz.h"

namespace Game
{
    class CBrickCategory
    {
        enum EDirection
        {
            None,
            Up,
            Down,
            Left,
            Right,
        };

    public:
        int m_Id;
        TheBrick::CBrick** m_Bricks;
        int m_BricksPerLine;
        int m_LastBrickSubId;
        int m_FocusedBrick;
        
        PuRe_Vector2F m_FocusPosition;
        EDirection m_PreviousState;
        float m_GamepadThreshold;
        float m_ScrollingThreshold;
        TheBrick::CBrickTimer* m_pTimer;

    public:
        CBrickCategory(int a_Id);
        ~CBrickCategory();

        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, int a_PlayerIdx);
        void Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

    private:
        EDirection ThumbStateFromThumbVector(PuRe_Vector2F a_ThumbInput) const;
        void NavigateList(EDirection a_Direction);
        void Scroll(PuRe_Vector2F a_Input, float a_DeltaSec);

    };
}

#endif /* _BRICKCATEGORY_H_ */