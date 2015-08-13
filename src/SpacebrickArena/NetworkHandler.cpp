#include "include/NetworkHandler.h"
#include "include/Space.h"

namespace sba
{
    CNetworkHandler::CNetworkHandler(CIniReader* a_pIniReader)
    {
        this->m_pSocket = new PuRe_Socket();
        this->m_pBSocket = new PuRe_Socket();
        this->m_IP = a_pIniReader->GetValue("IP");
        this->m_Port = a_pIniReader->GetValue("Port");
        this->m_Host = false;
    }


    // **************************************************************************
    // **************************************************************************
    CNetworkHandler::~CNetworkHandler()
    {
        if (this->m_Connected)
            this->Disconnect();
        SAFE_DELETE(this->m_pBSocket);
        SAFE_DELETE(this->m_pSocket);

    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Disconnect()
    {
        this->m_pSocket->Cleanup();
        this->DeleteBroadcast();
        this->m_Connected = false;
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::DeleteBroadcast()
    {
        this->m_pBSocket->Cleanup();
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
        if (a_Broadcast)
            this->m_pBSocket->Initialize(PuRe_SocketType::Broadcaster, PuRe_Protocol::UDP, a_Port,"127.0.0.1");
        else
            this->m_pBSocket->Initialize(PuRe_SocketType::Broadcasted, PuRe_Protocol::UDP, a_Port, "0.0.0.0");
    }


    // **************************************************************************
    // **************************************************************************
    bool CNetworkHandler::Connect(bool a_Host)
    {
        this->m_Host = a_Host;
        if (a_Host)
        {
            this->m_pSocket->Initialize(PuRe_SocketType::Host, PuRe_Protocol::TCP, std::stoi(this->m_Port), "127.0.0.1");
            this->m_IP = this->m_pSocket->GetIP();
        }
        else
            this->m_pSocket->Initialize(PuRe_SocketType::Client, PuRe_Protocol::TCP, std::stoi(this->m_Port), this->m_IP.c_str());
        sba_Options->SetValue("IP", this->m_IP);
        sba_Options->SetValue("Port", this->m_Port);
        sba_Options->Save();
        if (this->m_pSocket->IsWorking())
            this->m_Connected = true;
        else
            return false;
        return true;
    }

    // **************************************************************************
    // **************************************************************************
    SOCKET CNetworkHandler::Accept()
    {
        SOCKADDR_IN clientData;
        if (this->m_pSocket->Select(this->m_pSocket->GetSocket(),PuRe_SocketSelect::Read,1000))
        {
            SOCKET s = this->m_pSocket->Accept(&clientData);
            if (s != 4294967295)
                return s;
        }
        return NULL;
    }

    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::SetBlockMode(SOCKET s,bool a_Block)
    {
        if (this->m_pSocket->IsWorking())
            this->m_pSocket->SetMode(s, a_Block);
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
    long CNetworkHandler::Receive(char* a_pBuffer, int a_Size, SOCKET a_pSender,bool a_Select)
    {
        long result = 0;
        //wait for socket to be ready to get data
        if (a_Select)
        {
            long selerror = 0;
            while (selerror == 0)
                selerror = this->m_pSocket->Select(a_pSender, PuRe_SocketSelect::Read, 1000);
            //if he is ready, check if he really is ready or if he got an error
            if (selerror > 0)
            {
                //ready, receive
                a_Select = false;
            }
            else
            {
                //got an error, print it
                int err = this->m_pSocket->GetError();
                printf("Error Receiving Select: %i\n", this->m_pSocket->GetError());
            }
        }
        if (!a_Select)
        {
            result = this->m_pSocket->Receive(a_pSender, a_pBuffer, a_Size);
            if (result > -1)
                printf("Received Data with result %d\n", result); //received
            else
            {
                //nope, receive got error, print it
                int err = this->m_pSocket->GetError();
                if (err != 10035)
                    printf("Error Receiving: %i\n", this->m_pSocket->GetError());
                else
                    return 0;
            }
        }
        return result;
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::SendHost(char* a_pBuffer, int a_Size, bool a_Select)
    {
        if (this->m_pSocket != NULL)
            this->Send(a_pBuffer, a_Size, this->m_pSocket->GetSocket(), a_Select);
    }


    // **************************************************************************
    // **************************************************************************
    void CNetworkHandler::Send(char* a_pBuffer, int a_Size, SOCKET a_Receiver, bool a_Select)
    {
        if (this->m_pSocket != NULL)
        {

            long result = 0;
            //wait for socket to be ready to send data
            if (a_Select)
            {
                long selerror = 0;
                while (selerror == 0)
                    selerror = this->m_pSocket->Select(a_Receiver, PuRe_SocketSelect::Write, 1000);
                //if he is ready, check if he really is ready or if he got an error
                if (selerror > 0)
                {
                    //ready, send
                    a_Select = false;
                }
                else
                {
                    //got an error, print it
                    int err = this->m_pSocket->GetError();
                    printf("Error Send Select: %i\n", this->m_pSocket->GetError());
                }
            }
            if (!a_Select)
            {
                //ready, receive
                result = this->m_pSocket->Send(a_Receiver, a_pBuffer, a_Size);
                if (result > -1)
                    printf("Send Data with result %d\n", result); //received
                else
                {
                    //nope, receive got error, print it
                    int err = this->m_pSocket->GetError();
                    printf("Error Send: %i\n", this->m_pSocket->GetError());
                }
            }
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