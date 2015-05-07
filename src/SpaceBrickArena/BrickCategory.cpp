#include "include/BrickCategory.h"
namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CBrickCategory::CBrickCategory(int a_Id)
    {
        /*this->m_Bricks = BrickBozz::Instance()->BrickManager->GetCategoryStart(a_Id);
        this->m_LastBrickSubId = 0;
        while (this->m_Bricks[this->m_LastBrickSubId + 1] != nullptr)
        {
            this->m_LastBrickSubId++;
        }*/
        this->m_LastBrickSubId = 99;
        this->m_FocusedBrick = 0;
        this->m_FocusPosition = PuRe_Vector2F::Zero();
        this->m_GamepadThreshold = 0.2f;
        this->m_ScrollingThreshold = 0.5f;
    }

    // **************************************************************************
    // **************************************************************************
    CBrickCategory::~CBrickCategory()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, int a_PlayerIdx)
    {
        float speed = a_pTimer->GetElapsedSeconds();
        //Gamepad
        PuRe_Vector2F leftThumb = a_pInput->GetGamepadLeftThumb(a_PlayerIdx);
        EDirection currentState = ThumbStateFromThumbVector(leftThumb);
        if (currentState != this->m_PreviousState)
        {
            switch (currentState)
            {
            case EDirection::None:
                this->m_FocusPosition = PuRe_Vector2F::Zero(); //Stop scrolling
                break;
            default:
                this->NavigateList(currentState); //Navigate before scrolling
                break;
            }
        }
        else //Scrolling
        {
            this->Scroll(leftThumb, speed);
        }

        //Keyboard
        if (a_pInput->KeyPressed(a_pInput->Right))
        {
            this->NavigateList(EDirection::Right);
        }
        if (a_pInput->KeyPressed(a_pInput->Left))
        {
            this->NavigateList(EDirection::Left);
        }
        if (a_pInput->KeyPressed(a_pInput->Up))
        {
            this->NavigateList(EDirection::Up);
        }
        if (a_pInput->KeyPressed(a_pInput->Down))
        {
            this->NavigateList(EDirection::Down);
        }

        printf("BrickCategory %i: %i", this->m_Id, this->m_FocusedBrick);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {

    }

    // **************************************************************************
    // **************************************************************************
    CBrickCategory::EDirection CBrickCategory::ThumbStateFromThumbVector(PuRe_Vector2F a_ThumbInput) const
    {
        EDirection state = EDirection::None;
        if (a_ThumbInput.X > this->m_GamepadThreshold)
        {
            state = EDirection::Right;
        }
        else if (a_ThumbInput.X < this->m_GamepadThreshold)
        {
            state = EDirection::Left;
        }
        else if (a_ThumbInput.Y > this->m_GamepadThreshold)
        {
            state = EDirection::Up;
        }
        else if (a_ThumbInput.Y < this->m_GamepadThreshold)
        {
            state = EDirection::Down;
        }
        return state;
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::NavigateList(CBrickCategory::EDirection a_Direction)
    {
        switch (a_Direction)
        {
        case EDirection::Right:
            this->m_FocusedBrick++;
            break;
        case EDirection::Left:
            this->m_FocusedBrick--;
            break;
        case EDirection::Up:
            this->m_FocusedBrick -= this->m_BricksPerLine;
            break;
        case EDirection::Down:
            this->m_FocusedBrick += this->m_BricksPerLine;
            break;
        }
        //Jump Start <-> End
        if (this->m_FocusedBrick < 0)
        {
            this->m_FocusedBrick = this->m_LastBrickSubId;
        }
        else if (this->m_FocusedBrick > this->m_LastBrickSubId)
        {
            this->m_FocusedBrick = 0;
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Scroll(PuRe_Vector2F a_Input, float a_DeltaSec)
    {
        this->m_FocusPosition += a_Input * a_DeltaSec;
        if (this->m_FocusPosition.X > this->m_ScrollingThreshold)
        {
            NavigateList(EDirection::Right);
            this->m_FocusPosition.X -= this->m_ScrollingThreshold;
        }
        else if (this->m_FocusPosition.X < this->m_ScrollingThreshold)
        {
            NavigateList(EDirection::Left);
            this->m_FocusPosition.X += this->m_ScrollingThreshold;
        }
        else if (this->m_FocusPosition.Y > this->m_ScrollingThreshold)
        {
            NavigateList(EDirection::Up);
            this->m_FocusPosition.Y -= this->m_ScrollingThreshold;
        }
        else if (this->m_FocusPosition.Y < this->m_ScrollingThreshold)
        {
            NavigateList(EDirection::Down);
            this->m_FocusPosition.Y += this->m_ScrollingThreshold;
        }
    }
}