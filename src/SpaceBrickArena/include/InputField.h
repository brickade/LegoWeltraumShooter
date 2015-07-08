#ifndef _INPUTFIELD_H_
#define _INPUTFIELD_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/BrickTimer.h>

namespace sba
{
    class CInputField
    {
    public:
        struct InputTypes
        {
            enum Type
            {
                NUMBERS = 1 << 0,
                LETTERS_UPPERCASE = 1 << 1,
                LETTERS_LOWERCASE = 1 << 2,
                PERIOD = 1 << 3,
                UNDERLINE = 1 << 4
            };
        };

    private:
        std::string m_Value;
        std::string m_OldValue;
        int m_MaxLength;
        InputTypes::Type m_AllowedInputs;

    public:
        const std::string& GetValue() const
        {
            return this->m_Value;
        }

        void SetValue(const std::string& a_rValue)
        {
            this->m_Value = a_rValue;
            this->m_OldValue = this->m_Value;
        }

    public:
        CInputField(InputTypes::Type a_AllowedInputs = (InputTypes::Type)(InputTypes::NUMBERS | InputTypes::LETTERS_UPPERCASE | InputTypes::LETTERS_UPPERCASE), std::string a_InitValue = "", int a_MaxLength = 14);
        ~CInputField();

        bool Update(PuRe_IInput& a_rInput, int a_PlayerIndex); //Returns if the inputfield was closed in this update via Enter/Esc
    };
}

#endif /* _INPUTFIELD_H_ */