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
        //send that we are connecting to him
        HeadPacket h;
        h.Type = 0;
        this->Send((char*)&h, sizeof(HeadPacket), this->m_pSocket->GetAddress());
    }


    // **************************************************************************
    // **************************************************************************
    long CNetworkHandler::Receive(char* a_pBuffer, int a_Size, SOCKADDR_IN* a_pSender)
    {
        return this->m_pSocket->Receive(a_pBuffer, a_Size, a_pSender);
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::SendHost(char* a_pBuffer, int a_Size)
    {
        if (this->m_pSocket != NULL)
            this->Send(a_pBuffer, a_Size, this->m_pSocket->GetAddress());
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Send(char* a_pBuffer, int a_Size, SOCKADDR_IN a_Receiver)
    {
        if (this->m_pSocket != NULL)
            this->m_pSocket->Send(a_Receiver,a_pBuffer,a_Size);
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