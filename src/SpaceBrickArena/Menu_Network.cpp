#include "include/Menu_Network.h"

namespace Menu
{

    CNetwork::CNetwork(PuRe_Timer* a_pTimer)
    {
        this->m_pNavigation = new sba::CNavigation(1, 3);
        this->m_Focus = false;
        this->m_Focus2 = false;
        this->m_pTimer = a_pTimer;
        this->m_Server = 0;
    }

    CNetwork::~CNetwork()
    {
        SAFE_DELETE(this->m_pNavigation);
        this->DeleteServers();
    }

    void CNetwork::DeleteServers()
    {
        this->m_Servers.clear();
    }

    void CNetwork::Start()
    {
        this->m_getList = true;
        sba_Network->CreateBroadcast(false, sba::BroadcastPort);
        std::thread bThread(&CNetwork::ReceiveData, this);
        bThread.detach();
    }

    void CNetwork::ReceiveData()
    {
        printf("Thread Start!\n");
        SOCKET bsocket = sba_Network->GetBroadcast();
        sba_Network->SetBlockMode(bsocket, false);
        const int len = 1024;
        char buffer[len];
        while (this->m_getList)
        {
            memset(buffer,0,len);
            if (sba_Network->Receive(buffer, len, bsocket,true) > 0)
            {
                sba::SReceivePacket* rpacket = (sba::SReceivePacket*)buffer;
                if (rpacket->Head.Type == sba::EPacket::Broadcast)
                {
                    sba_Network->m_Mutex.lock();
                    sba::SBroadcastPacket* bp = (sba::SBroadcastPacket*)rpacket;
                    SServer s;
                    s.IP = bp->IP;
                    s.Name = bp->Name;
                    s.Port = bp->Port;
                    s.Players = bp->Players;
                    bool in = false;
                    for (unsigned int i = 0; i < this->m_Servers.size(); i++)
                    {
                        if (this->m_Servers[i] == s)
                        {
                            in = true;
                            this->m_Servers[i].LastUpdate = this->m_pTimer->GetTotalElapsedSeconds();
                            break;
                        }
                    }
                    if (!in)
                        this->m_Servers.push_back(s);
                    sba_Network->m_Mutex.unlock();
                }
            }
        }
        printf("Thread End!\n");
    }

    bool CNetwork::ConnectToServer(PuRe_IWindow* a_pWindow)
    {
        //seems possible, now connect
        if (sba_Network->Connect(false))
        {
            this->m_getList = false;
            sba_Network->DeleteBroadcast();
            this->DeleteServers();
            return true;
        }
        return false;
    }

