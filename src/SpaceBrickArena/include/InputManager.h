#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "InputManagerTypes.h"

#include <PuReEngine/IInput.h>

namespace TheBrick{
    class CSerializer;
}

namespace sba
{
    class CInputManager
    {
    public:

    private:
        PuRe_IInput* m_pInput;

        static const float DEFAULT_GAMEPAD_THRESHOLD;

        Input::SDirectionMapping m_pDirectionMapping[Input::EDirection::LAST + 1];
        Input::SAxisMapping m_pAxisMapping[Input::EAxis::LAST + 1];
        Input::SButtonMapping m_pButtonMapping[Input::EButton::LAST + 1];

        int m_ConnectedGamepadsCount;
        bool m_LastInputWasGamepad;
        double m_LastGamepadDetection;

    public:
        Input::SDirectionMapping* GetDirectionMapping()
        {
            return this->m_pDirectionMapping;
        }

        Input::SAxisMapping* GetAxisMapping()
        {
            return this->m_pAxisMapping;
        }

        Input::SButtonMapping* GetButtonMapping()
        {
            return this->m_pButtonMapping;
        }

        bool LastInputWasGamepad() const
        {
            return this->m_LastInputWasGamepad;
        }

    public:
        CInputManager();
        ~CInputManager();

        void Initialize();
        void Reset();
        void Load(const char*a_pFile);
        void Save(const char* a_pFile);
        void Deserialize(TheBrick::CSerializer& a_pSerializer);
        void Serialize(TheBrick::CSerializer& a_pSerializer);
        void DetectGamepads();
        void TryDetectGamepads(PuRe_Timer& a_rTimer, float a_RefreshRatePerSecond = 1.0f);

        PuRe_Vector2F Direction(Input::EDirection::Type a_Direction, int a_PlayerIndex, float a_GamepadThreshold = DEFAULT_GAMEPAD_THRESHOLD);

        float Axis(Input::EAxis::Type a_Axis, int a_PlayerIndex, float a_GamepadThreshold = DEFAULT_GAMEPAD_THRESHOLD);

        bool ButtonPressed(Input::EButton::Type a_Button, int a_PlayerIndex);
        bool ButtonIsPressed(Input::EButton::Type a_Button, int a_PlayerIndex);

    private:
        //Maybe later test whether it is more performant to replace the switch cases with function pointer arrays (the compiler might be already doing this with the switch cases)

        PuRe_Vector2F GetKeyboardDirection(Input::EKeyboardDirection::Type a_Direction);
        PuRe_Vector2F GetGamepadDirection(Input::EGamepadDirection::Type a_Direction, int a_Index, float a_Threshold = DEFAULT_GAMEPAD_THRESHOLD);

        float GetKeyboardAxis(Input::EKeyboardAxis::Type a_Axis);
        float GetGamepadAxis(Input::EGamepadAxis::Type a_Axis, int a_Index, float a_Threshold = DEFAULT_GAMEPAD_THRESHOLD);

        bool GetKeyboardButtonPressed(Input::EKeyboardButton::Type a_Button);
        bool GetGamepadButtonPressed(Input::EGamepadButton::Type a_Button, int a_Index);

        bool GetKeyboardButtonIsPressed(Input::EKeyboardButton::Type a_Button);
        bool GetGamepadButtonIsPressed(Input::EGamepadButton::Type a_Button, int a_Index);


        //Direction
        PuRe_Vector2F CalcKeyboardDirectionFromKeys(PuRe_IInput::EKeys a_KeyUp, PuRe_IInput::EKeys a_KeyDown, PuRe_IInput::EKeys a_KeyRight, PuRe_IInput::EKeys a_KeyLeft)
        {
            PuRe_Vector2F result = PuRe_Vector2F::Zero();
            if (this->m_pInput->KeyIsPressed(a_KeyUp)) result.Y++;
            if (this->m_pInput->KeyIsPressed(a_KeyDown)) result.Y--;
            if (this->m_pInput->KeyIsPressed(a_KeyRight)) result.X++;
            if (this->m_pInput->KeyIsPressed(a_KeyLeft)) result.X--;
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
            if (this->m_pInput->KeyIsPressed(a_KeyPositive)) result++;
            if (this->m_pInput->KeyIsPressed(a_KeyNegative)) result--;
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

        PuRe_Vector2F CheckLastInputIsGamepad(PuRe_Vector2F a_GamepadInput)
        {
            if (a_GamepadInput.Length() > 0.001f)
            {
                this->m_LastInputWasGamepad = true;
            }
            return a_GamepadInput;
        }

        PuRe_Vector2F CheckLastInputIsKeyboard(PuRe_Vector2F a_KeyboardInput)
        {
            if (a_KeyboardInput.Length() > 0.001f)
            {
                this->m_LastInputWasGamepad = false;
            }
            return a_KeyboardInput;
        }

        float CheckLastInputIsGamepad(float a_GamepadInput)
        {
            if (a_GamepadInput > 0.0001f)
            {
                this->m_LastInputWasGamepad = true;
            }
            return a_GamepadInput;
        }

        float CheckLastInputIsKeyboard(float a_KeyboardInput)
        {
            if (a_KeyboardInput > 0.0001f)
            {
                this->m_LastInputWasGamepad = false;
            }
            return a_KeyboardInput;
        }

        bool CheckLastInputIsGamepad(bool a_GamepadInput)
        {
            if (a_GamepadInput)
            {
                this->m_LastInputWasGamepad = true;
            }
            return a_GamepadInput;
        }

        bool CheckLastInputIsKeyboard(bool a_KeyboardInput)
        {
            if (a_KeyboardInput)
            {
                this->m_LastInputWasGamepad = false;
            }
            return a_KeyboardInput;
        }

#define INPUTMANAGER_VALIDIDX(idx) (idx < this->m_ConnectedGamepadsCount + 2)
#define INPUTMANAGER_PASSTOGAMEPAD(idx) (idx < this->m_ConnectedGamepadsCount)
    };
}

#endif /* _INPUTMANAGER_H_ */