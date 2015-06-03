#include "include/InputManager.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CInputManager::CInputManager()
    {
    }

    // **************************************************************************
    // **************************************************************************
    CInputManager::~CInputManager()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Initialize(PuRe_IInput* a_pInput)
    {
        this->m_pInput = a_pInput;

        //Direction
        this->m_pDirectionMapping[Input::EDirection::EditorCameraRotate] =
        {
            Input::EGamepadDirection::RightThumb,
            {
                Input::EKeyboardDirection::ArrowKeys,
                Input::EKeyboardDirection::ArrowKeys,
                Input::EKeyboardDirection::ArrowKeys,
                Input::EKeyboardDirection::ArrowKeys
            }
        };
        this->m_pDirectionMapping[Input::EDirection::EditorMoveBrick] =
        {
            Input::EGamepadDirection::LeftThumb,
            {
                Input::EKeyboardDirection::WSAD,
                Input::EKeyboardDirection::WSAD,
                Input::EKeyboardDirection::WSAD,
                Input::EKeyboardDirection::WSAD
            }
        };
        this->m_pDirectionMapping[Input::EDirection::EditorNavigateCategory] =
        {
            Input::EGamepadDirection::DPad,
            {
                Input::EKeyboardDirection::IKJL,
                Input::EKeyboardDirection::IKJL,
                Input::EKeyboardDirection::IKJL,
                Input::EKeyboardDirection::IKJL
            }
        };

        //Axis
        this->m_pAxisMapping[Input::EAxis::EditorCameraZoom] =
        {
            Input::EGamepadAxis::BothTrigger,
            {
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF
            }
        };
        this->m_pAxisMapping[Input::EAxis::EditorColorSlider] =
        {
            Input::EGamepadAxis::BothTrigger,
            {
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF
            }
        };

        //Button
        this->m_pButtonMapping[Input::EButton::EditorPlaceBrick] =
        {
            Input::EGamepadButton::Pad_A,
            {
                Input::EKeyboardButton::Enter,
                Input::EKeyboardButton::Enter,
                Input::EKeyboardButton::Enter,
                Input::EKeyboardButton::Enter
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorTogglePlacementSide] =
        {
            Input::EGamepadButton::Pad_B,
            {
                Input::EKeyboardButton::Space,
                Input::EKeyboardButton::Space,
                Input::EKeyboardButton::Space,
                Input::EKeyboardButton::Space
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorUndo] =
        {
            Input::EGamepadButton::Pad_X,
            {
                Input::EKeyboardButton::CtrlZ,
                Input::EKeyboardButton::CtrlZ,
                Input::EKeyboardButton::CtrlZ,
                Input::EKeyboardButton::CtrlZ
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorRedo] =
        {
            Input::EGamepadButton::Pad_Y,
            {
                Input::EKeyboardButton::CtrlY,
                Input::EKeyboardButton::CtrlY,
                Input::EKeyboardButton::CtrlY,
                Input::EKeyboardButton::CtrlY
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorRotateBrickRight] =
        {
            Input::EGamepadButton::Right_Shoulder,
            {
                Input::EKeyboardButton::E,
                Input::EKeyboardButton::E,
                Input::EKeyboardButton::E,
                Input::EKeyboardButton::E
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorRotateBrickLeft] =
        {
            Input::EGamepadButton::Left_Shoulder,
            {
                Input::EKeyboardButton::Q,
                Input::EKeyboardButton::Q,
                Input::EKeyboardButton::Q,
                Input::EKeyboardButton::Q
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorResetShip] =
        {
            Input::EGamepadButton::Pad_Start,
            {
                Input::EKeyboardButton::Delete,
                Input::EKeyboardButton::Delete,
                Input::EKeyboardButton::Delete,
                Input::EKeyboardButton::Delete
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorSaveShip] =
        {
            Input::EGamepadButton::Pad_Back,
            {
                Input::EKeyboardButton::CtrlS,
                Input::EKeyboardButton::CtrlS,
                Input::EKeyboardButton::CtrlS,
                Input::EKeyboardButton::CtrlS
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorToggleNavigateCategories] =
        {
            Input::EGamepadButton::Right_Thumb,
            {
                Input::EKeyboardButton::X,
                Input::EKeyboardButton::X,
                Input::EKeyboardButton::X,
                Input::EKeyboardButton::X
            }
        };
        this->m_pButtonMapping[Input::EButton::Exit] =
        {
            Input::EGamepadButton::Pad_Back,
            {
                Input::EKeyboardButton::Escape,
                Input::EKeyboardButton::Escape,
                Input::EKeyboardButton::Escape,
                Input::EKeyboardButton::Escape
            }
        };
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::Direction(Input::EDirection::Type a_Direction, int a_PlayerIndex)
    {
        Input::SDirectionMapping& mapping = this->m_pDirectionMapping[a_Direction];
        return this->GetGamepadDirection(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardDirection(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::Axis(Input::EAxis::Type a_Axis, int a_PlayerIndex)
    {
        Input::SAxisMapping& mapping = this->m_pAxisMapping[a_Axis];
        return this->GetGamepadAxis(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardAxis(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonPressed(Input::EButton::Type a_Button, int a_PlayerIndex)
    {
        Input::SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        return this->GetGamepadButtonPressed(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardButtonPressed(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonIsPressed(Input::EButton::Type a_Button, int a_PlayerIndex)
    {
        Input::SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        return this->GetGamepadButtonIsPressed(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardButtonIsPressed(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::GetKeyboardDirection(Input::EKeyboardDirection::Type a_Direction)
    {
        switch (a_Direction)
        {
        case Input::EKeyboardDirection::ArrowKeys:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->Up, this->m_pInput->Down, this->m_pInput->Right, this->m_pInput->Left);
        case Input::EKeyboardDirection::WSAD:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->W, this->m_pInput->S, this->m_pInput->D, this->m_pInput->A);
        case Input::EKeyboardDirection::IKJL:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->I, this->m_pInput->K, this->m_pInput->L, this->m_pInput->J);
        case Input::EKeyboardDirection::Numpad:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->Eight, this->m_pInput->Five, this->m_pInput->Six, this->m_pInput->Four);
        case Input::EKeyboardDirection::Mouse:
            return this->m_pInput->GetRelativeMousePosition();
        }
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::GetGamepadDirection(Input::EGamepadDirection::Type a_Direction, int a_Index)
    {
        switch (a_Direction)
        {
        case Input::EGamepadDirection::LeftThumb:
            return this->m_pInput->GetGamepadLeftThumb(a_Index);
        case Input::EGamepadDirection::RightThumb:
            return this->m_pInput->GetGamepadRightThumb(a_Index);
        case Input::EGamepadDirection::DPad:
            return this->CalcGamepadDirectionFromButtons(this->m_pInput->DPAD_Up, this->m_pInput->DPAD_Down, this->m_pInput->DPAD_Right, this->m_pInput->DPAD_Left, a_Index);
        case Input::EGamepadDirection::Buttons:
            return this->CalcGamepadDirectionFromButtons(this->m_pInput->Pad_Y, this->m_pInput->Pad_A, this->m_pInput->Pad_B, this->m_pInput->Pad_X, a_Index);
        }
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::GetKeyboardAxis(Input::EKeyboardAxis::Type a_Axis)
    {
        switch (a_Axis)
        {
        case Input::EKeyboardAxis::WS:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->W, this->m_pInput->S);
        case Input::EKeyboardAxis::AD:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->D, this->m_pInput->A);
        case Input::EKeyboardAxis::RF:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->R, this->m_pInput->F);
        case Input::EKeyboardAxis::QE:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->E, this->m_pInput->Q);
        case Input::EKeyboardAxis::ArrowUD:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->Up, this->m_pInput->Down);
        case Input::EKeyboardAxis::ArrowLR:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->Right, this->m_pInput->Left);
        case Input::EKeyboardAxis::MouseX:
            return this->m_pInput->GetRelativeMousePosition().X;
        case Input::EKeyboardAxis::MouseY:
            return this->m_pInput->GetRelativeMousePosition().Y;
        case Input::EKeyboardAxis::MouseScroll:
            return this->m_pInput->GetMouseScroll();
        }
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::GetGamepadAxis(Input::EGamepadAxis::Type a_Axis, int a_Index)
    {
        switch (a_Axis)
        {
        case Input::EGamepadAxis::LeftTrigger:
            return this->m_pInput->GetGamepadLeftTrigger(a_Index);
        case Input::EGamepadAxis::RightTrigger:
            return this->m_pInput->GetGamepadRightTrigger(a_Index);
        case Input::EGamepadAxis::BothTrigger:
            return this->m_pInput->GetGamepadRightTrigger(a_Index) - this->m_pInput->GetGamepadLeftTrigger(a_Index);
        case Input::EGamepadAxis::LeftThumbX:
            return this->m_pInput->GetGamepadLeftThumb(a_Index).X;
        case Input::EGamepadAxis::LeftThumbY:
            return this->m_pInput->GetGamepadLeftThumb(a_Index).Y;
        case Input::EGamepadAxis::RightThumbX:
            return this->m_pInput->GetGamepadRightThumb(a_Index).X;
        case Input::EGamepadAxis::RightThumbY:
            return this->m_pInput->GetGamepadRightThumb(a_Index).Y;
        case Input::EGamepadAxis::DPadX:
            return this->CalcGamepadAxisFromButtons(this->m_pInput->DPAD_Right, this->m_pInput->DPAD_Left, a_Index);
        case Input::EGamepadAxis::DPadY:
            return this->CalcGamepadAxisFromButtons(this->m_pInput->DPAD_Up, this->m_pInput->DPAD_Down, a_Index);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetKeyboardButtonPressed(Input::EKeyboardButton::Type a_Button)
    {
        if (a_Button <= Input::EKeyboardButton::PURE_LAST)
        {
            return this->m_pInput->KeyPressed((PuRe_IInput::EKeys)a_Button);
        }
        switch (a_Button)
        {
        case Input::EKeyboardButton::Delete:
            return this->m_pInput->KeyPressed(this->m_pInput->Backspace);
        case Input::EKeyboardButton::CtrlZ:
            return this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Z);
        case Input::EKeyboardButton::CtrlY:
            return this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Y);
        case Input::EKeyboardButton::CtrlS:
            return this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->S);
        case Input::EKeyboardButton::MouseLeft:
            return this->m_pInput->MousePressed(this->m_pInput->LeftClick);
        case Input::EKeyboardButton::MouseMiddle:
            return this->m_pInput->MousePressed(this->m_pInput->MiddleClick);
        case Input::EKeyboardButton::MouseRight:
            return this->m_pInput->MousePressed(this->m_pInput->RightClick);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetGamepadButtonPressed(Input::EGamepadButton::Type a_Button, int a_Index)
    {
        if (a_Button <= Input::EGamepadButton::PURE_LAST)
        {
            return this->m_pInput->GamepadPressed((PuRe_IInput::EGamepad)a_Button, a_Index);
        }
        switch (a_Button)
        {
        default:
            //Nothing added yet
            return false;
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetKeyboardButtonIsPressed(Input::EKeyboardButton::Type a_Button)
    {
        if (a_Button <= Input::EKeyboardButton::PURE_LAST)
        {
            return this->m_pInput->KeyIsPressed((PuRe_IInput::EKeys)a_Button);
        }
        switch (a_Button)
        {
        case Input::EKeyboardButton::Delete:
            return this->m_pInput->KeyIsPressed(this->m_pInput->Backspace);
        case Input::EKeyboardButton::CtrlZ:
            return this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Z);
        case Input::EKeyboardButton::CtrlY:
            return this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Y);
        case Input::EKeyboardButton::CtrlS:
            return this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->S);
        case Input::EKeyboardButton::MouseLeft:
            return this->m_pInput->MouseIsPressed(this->m_pInput->LeftClick);
        case Input::EKeyboardButton::MouseMiddle:
            return this->m_pInput->MouseIsPressed(this->m_pInput->MiddleClick);
        case Input::EKeyboardButton::MouseRight:
            return this->m_pInput->MouseIsPressed(this->m_pInput->RightClick);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetGamepadButtonIsPressed(Input::EGamepadButton::Type a_Button, int a_Index)
    {
        if (a_Button <= Input::EGamepadButton::PURE_LAST)
        {
            return this->m_pInput->GamepadIsPressed((PuRe_IInput::EGamepad)a_Button, a_Index);
        }
        switch (a_Button)
        {
        default:
            //Nothing added yet
            return false;
        }
    }
}