    int CNetwork::Update(PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, int a_PlayerIdx)
    {
        if (this->m_pTimer == NULL)
            this->m_pTimer = a_pTimer;
        if (this->m_ServerWait != 0.0f)
        {
            this->m_ServerWait -= a_pTimer->GetElapsedSeconds();
            if (this->m_ServerWait < 0.0f)
                this->m_ServerWait = 0.0f;
        }

        float totalSeconds = a_pTimer->GetTotalElapsedSeconds();
        //All two seconds, delete old Servers
        if ((int)totalSeconds % 2 == 0)
        {
            sba_Network->m_Mutex.lock();
            for (unsigned int i = 0; i < this->m_Servers.size(); i++)
            {
                if (totalSeconds - this->m_Servers[i].LastUpdate > 4.0f) //since five seconds no response
                {
                    this->m_Servers.erase(this->m_Servers.begin()+i);
                    --i;
                }
            }
            sba_Network->m_Mutex.unlock();
        }

        if (!this->m_Focus)
        {
            PuRe_Vector2F nav = sba_Input->Direction(sba_Direction::Navigate, a_PlayerIdx);
            this->m_pNavigation->Update(*a_pTimer, nav);
            if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
            {
                sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                switch (this->m_pNavigation->GetFocusedElementId())
                {
                case 0: //Internet
                    this->m_Focus = true;
                    this->m_Focus2 = true;
                    break;
                case 1: //Lan
                    this->m_Server = 0;
                    this->m_Focus = true;
                    break;
                case 2: //Host
                    this->m_Focus = true;
                    this->m_Focus2 = true;
                    break;
                case 3: //Back
                    this->m_Focus = false;
                    this->m_Focus2 = false;
                    this->m_getList = false;
                    sba_Network->DeleteBroadcast();
                    this->DeleteServers();
                    return 0;
                    break;
                }
            }
        }
        else
        {
            PuRe_Vector2F dir = sba_Input->Direction(sba_Direction::Navigate, a_PlayerIdx);
            switch (this->m_pNavigation->GetFocusedElementId())
            {
            case 0: //internet
                if (this->m_Focus2)
                    sba_Network->Update(a_pInput, sba::EUpdate::IP);
                else
                    sba_Network->Update(a_pInput, sba::EUpdate::Port);
                if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
                {
                    sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (this->m_Focus2)
                    {
                        //check if ip works
                        std::string ip = sba_Network->m_IP;
                        size_t n = std::count(ip.begin(), ip.end(), '.');

                        if (n == 3 && ip[ip.length() - 1] != '.')
                            this->m_Focus2 = false;
                    }
                    else
                    {
                        //check if port is not null, else connect
                        if (sba_Network->m_Port.length() > 0 && std::stoi(sba_Network->m_Port) >= 1024)
                        {
                            //just test if he can create a ship, otherwise it crashes before the user is connected
                            if (this->ConnectToServer(a_pWindow))
                            {
                                this->m_Focus = false;
                                this->m_Focus2 = false;
                                return 2;
                            }
                        }
                    }
                }
                if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
                {
                    sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (this->m_Focus2)
                        this->m_Focus = false;
                    else
                        this->m_Focus2 = true;
                }
            case 1: //Lan
                if (this->m_ServerWait == 0)
                {
                    if (dir.Y > 0.5f)
                        this->m_Server--;
                    else if (dir.Y < -0.5f)
                        this->m_Server++;
                    this->m_ServerWait = 0.1f;
                }
                if (0 > this->m_Server)
                    this->m_Server = 0;
                if (this->m_Servers.size()-1 < (unsigned int)this->m_Server)
                    this->m_Server = this->m_Servers.size()-1;
                if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
                {
                    sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (this->m_Servers.size() != 0)
                    {
                        //test if he has a possible ship
                        sba_Network->m_Name = this->m_Servers[this->m_Server].Name;
                        sba_Network->m_IP = this->m_Servers[this->m_Server].IP;
                        sba_Network->m_Port = this->m_Servers[this->m_Server].Port;
                        if (this->ConnectToServer(a_pWindow))
                        {
                            this->m_Focus = false;
                            this->m_Focus2 = false;
                            return 2;
                        }
                    }
                }
                if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
                {
                    sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (this->m_Focus2)
                        this->m_Focus = false;
                    else
                        this->m_Focus2 = true;
                }
                break;
            case 2: //Host
                if (this->m_Focus2)
                    sba_Network->Update(a_pInput, sba::EUpdate::Name);
                else
                    sba_Network->Update(a_pInput, sba::EUpdate::Port);
                if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
                {
                    sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (this->m_Focus2)
                    {
                        int size = sba_Network->m_Name.length();
                        if (size > 1)
                            this->m_Focus2 = false;
                    }
                    else
                    {
                        //check if port is not null, else connect
                        if (sba_Network->m_Port.length() > 0 && std::stoi(sba_Network->m_Port) >= 1024 && std::stoi(sba_Network->m_Port) != sba::BroadcastPort)
                        {
                            if (sba_Network->Connect(true))
                            {
                                this->m_Focus = false;
                                this->m_Focus2 = false;

                                this->m_getList = false;
                                sba_Network->DeleteBroadcast();
                                sba_Network->CreateBroadcast(true, sba::BroadcastPort);
                                sba_Space->CreatePlayer(0, a_pWindow);
                                this->DeleteServers();
                                return 2;
                            }
                        }
                    }
                }
                if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
                {
                    sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (this->m_Focus2)
                        this->m_Focus = false;
                    else
                        this->m_Focus2 = true;
                }
            break;
            default:
                if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
                {
                    sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    this->m_Focus = false;
                }
                break;
            }
        }


        return 1;
    }

