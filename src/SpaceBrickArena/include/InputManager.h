#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <PuReEngine/IInput.h>
#include <unordered_map>

namespace SpacebrickArena
{
    class CInputManager
    {
        // **************************************************************************
        // Inputs
        // **************************************************************************
        struct EGamepadDirection
        {
            enum Type
            {
                LeftThumb,
                RightThumb,
                DPad,
                Buttons,
                LAST = Buttons
            };
        };

        struct EKeyboardDirection
        {
            enum Type
            {
                ArrowKeys,
                WSAD,
                IKJL,
                Numpad,
                Mouse,
                LAST = Mouse
            };
        };

        struct EGamepadAxis
        {
            enum Type
            {
                LeftTrigger,
                RightTrigger,
                BothTrigger,
                LeftThumbX,
                LeftThumbY,
                RightThumbX,
                RightThumbY,
                DPadX,
                DPadY,
                LAST = DPadY
            };
        };

        struct EKeyboardAxis
        {
            enum Type
            {
                WS,
                AD,
                RF,
                QE,
                ArrowUD,
                ArrowLR,
                MouseX,
                MouseY,
                MouseScroll,
                LAST = MouseScroll
            };
        };

        struct EGamepadButton
        {
            enum Type
            {
                //PuRe_Input
                Pad_A, Pad_B, Pad_X, Pad_Y, DPAD_Left, DPAD_Right, DPAD_Up, DPAD_Down, Left_Shoulder, Right_Shoulder, Pad_Back, Pad_Start, Left_Thumb, Right_Thumb,
                PURE_LAST = Right_Thumb,
                //Own
                LAST = PURE_LAST
            };
        };

        struct EKeyboardButton
        {
            enum Type
            {
                //PuRe_Input
                Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
                A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
                Enter, Backspace, Space, Ctrl, Alt, Shift, Escape,
                F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
                Left, Up, Right, Down, Period, Comma, Minus,
                PURE_LAST = Minus,
                //Own
                Delete, CtrlZ, CtrlY, CtrlS,
                MouseLeft, MouseMiddle, MouseRight,
                LAST = MouseRight
            };
        };

        // **************************************************************************
        // Keys
        // **************************************************************************
        struct EDirection
        {
            enum Type
            {
                EditorCameraRotate,
                EditorMoveBrick,
                EditorNavigateCategory,
                LAST = EditorNavigateCategory
            };
        };

        struct EAxis
        {
            enum Type
            {
                EditorCameraZoom,
                EditorColorSlider,
                LAST = EditorColorSlider
            };
        };

        struct EButton
        {
            enum Type
            {
                EditorPlaceBrick,
                EditorTogglePlacementSide,
                EditorUndo,
                EditorRedo,
                EditorRotateBrickRight,
                EditorRotateBrickLeft,
                EditorResetShip,
                EditorSaveShip,
                EditorToggleNavigateCategories,
                Exit,
                LAST = Exit
            };
        };

        // **************************************************************************
        // Values (Mappings)
        // **************************************************************************

        struct SDirectionMapping
        {
            EGamepadDirection::Type Gamepad;
            EKeyboardDirection::Type Keyboard[4];
        };

        struct SAxisMapping
        {
            EGamepadAxis::Type Gamepad;
            EKeyboardAxis::Type Keyboard[4];
        };

        struct SButtonMapping
        {
            EGamepadButton::Type Gamepad;
            EKeyboardButton::Type Keyboard[4];
        };

        // **************************************************************************
        // Variables
        // **************************************************************************
    public:

    private:
        PuRe_IInput* m_pInput;

        SDirectionMapping m_pDirectionMapping[EDirection::LAST];
        SAxisMapping m_pAxisMapping[EAxis::LAST];
        SButtonMapping m_pButtonMapping[EButton::LAST];

        // **************************************************************************
        // Functions
        // **************************************************************************
    public:
        CInputManager();
        ~CInputManager();

        void Initialize(PuRe_IInput* a_pInput);

        PuRe_Vector2F Direction(EDirection::Type a_Direction, int a_PlayerIndex);

        float Axis(EAxis::Type a_Axis, int a_PlayerIndex);

        bool ButtonPressed(EButton::Type a_Button, int a_PlayerIndex);
        bool ButtonIsPressed(EButton::Type a_Button, int a_PlayerIndex);

    private:
        //Maybe later test whether it is more performant to replace the switch cases with function pointer arrays (the compiler might be already doing this with the switch cases)

        PuRe_Vector2F GetKeyboardDirection(EKeyboardDirection::Type a_Direction);
        PuRe_Vector2F GetGamepadDirection(EGamepadDirection::Type a_Direction, int a_Index);

        float GetKeyboardAxis(EKeyboardAxis::Type a_Axis);
        float GetGamepadAxis(EGamepadAxis::Type a_Axis, int a_Index);

