#include "include/NetworkHandler.h"

namespace Game
{
    CNetworkHandler::CNetworkHandler()
    {
    }


    // **************************************************************************
    // **************************************************************************
    CNetworkHandler::~CNetworkHandler()
    {
        SAFE_DELETE(this->m_pSocket);
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Connect()
    {
        this->m_pSocket = new PuRe_Socket(this->m_IP.c_str(), std::stoi(this->m_Port), this->m_Host);
    }


    // **************************************************************************
    // **************************************************************************
    int CNetworkHandler::GetState()
    {
        return this->m_NetworkState;
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Update(PuRe_IInput* a_pInput)
    {
        if (this->m_NetworkState == 0)
        {
            if (a_pInput->KeyPressed(a_pInput->Zero))
            {
                this->m_Host = true;
                this->m_IP = "127.0.0.1";
                this->m_NetworkState = 2;
            }
            if (a_pInput->KeyPressed(a_pInput->One))
            {
                this->m_Host = false;
                this->m_NetworkState = 1;
            }
        }
        else if (this->m_NetworkState != 3)
        {
            std::string* text = nullptr;
            if (this->m_NetworkState == 1)
                text = &this->m_IP;
            else if (this->m_NetworkState == 2)
                text = &this->m_Port;

            if (a_pInput->KeyPressed(a_pInput->Zero))
                *text += "0";
            if (a_pInput->KeyPressed(a_pInput->One))
                *text += "1";
            if (a_pInput->KeyPressed(a_pInput->Two))
                *text += "2";
            if (a_pInput->KeyPressed(a_pInput->Three))
                *text += "3";
            if (a_pInput->KeyPressed(a_pInput->Four))
                *text += "4";
            if (a_pInput->KeyPressed(a_pInput->Five))
                *text += "5";
            if (a_pInput->KeyPressed(a_pInput->Six))
                *text += "6";
            if (a_pInput->KeyPressed(a_pInput->Seven))
                *text += "7";
            if (a_pInput->KeyPressed(a_pInput->Eight))
                *text += "8";
            if (a_pInput->KeyPressed(a_pInput->Nine))
                *text += "9";
            if (a_pInput->KeyPressed(a_pInput->Backspace) && text->length() != 0)
                *text = text->substr(0, text->length() - 1);
            if (this->m_NetworkState == 1 && a_pInput->KeyPressed(a_pInput->Period))
                *text += ".";
            if (a_pInput->KeyPressed(a_pInput->Enter))
            {
                this->m_NetworkState++;
            }
        }
    }
}