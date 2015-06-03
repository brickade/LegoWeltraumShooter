#include "include/InputManager.h"

namespace SpacebrickArena
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
        this->m_pDirectionMapping[EDirection::EditorCameraRotate] =
        {
            EGamepadDirection::RightThumb,
            {
                EKeyboardDirection::ArrowKeys,
                EKeyboardDirection::ArrowKeys,
                EKeyboardDirection::ArrowKeys,
                EKeyboardDirection::ArrowKeys
            }
        };
        this->m_pDirectionMapping[EDirection::EditorMoveBrick] =
        {
            EGamepadDirection::LeftThumb,
            {
                EKeyboardDirection::WSAD,
                EKeyboardDirection::WSAD,
                EKeyboardDirection::WSAD,
                EKeyboardDirection::WSAD
            }
        };
        this->m_pDirectionMapping[EDirection::EditorNavigateCategory] =
        {
            EGamepadDirection::DPad,
            {
                EKeyboardDirection::IKJL,
                EKeyboardDirection::IKJL,
                EKeyboardDirection::IKJL,
                EKeyboardDirection::IKJL
            }
        };

        //Axis
        this->m_pAxisMapping[EAxis::EditorCameraZoom] =
        {
            EGamepadAxis::BothTrigger,
            {
                EKeyboardAxis::RF,
                EKeyboardAxis::RF,
                EKeyboardAxis::RF,
                EKeyboardAxis::RF
            }
        };
        this->m_pAxisMapping[EAxis::EditorColorSlider] =
        {
            EGamepadAxis::BothTrigger,
            {
                EKeyboardAxis::RF,
                EKeyboardAxis::RF,
                EKeyboardAxis::RF,
                EKeyboardAxis::RF
            }
        };

        //Button
        this->m_pButtonMapping[EButton::EditorPlaceBrick] =
        {
            EGamepadButton::Pad_A,
            {
                EKeyboardButton::Enter,
                EKeyboardButton::Enter,
                EKeyboardButton::Enter,
                EKeyboardButton::Enter
            }
        };
        this->m_pButtonMapping[EButton::EditorTogglePlacementSide] =
        {
            EGamepadButton::Pad_B,
            {
                EKeyboardButton::Space,
                EKeyboardButton::Space,
                EKeyboardButton::Space,
                EKeyboardButton::Space
            }
        };
        this->m_pButtonMapping[EButton::EditorUndo] =
        {
            EGamepadButton::Pad_X,
            {
                EKeyboardButton::CtrlZ,
                EKeyboardButton::CtrlZ,
                EKeyboardButton::CtrlZ,
                EKeyboardButton::CtrlZ
            }
        };
        this->m_pButtonMapping[EButton::EditorRedo] =
        {
            EGamepadButton::Pad_Y,
            {
                EKeyboardButton::CtrlY,
                EKeyboardButton::CtrlY,
                EKeyboardButton::CtrlY,
                EKeyboardButton::CtrlY
            }
        };
        this->m_pButtonMapping[EButton::EditorRotateBrickRight] =
        {
            EGamepadButton::Right_Shoulder,
            {
                EKeyboardButton::E,
                EKeyboardButton::E,
                EKeyboardButton::E,
                EKeyboardButton::E
            }
        };
        this->m_pButtonMapping[EButton::EditorRotateBrickLeft] =
        {
            EGamepadButton::Left_Shoulder,
            {
                EKeyboardButton::Q,
                EKeyboardButton::Q,
                EKeyboardButton::Q,
                EKeyboardButton::Q
            }
        };
        this->m_pButtonMapping[EButton::EditorResetShip] =
        {
            EGamepadButton::Pad_Start,
            {
                EKeyboardButton::Delete,
                EKeyboardButton::Delete,
                EKeyboardButton::Delete,
                EKeyboardButton::Delete
            }
        };
        this->m_pButtonMapping[EButton::EditorSaveShip] =
        {
            EGamepadButton::Pad_Back,
            {
                EKeyboardButton::CtrlS,
                EKeyboardButton::CtrlS,
                EKeyboardButton::CtrlS,
                EKeyboardButton::CtrlS
            }
        };
        this->m_pButtonMapping[EButton::EditorToggleNavigateCategories] =
        {
            EGamepadButton::Right_Thumb,
            {
                EKeyboardButton::X,
                EKeyboardButton::X,
                EKeyboardButton::X,
                EKeyboardButton::X
            }
        };
        this->m_pButtonMapping[EButton::Exit] =
        {
            EGamepadButton::Pad_Back,
            {
                EKeyboardButton::Escape,
                EKeyboardButton::Escape,
                EKeyboardButton::Escape,
                EKeyboardButton::Escape
            }
        };
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::Direction(EDirection::Type a_Direction, int a_PlayerIndex)
    {
        SDirectionMapping& mapping = this->m_pDirectionMapping[a_Direction];
        return this->GetGamepadDirection(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardDirection(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::Axis(EAxis::Type a_Axis, int a_PlayerIndex)
    {
        SAxisMapping& mapping = this->m_pAxisMapping[a_Axis];
        return this->GetGamepadAxis(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardAxis(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonPressed(EButton::Type a_Button, int a_PlayerIndex)
    {
        SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        return this->GetGamepadButtonPressed(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardButtonPressed(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonIsPressed(EButton::Type a_Button, int a_PlayerIndex)
    {
        SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        return this->GetGamepadButtonIsPressed(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardButtonIsPressed(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::GetKeyboardDirection(EKeyboardDirection::Type a_Direction)
    {
        switch (a_Direction)
        {
        case SpacebrickArena::CInputManager::EKeyboardDirection::ArrowKeys:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->Up, this->m_pInput->Down, this->m_pInput->Right, this->m_pInput->Left);
        case SpacebrickArena::CInputManager::EKeyboardDirection::WSAD:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->W, this->m_pInput->S, this->m_pInput->D, this->m_pInput->A);
        case SpacebrickArena::CInputManager::EKeyboardDirection::IKJL:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->I, this->m_pInput->K, this->m_pInput->L, this->m_pInput->J);
        case SpacebrickArena::CInputManager::EKeyboardDirection::Numpad:
            return this->CalcKeyboardDirectionFromKeys(this->m_pInput->Eight, this->m_pInput->Five, this->m_pInput->Six, this->m_pInput->Four);
        case SpacebrickArena::CInputManager::EKeyboardDirection::Mouse:
            return this->m_pInput->GetRelativeMousePosition();
        }
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::GetGamepadDirection(EGamepadDirection::Type a_Direction, int a_Index)
    {
        switch (a_Direction)
        {
        case SpacebrickArena::CInputManager::EGamepadDirection::LeftThumb:
            return this->m_pInput->GetGamepadLeftThumb(a_Index);
        case SpacebrickArena::CInputManager::EGamepadDirection::RightThumb:
            return this->m_pInput->GetGamepadRightThumb(a_Index);
        case SpacebrickArena::CInputManager::EGamepadDirection::DPad:
            return this->CalcGamepadDirectionFromButtons(this->m_pInput->DPAD_Up, this->m_pInput->DPAD_Down, this->m_pInput->DPAD_Right, this->m_pInput->DPAD_Left, a_Index);
        case SpacebrickArena::CInputManager::EGamepadDirection::Buttons:
            return this->CalcGamepadDirectionFromButtons(this->m_pInput->Pad_Y, this->m_pInput->Pad_A, this->m_pInput->Pad_B, this->m_pInput->Pad_X, a_Index);
        }
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::GetKeyboardAxis(EKeyboardAxis::Type a_Axis)
    {
        switch (a_Axis)
        {
        case SpacebrickArena::CInputManager::EKeyboardAxis::WS:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->W, this->m_pInput->S);
        case SpacebrickArena::CInputManager::EKeyboardAxis::AD:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->D, this->m_pInput->A);
        case SpacebrickArena::CInputManager::EKeyboardAxis::RF:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->R, this->m_pInput->F);
        case SpacebrickArena::CInputManager::EKeyboardAxis::QE:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->E, this->m_pInput->Q);
        case SpacebrickArena::CInputManager::EKeyboardAxis::ArrowUD:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->Up, this->m_pInput->Down);
        case SpacebrickArena::CInputManager::EKeyboardAxis::ArrowLR:
            return this->CalcKeyboardAxisFromKeys(this->m_pInput->Right, this->m_pInput->Left);
        case SpacebrickArena::CInputManager::EKeyboardAxis::MouseX:
            return this->m_pInput->GetRelativeMousePosition().X;
        case SpacebrickArena::CInputManager::EKeyboardAxis::MouseY:
            return this->m_pInput->GetRelativeMousePosition().Y;
        case SpacebrickArena::CInputManager::EKeyboardAxis::MouseScroll:
            return this->m_pInput->GetMouseScroll();
        }
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::GetGamepadAxis(EGamepadAxis::Type a_Axis, int a_Index)
    {
        switch (a_Axis)
        {
        case SpacebrickArena::CInputManager::EGamepadAxis::LeftTrigger:
            return this->m_pInput->GetGamepadLeftTrigger(a_Index);
        case SpacebrickArena::CInputManager::EGamepadAxis::RightTrigger:
            return this->m_pInput->GetGamepadRightTrigger(a_Index);
        case SpacebrickArena::CInputManager::EGamepadAxis::BothTrigger:
            return this->m_pInput->GetGamepadRightTrigger(a_Index) - this->m_pInput->GetGamepadLeftTrigger(a_Index);
        case SpacebrickArena::CInputManager::EGamepadAxis::LeftThumbX:
            return this->m_pInput->GetGamepadLeftThumb(a_Index).X;
        case SpacebrickArena::CInputManager::EGamepadAxis::LeftThumbY:
            return this->m_pInput->GetGamepadLeftThumb(a_Index).Y;
        case SpacebrickArena::CInputManager::EGamepadAxis::RightThumbX:
            return this->m_pInput->GetGamepadRightThumb(a_Index).X;
        case SpacebrickArena::CInputManager::EGamepadAxis::RightThumbY:
            return this->m_pInput->GetGamepadRightThumb(a_Index).Y;
        case SpacebrickArena::CInputManager::EGamepadAxis::DPadX:
            return this->CalcGamepadAxisFromButtons(this->m_pInput->DPAD_Right, this->m_pInput->DPAD_Left, a_Index);
        case SpacebrickArena::CInputManager::EGamepadAxis::DPadY:
            return this->CalcGamepadAxisFromButtons(this->m_pInput->DPAD_Up, this->m_pInput->DPAD_Down, a_Index);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetKeyboardButtonPressed(EKeyboardButton::Type a_Button)
    {
        if (a_Button <= EKeyboardButton::PURE_LAST)
        {
            return this->m_pInput->KeyPressed((PuRe_IInput::EKeys)a_Button);
        }
        switch (a_Button)
        {
        case SpacebrickArena::CInputManager::EKeyboardButton::Delete:
            return this->m_pInput->KeyPressed(this->m_pInput->Backspace);
        case SpacebrickArena::CInputManager::EKeyboardButton::CtrlZ:
            return this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Z);
        case SpacebrickArena::CInputManager::EKeyboardButton::CtrlY:
            return this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Y);
        case SpacebrickArena::CInputManager::EKeyboardButton::CtrlS:
            return this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->S);
        case SpacebrickArena::CInputManager::EKeyboardButton::MouseLeft:
            return this->m_pInput->MousePressed(this->m_pInput->LeftClick);
        case SpacebrickArena::CInputManager::EKeyboardButton::MouseMiddle:
            return this->m_pInput->MousePressed(this->m_pInput->MiddleClick);
        case SpacebrickArena::CInputManager::EKeyboardButton::MouseRight:
            return this->m_pInput->MousePressed(this->m_pInput->RightClick);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetGamepadButtonPressed(EGamepadButton::Type a_Button, int a_Index)
    {
        if (a_Button <= EGamepadButton::PURE_LAST)
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
    bool CInputManager::GetKeyboardButtonIsPressed(EKeyboardButton::Type a_Button)
    {
        if (a_Button <= EKeyboardButton::PURE_LAST)
        {
            return this->m_pInput->KeyIsPressed((PuRe_IInput::EKeys)a_Button);
        }
        switch (a_Button)
        {
        case SpacebrickArena::CInputManager::EKeyboardButton::Delete:
            return this->m_pInput->KeyIsPressed(this->m_pInput->Backspace);
        case SpacebrickArena::CInputManager::EKeyboardButton::CtrlZ:
            return this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Z);
        case SpacebrickArena::CInputManager::EKeyboardButton::CtrlY:
            return this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Y);
        case SpacebrickArena::CInputManager::EKeyboardButton::CtrlS:
            return this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->S);
        case SpacebrickArena::CInputManager::EKeyboardButton::MouseLeft:
            return this->m_pInput->MouseIsPressed(this->m_pInput->LeftClick);
        case SpacebrickArena::CInputManager::EKeyboardButton::MouseMiddle:
            return this->m_pInput->MouseIsPressed(this->m_pInput->MiddleClick);
        case SpacebrickArena::CInputManager::EKeyboardButton::MouseRight:
            return this->m_pInput->MouseIsPressed(this->m_pInput->RightClick);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetGamepadButtonIsPressed(EGamepadButton::Type a_Button, int a_Index)
    {
        if (a_Button <= EGamepadButton::PURE_LAST)
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