        bool GetKeyboardButtonPressed(EKeyboardButton::Type a_Button);
        bool GetGamepadButtonPressed(EGamepadButton::Type a_Button, int a_Index);

        bool GetKeyboardButtonIsPressed(EKeyboardButton::Type a_Button);
        bool GetGamepadButtonIsPressed(EGamepadButton::Type a_Button, int a_Index);

        // **************************************************************************
        // Helper Functions
        // **************************************************************************
        //Direction
        PuRe_Vector2F CalcKeyboardDirectionFromKeys(PuRe_IInput::EKeys a_KeyUp, PuRe_IInput::EKeys a_KeyDown, PuRe_IInput::EKeys a_KeyRight, PuRe_IInput::EKeys a_KeyLeft)
        {
            PuRe_Vector2F result = PuRe_Vector2F::Zero();
            if (this->m_pInput->KeyPressed(a_KeyUp)) result.Y++;
            if (this->m_pInput->KeyPressed(a_KeyDown)) result.Y--;
            if (this->m_pInput->KeyPressed(a_KeyRight)) result.X++;
            if (this->m_pInput->KeyPressed(a_KeyLeft)) result.X--;
            return result;
        }

        PuRe_Vector2F CalcGamepadDirectionFromButtons(PuRe_IInput::EGamepad a_ButtonUp, PuRe_IInput::EGamepad a_ButtonDown, PuRe_IInput::EGamepad a_ButtonRight, PuRe_IInput::EGamepad a_ButtonLeft, int a_Index)
        {
            PuRe_Vector2F result = PuRe_Vector2F::Zero();
            if (this->m_pInput->GamepadPressed(a_ButtonUp, a_Index)) result.Y++;
            if (this->m_pInput->GamepadPressed(a_ButtonDown, a_Index)) result.Y--;
            if (this->m_pInput->GamepadPressed(a_ButtonRight, a_Index)) result.X++;
            if (this->m_pInput->GamepadPressed(a_ButtonLeft, a_Index)) result.X--;
            return result;
        }

        //Axis
        float CalcKeyboardAxisFromKeys(PuRe_IInput::EKeys a_KeyPositive, PuRe_IInput::EKeys a_KeyNegative)
        {
            float result = 0;
            if (this->m_pInput->KeyPressed(a_KeyPositive)) result++;
            if (this->m_pInput->KeyPressed(a_KeyNegative)) result--;
            return result;
        }

        float CalcGamepadAxisFromButtons(PuRe_IInput::EGamepad a_ButtonPositive, PuRe_IInput::EGamepad a_ButtonNegative, int a_Index)
        {
            float result = 0;
            if (this->m_pInput->GamepadPressed(a_ButtonPositive, a_Index)) result++;
            if (this->m_pInput->GamepadPressed(a_ButtonNegative, a_Index)) result--;
            return result;
        }

        //Pressed w hold
        bool CalcKeyboardKeyPressedWithHoldKey(PuRe_IInput::EKeys a_KeyHold, PuRe_IInput::EKeys a_KeyAdditional)
        {
            if (!this->m_pInput->KeyIsPressed(a_KeyHold))
            {
                return false;
            }
            if (this->m_pInput->KeyPressed(a_KeyAdditional))
            {
                return true;
            }
            return false;
        }


        bool CalcGamepadButtonPressedWithHoldButton(PuRe_IInput::EGamepad a_ButtonHold, PuRe_IInput::EGamepad a_ButtonAdditional, int a_Index)
        {
            if (!this->m_pInput->GamepadIsPressed(a_ButtonHold, a_Index))
            {
                return false;
            }
            if (this->m_pInput->GamepadPressed(a_ButtonAdditional, a_Index))
            {
                return true;
            }
            return false;
        }

        //IsPressed w hold
        bool CalcKeyboardKeyIsPressedWithHoldKey(PuRe_IInput::EKeys a_KeyHold, PuRe_IInput::EKeys a_KeyAdditional)
        {
            if (!this->m_pInput->KeyIsPressed(a_KeyHold))
            {
                return false;
            }
            if (this->m_pInput->KeyIsPressed(a_KeyAdditional))
            {
                return true;
            }
            return false;
        }

        bool CalcGamepadButtonIsPressedWithHoldButton(PuRe_IInput::EGamepad a_ButtonHold, PuRe_IInput::EGamepad a_ButtonAdditional, int a_Index)
        {
            if (!this->m_pInput->GamepadIsPressed(a_ButtonHold, a_Index))
            {
                return false;
            }
            if (this->m_pInput->GamepadIsPressed(a_ButtonAdditional, a_Index))
            {
                return true;
            }
            return false;
        }


    };
}

#endif /* _INPUTMANAGER_H_ */