    void CNetwork::Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Timer* a_pTimer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        //DRAW UI
        Position.X = 1920 / 2.0f;
        Position.Y = 1080 / 2.0f;
        a_pSpriteReader->Draw(1, a_pRenderer, "kasten_gross", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pSpriteReader->Draw(1, a_pRenderer, "network_kasten", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        //Draw Bottom
        Position.X = 100.0f;
        Position.Y = 1080.0f;
        Position.Y -= 150.0f;

        PuRe_Color color = PuRe_Color(1.0f,1.0f,1.0f);
        PuRe_Vector3F highlight = Position;
        highlight.X += 175.0f;

        if (this->m_pNavigation->GetFocusedElementId() == 0)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "INTERNET", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

        Position.X += 400.0f;
        highlight.X += 340.0f;

        if (this->m_pNavigation->GetFocusedElementId() == 1)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.3f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "LAN", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
        Position.X += 300.0f;
        highlight.X += 410.0f;

        if (this->m_pNavigation->GetFocusedElementId() == 2)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.8f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "HOST A GAME", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

        Position.Y = 100.0f;
        Position.X = 100.0f;
        highlight = Position;
        highlight.X += 125.0f;

        if (this->m_pNavigation->GetFocusedElementId() == 3)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "BACK", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
        
        std::string text;
        switch (this->m_pNavigation->GetFocusedElementId())
        {
        case 0:
            Position.Y = 1080.0f - 380.0f;
            Position.X = 400.0f;
            highlight = Position;
            highlight.X += 70.0f;
            if (this->m_Focus&&this->m_Focus2)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.2f, 0.5f));
            text = "IP: " + sba_Network->m_IP;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, text.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            Position.X += 500.0f;
            highlight.X += 510.0f;
            text = "Port: " + sba_Network->m_Port;
            if (this->m_Focus&&!this->m_Focus2)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.2f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, text.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            break;
        case 1: //Lan
            Position.Y = 1080.0f - 230.0f;
            Position.X = 250.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "NAME", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            Position.X += 350.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "IP", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            Position.X += 350.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "PORT", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            Position.X += 350.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "PLAYER", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);

            for (unsigned int i = 0; i < this->m_Servers.size(); i++)
            {
                Position.Y -= 40.0f;
                Position.X = 250.0f;
                highlight = Position;
                highlight.X += 650.0f;
                if (this->m_Focus&&this->m_Server == i)
                    a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(1.5f, 0.5f));
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, this->m_Servers[i].Name, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
                Position.X += 350.0f;
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, this->m_Servers[i].IP, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
                Position.X += 350.0f;
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, this->m_Servers[i].Port, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
                Position.X += 350.0f;
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, std::to_string(this->m_Servers[i].Players), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            }
            break;
        case 2: //Host
            Position.Y = 1080.0f - 380.0f;
            Position.X = 400.0f;
            highlight = Position;
            highlight.X += 90.0f;
            if (this->m_Focus&&this->m_Focus2)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.2f, 0.5f));
            text = "Name: ";
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, text.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            Position.X += text.length()*26.0f; 
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Network->m_Name, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 30.0f, color);
            Position.X += 500.0f;
            highlight.X += 620.0f;
            text = "Port: " + sba_Network->m_Port;
            if (this->m_Focus&&!this->m_Focus2)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.2f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, text.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(24.0f, 24.0f, 0.0f), 30.0f, color);
            break;
        }
    }
}