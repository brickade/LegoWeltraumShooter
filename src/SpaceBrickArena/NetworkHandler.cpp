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
        this->m_pSocket = new PuRe_Socket(this->m_IP.c_str(), std::stoi(this->m_Port), this->m_Host,PuRe_Protocol::TCP);
        sba::CIniReader::Instance()->SetValue("IP", this->m_IP);
        sba::CIniReader::Instance()->SetValue("Port", this->m_Port);
    }

    // **************************************************************************
    // **************************************************************************
    SOCKET CNetworkHandler::Accept()
    {
        return this->m_pSocket->Accept(nullptr);
    }

    // **************************************************************************
    // **************************************************************************
    int CNetworkHandler::GetSocket()
    {
        return this->m_pSocket->GetSocket();
    }

    // **************************************************************************
    // **************************************************************************
    bool CNetworkHandler::Listen()
    {
        return this->m_pSocket->Listen();
    }


    // **************************************************************************
    // **************************************************************************
    long CNetworkHandler::Receive(char* a_pBuffer, int a_Size, SOCKET a_pSender)
    {
        long error = this->m_pSocket->Receive(a_pSender, a_pBuffer, a_Size);
        printf("Received Data with result %d\n", error);
        return error;
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::SendHost(char* a_pBuffer, int a_Size)
    {
        if (this->m_pSocket != NULL)
            this->Send(a_pBuffer, a_Size,this->m_pSocket->GetSocket());
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Send(char* a_pBuffer, int a_Size, SOCKET a_Receiver)
    {
        if (this->m_pSocket != NULL)
        {
            long error = this->m_pSocket->Send(a_Receiver,a_pBuffer,a_Size);
            printf("Send Data with result %d\n",error);
        }
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
                this->m_Port = sba::CIniReader::Instance()->GetValue("Port");
                this->m_NetworkState = 2;
            }
            if (a_pInput->KeyPressed(a_pInput->One))
            {
                this->m_IP = sba::CIniReader::Instance()->GetValue("IP");
                this->m_Port = sba::CIniReader::Instance()->GetValue("Port");
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

            if (a_pInput->KeyPressed(a_pInput->Zero)||a_pInput->KeyPressed(a_pInput->Num_Zero))
                *text += "0";
            if (a_pInput->KeyPressed(a_pInput->One) || a_pInput->KeyPressed(a_pInput->Num_One))
                *text += "1";
            if (a_pInput->KeyPressed(a_pInput->Two) || a_pInput->KeyPressed(a_pInput->Num_Two))
                *text += "2";
            if (a_pInput->KeyPressed(a_pInput->Three) || a_pInput->KeyPressed(a_pInput->Num_Three))
                *text += "3";
            if (a_pInput->KeyPressed(a_pInput->Four) || a_pInput->KeyPressed(a_pInput->Num_Four))
                *text += "4";
            if (a_pInput->KeyPressed(a_pInput->Five) || a_pInput->KeyPressed(a_pInput->Num_Five))
                *text += "5";
            if (a_pInput->KeyPressed(a_pInput->Six) || a_pInput->KeyPressed(a_pInput->Num_Six))
                *text += "6";
            if (a_pInput->KeyPressed(a_pInput->Seven) || a_pInput->KeyPressed(a_pInput->Num_Seven))
                *text += "7";
            if (a_pInput->KeyPressed(a_pInput->Eight) || a_pInput->KeyPressed(a_pInput->Num_Eight))
                *text += "8";
            if (a_pInput->KeyPressed(a_pInput->Nine) || a_pInput->KeyPressed(a_pInput->Num_Nine))
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