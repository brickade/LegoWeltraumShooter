#include "include/NetworkHandler.h"

namespace sba
{
    CNetworkHandler::CNetworkHandler()
    {
        this->m_IP = sba::CIniReader::Instance()->GetValue("IP");
        this->m_Port = sba::CIniReader::Instance()->GetValue("Port");
        this->m_Host = false;
    }


    // **************************************************************************
    // **************************************************************************
    CNetworkHandler::~CNetworkHandler()
    {
        this->Disconnect();

    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Disconnect()
    {
        SAFE_DELETE(this->m_pSocket);
        this->DeleteBroadcast();
        this->m_Connected = false;
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::DeleteBroadcast()
    {
        SAFE_DELETE(this->m_pBSocket);
    }


    // **************************************************************************
    // **************************************************************************
    bool CNetworkHandler::GetHost()
    {
        return this->m_Host;
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Broadcast(char* a_pBuffer, int a_Size)
    {
        this->m_pBSocket->Broadcast(a_pBuffer, a_Size);
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::CreateBroadcast(bool a_Broadcast,int a_Port)
    {
        this->m_pBSocket = new PuRe_Socket(PuRe_SocketType::Broadcast, PuRe_Protocol::UDP, a_Port, "0.0.0.0");
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Connect(bool a_Host)
    {
        this->m_Host = a_Host;
        if (a_Host)
        {
            this->m_pSocket = new PuRe_Socket(PuRe_SocketType::Host,PuRe_Protocol::TCP, std::stoi(this->m_Port));
            this->m_IP = this->m_pSocket->GetIP();
        }
        else
            this->m_pSocket = new PuRe_Socket(PuRe_SocketType::Client, PuRe_Protocol::TCP, std::stoi(this->m_Port), this->m_IP.c_str());
        sba::CIniReader::Instance()->SetValue("IP", this->m_IP);
        sba::CIniReader::Instance()->SetValue("Port", this->m_Port);
        sba::CIniReader::Instance()->Save();
        this->m_Connected = true;
    }

    // **************************************************************************
    // **************************************************************************
    SOCKET CNetworkHandler::Accept()
    {
        return this->m_pSocket->Accept(nullptr);
    }

    // **************************************************************************
    // **************************************************************************
    int CNetworkHandler::GetBroadcast()
    {
        return this->m_pBSocket->GetSocket();
    }

    // **************************************************************************
    // **************************************************************************
    int CNetworkHandler::GetSocket()
    {
        return this->m_pSocket->GetSocket();
    }

    // **************************************************************************
    // **************************************************************************
    bool CNetworkHandler::IsConnected()
    {
        return this->m_Connected;
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
        return 1;
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Update(PuRe_IInput* a_pInput, EUpdate a_What)
    {
        std::string* text = nullptr;
        if (a_What == EUpdate::IP)
            text = &this->m_IP;
        else if (a_What == EUpdate::Port)
            text = &this->m_Port;
        else
            text = &this->m_Name;

        if (a_pInput->KeyPressed(a_pInput->Backspace) && text->length() != 0)
            *text = text->substr(0, text->length() - 1);
        if (a_What == EUpdate::Name&&text->length() < MaxName || a_What != EUpdate::Name&&text->length() < MaxLength)
        {
            //handle numbers for all
            if (a_pInput->KeyPressed(a_pInput->Zero) || a_pInput->KeyPressed(a_pInput->Num_Zero))
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
            //handle . for IP only
            if (a_What == EUpdate::IP&& a_pInput->KeyPressed(a_pInput->Period))
                *text += ".";

            //Handle A-Z for Name Update only
            if (a_What == EUpdate::Name)
            {
                for (int i = 0; i < 26; i++)
                {
                    int key = a_pInput->A + i;
                    int first = 'A' - 0;
                    if (a_pInput->KeyPressed((PuReEngine::Core::IInput::EKeys)key))
                    {
                        key = first+i;
                        char val = (char)key;
                        *text += val; 
                    }
                }
            }
        }
    }
}