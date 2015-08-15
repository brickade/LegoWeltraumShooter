#include "include/InputField.h"

#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CInputField::CInputField(InputTypes::Type a_AllowedInputs, std::string a_InitValue, int a_MaxLength)
    {
        this->m_MaxLength = a_MaxLength;
        this->m_AllowedInputs = a_AllowedInputs;
        this->m_Value = a_InitValue;
        this->m_OldValue = this->m_Value;
    }

    // **************************************************************************
    // **************************************************************************
    CInputField::~CInputField()
    {

    }

    // **************************************************************************
    // **************************************************************************
    bool CInputField::Update(PuRe_IInput& a_rInput, int a_PlayerIndex)
    {
        //Delete character
        if ((sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIndex) || a_rInput.KeyPressed(PuRe_IInput::Backspace)) && this->m_Value.length() > 0)
        {
            this->m_Value.erase(this->m_Value.length() - 1);
        }

        //Add character
        if (this->m_Value.length() < this->m_MaxLength)
        {
            if ((this->m_AllowedInputs & InputTypes::NUMBERS) && !a_rInput.KeyIsPressed(a_rInput.Shift))
            {
                //Numbers
                if (a_rInput.KeyPressed(a_rInput.Zero) || a_rInput.KeyPressed(a_rInput.Num_Zero))
                {
                    this->m_Value += "0";
                }
                if (a_rInput.KeyPressed(a_rInput.One) || a_rInput.KeyPressed(a_rInput.Num_One))
                {
                    this->m_Value += "1";
                }
                if (a_rInput.KeyPressed(a_rInput.Two) || a_rInput.KeyPressed(a_rInput.Num_Two))
                {
                    this->m_Value += "2";
                }
                if (a_rInput.KeyPressed(a_rInput.Three) || a_rInput.KeyPressed(a_rInput.Num_Three))
                {
                    this->m_Value += "3";
                }
                if (a_rInput.KeyPressed(a_rInput.Four) || a_rInput.KeyPressed(a_rInput.Num_Four))
                {
                    this->m_Value += "4";
                }
                if (a_rInput.KeyPressed(a_rInput.Five) || a_rInput.KeyPressed(a_rInput.Num_Five))
                {
                    this->m_Value += "5";
                }
                if (a_rInput.KeyPressed(a_rInput.Six) || a_rInput.KeyPressed(a_rInput.Num_Six))
                {
                    this->m_Value += "6";
                }
                if (a_rInput.KeyPressed(a_rInput.Seven) || a_rInput.KeyPressed(a_rInput.Num_Seven))
                {
                    this->m_Value += "7";
                }
                if (a_rInput.KeyPressed(a_rInput.Eight) || a_rInput.KeyPressed(a_rInput.Num_Eight))
                {
                    this->m_Value += "8";
                }
                if (a_rInput.KeyPressed(a_rInput.Nine) || a_rInput.KeyPressed(a_rInput.Num_Nine))
                {
                    this->m_Value += "9";
                }
            }

            if ((this->m_AllowedInputs & InputTypes::LETTERS_UPPERCASE) || (this->m_AllowedInputs & InputTypes::LETTERS_LOWERCASE))
            {
                //A-Z
                for (int i = 0; i < 26; i++)
                {
                    if (a_rInput.KeyPressed((PuReEngine::Core::IInput::EKeys)(a_rInput.A + i)))
                    {
                        if (this->m_AllowedInputs & InputTypes::LETTERS_UPPERCASE && a_rInput.KeyIsPressed(a_rInput.Shift))
                        {
                            this->m_Value += (char)((int)'A' + i);
                        }
                        else if ((this->m_AllowedInputs & InputTypes::LETTERS_LOWERCASE))
                        {
                            this->m_Value += (char)((int)'a' + i);
                        }
                        
                    }
                }
            }

            if ((this->m_AllowedInputs & InputTypes::PERIOD) && a_rInput.KeyPressed(a_rInput.Period))
            {
                //Period
                this->m_Value += '.';
            }

            if ((this->m_AllowedInputs & InputTypes::UNDERLINE) && (a_rInput.KeyPressed(a_rInput.Minus) || a_rInput.KeyPressed(a_rInput.Space)))
            {
                //Underline
                this->m_Value += '_'; //Who needs minus or space when you can map all to underline?!? muhahahahahahaha.
            }
        }

        //Close InputField
        if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIndex))
        {
            this->m_OldValue = this->m_Value;
            return true;
        }
        if (sba_Input->ButtonPressed(sba_Button::Exit, a_PlayerIndex))
        {
            this->m_Value = this->m_OldValue;
            return true;
        }
        return false;
    }
}




