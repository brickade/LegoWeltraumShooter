#include "include/Navigation.h"

namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CNavigation::CNavigation(int a_ElementsPerLine, int a_LastElement, float a_ScrollingSpeed, float a_ScrollingThreshold, float a_InputThreshold)
    {
        this->m_ElementsPerLine = a_ElementsPerLine;
        this->m_LastElement = a_LastElement;
        this->m_FocusedElement = 0;
        this->m_FocusPosition = PuRe_Vector2F::Zero();
        this->m_PreviousState = EDirection::None;
        this->m_InputThreshold = a_InputThreshold;
        this->m_ScrollingThreshold = a_ScrollingThreshold;
        this->m_ScrollingSpeed = a_ScrollingSpeed;
        this->m_pTimer = new TheBrick::CBrickTimer();
    }

    // **************************************************************************
    // **************************************************************************
    CNavigation::~CNavigation()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::Update(PuRe_Timer* a_pTimer, PuRe_Vector2F a_InputVector)
    {
        EDirection::Type a_NewDirection = this->DirectionFromInputVector(a_InputVector);
        if (a_NewDirection != this->m_PreviousState)
        {
            if (a_NewDirection != EDirection::None)
            {
                this->Navigate(a_NewDirection); //Navigate before scrolling
            }
            this->m_FocusPosition = PuRe_Vector2F::Zero(); //Stop/reset scrolling
            this->m_pTimer->Reset();
        }
        else if (a_NewDirection != EDirection::None && this->m_pTimer->GetTotalElapsedSeconds() > this->m_ScrollingThreshold) //Scrolling
        {
            this->Scroll(a_InputVector, a_pTimer->GetTotalElapsedSeconds());
        }
        this->m_PreviousState = a_NewDirection;
    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::Navigate(EDirection::Type a_Direction)
    {
        switch (a_Direction)
        {
        case EDirection::Right:
            this->m_FocusedElement++;
            break;
        case EDirection::Left:
            this->m_FocusedElement--;
            break;
        case EDirection::Up:
            this->m_FocusedElement -= this->m_ElementsPerLine;
            break;
        case EDirection::Down:
            this->m_FocusedElement += this->m_ElementsPerLine;
            if (floor(this->m_FocusedElement / (float)this->m_ElementsPerLine) == floor(this->m_LastElement / (float)this->m_ElementsPerLine))
            { //Bei Nav in Leerposition der letzten Reihe in letztes Element der letzten Reihe springen
                this->m_FocusedElement = this->m_LastElement;
            }
            break;
        }
        this->ClampFocus(true);
    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::Scroll(PuRe_Vector2F a_Input, float a_DeltaSec)
    {
        this->m_FocusPosition += a_Input * a_DeltaSec * this->m_ScrollingSpeed;
        if (this->m_FocusPosition.X > 100)
        {
            this->Navigate(EDirection::Right);
            this->m_FocusPosition.X -= 100;
        }
        else if (this->m_FocusPosition.X < -100)
        {
            this->Navigate(EDirection::Left);
            this->m_FocusPosition.X += 100;
        }
        else if (this->m_FocusPosition.Y > 100)
        {
            this->Navigate(EDirection::Up);
            this->m_FocusPosition.Y -= 100;
        }
        else if (this->m_FocusPosition.Y < -100)
        {
            this->Navigate(EDirection::Down);
            this->m_FocusPosition.Y += 100;
        }
    }

    // **************************************************************************
    // **************************************************************************
    EDirection::Type CNavigation::DirectionFromInputVector(PuRe_Vector2F a_InputVector) const
    {
        EDirection::Type state = EDirection::None;
        if (a_InputVector.Length() < this->m_InputThreshold)
        {
            return state;
        }
        a_InputVector.Normalize();
        float limit = sin(45 * 0.0174532925f);
        if (a_InputVector.X > limit)
        {
            state = EDirection::Right;
        }
        else if (a_InputVector.X < -limit)
        {
            state = EDirection::Left;
        }
        else if (a_InputVector.Y > limit)
        {
            state = EDirection::Up;
        }
        else if (a_InputVector.Y < -limit)
        {
            state = EDirection::Down;
        }
        return state;
    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::ClampFocus(bool a_LinkEnds)
    {
        if (this->m_FocusedElement > this->m_LastElement) //Over End
        {
            if (a_LinkEnds) //Jump
            {
                this->m_FocusedElement = 0;
            }
            else //Clamp
            {
                this->m_FocusedElement = this->m_LastElement;
            }
        }
        else if (this->m_FocusedElement < 0) //Over Start
        {
            if (a_LinkEnds) //Jump
            {
                this->m_FocusedElement = this->m_LastElement;
            }
            else //Clamp
            {
                this->m_FocusedElement = 0;
            }
        }
    }

    void CNavigation::AddElement(int a_Position)
    {
        this->m_LastElement++;
        if (a_Position != -1 && a_Position < this->m_FocusedElement)
        {
            this->m_FocusedElement++;
        }
    }

    void CNavigation::RemoveElement(int a_Position)
    {
        this->m_LastElement--;
        if (a_Position != -1 && a_Position < this->m_FocusedElement)
        {
            this->m_FocusedElement--;
        }
        this->ClampFocus();
    }

    void CNavigation::AddLine(int a_Position)
    {
        this->m_LastElement += this->m_ElementsPerLine;
        if (a_Position != -1 && a_Position < this->m_FocusedElement)
        {
            this->m_FocusedElement += this->m_ElementsPerLine;
        }
    }

    void CNavigation::RemoveLine(int a_Position)
    {
        this->m_LastElement -= this->m_ElementsPerLine;
        if (a_Position != -1 && a_Position < this->m_FocusedElement)
        {
            this->m_FocusedElement -= this->m_ElementsPerLine;
        }
        this->ClampFocus();
    }
}