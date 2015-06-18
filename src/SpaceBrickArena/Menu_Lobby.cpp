#include "include/Menu_Lobby.h"

namespace Menu
{

    CLobby::CLobby()
    {
        this->m_pNavigation = new sba::CNavigation(1, 1);
    }

    CLobby::~CLobby()
    {
        this->m_Run = false;
        SAFE_DELETE(this->m_pNavigation);
    }

    void CLobby::ListenLoop()
    {
        printf("Thread Start!\n");
        const int len = 1024;
        char buffer[len];
        sba_Network->SetBlockMode(false);
        while (this->m_Run)
        {
            memset(buffer, 0, len);
            if (sba_Network->Listen())
            {
                if (sba_Players.size() < sba::MaxPlayers)
                {
                    SOCKET s = sba_Network->Accept();
                    if (s > 0)
                    {
                        sba_Network->m_Mutex.lock();
                        printf("Client connected!\n");
                        int ID = 0; // 0 is Host
                        for (unsigned int i = 0; i < sba_Players.size(); i++)
                        {
                            if (ID == sba_Players[i]->ID)
                            {
                                ID++;
                                i = 0;
                            }
                        }
                        sba::SPlayer* p = new sba::SPlayer();
                        p->ID = ID;
                        p->NetworkInformation = s;
                        p->PadID = -1;
                        sba_Players.push_back(p);
                        printf("User %i joined!\n", ID);
                        std::thread HandleThread(&CLobby::ReceiveData, this, s);
                        HandleThread.detach(); 
                        sba_Network->m_Mutex.unlock();
                    }
                }
            }
        }
        sba_Network->SetBlockMode(true);
        printf("Thread End!\n");
    }

    void CLobby::ReceiveData(SOCKET s)
    {
        printf("Thread Start!\n");
        const int len = 1024;
        char buffer[len];
        sba_Network->SetBlockMode(false);
        while (this->m_Run)
        {
            memset(buffer, 0, len);
            if (sba_Network->Receive(buffer, len, s) > 0)
            {

            }
        }
        sba_Network->SetBlockMode(true);
        printf("Thread End!\n");
    }

    void CLobby::Start()
    {
        if (sba_Network->IsConnected())
        {
            this->m_Run = true;
            if (sba_Network->GetHost())
            {
                std::thread lThread(&CLobby::ListenLoop, this);
                lThread.detach();
            }
            else
            {
                std::thread rThread(&CLobby::ReceiveData, this,sba_Network->GetSocket());
                rThread.detach();
            }
        }
    }

    int CLobby::Update(PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, int a_PlayerIdx)
    {
        if (sba_Network->IsConnected())
        {   
            sba::SBroadcastPacket bp;
            bp.Head.Type = sba::Broadcast;
            memcpy(bp.IP, sba_Network->m_IP.c_str(), sba::MaxLength);
            memcpy(bp.Port, sba_Network->m_Port.c_str(), sba::MaxLength);
            memcpy(bp.Name, sba_Network->m_Name.c_str(), sba::MaxName);
            if ((int)a_pTimer->GetTotalElapsedSeconds() % 2 == 0)
                sba_Network->Broadcast((char*)&bp, sizeof(sba::SBroadcastPacket)); //Now Spam it!
        }
        this->m_pNavigation->Update(*a_pTimer, sba_Input->Direction(sba_Direction::MenuMove, a_PlayerIdx));
        if (sba_Input->ButtonPressed(sba_Button::MenuClick, a_PlayerIdx))
        {
            switch (this->m_pNavigation->GetFocusedElementId())
            {
            case 0: //Start
                this->m_Run = false;
                return 2;
                break;
            case 1: //Back
                this->m_Run = false;
                return 0;
                break;
            }
        }

        for (int i = 1; i < 4; i++)
        {
            if (a_pInput->GamepadPressed(a_pInput->Pad_A, i))
            {
                bool exist = false;
                for (unsigned j = 0; j<sba_Players.size(); j++)
                {
                    if (sba_Players[j]->PadID == i)
                    {
                        exist = true;
                        break;
                    }
                }
                if (!exist)
                    sba_Space->CreatePlayer(i, a_pWindow);
            }
            else if (a_pInput->GamepadPressed(a_pInput->Pad_B, i))
            {
                for (unsigned j = 0; j<sba_Players.size(); j++)
                {
                    if (sba_Players[j]->PadID == i)
                    {
                        sba_Space->DeletePlayer(j);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < 4; i++)
        {
            bool leftPress = a_pInput->GamepadPressed(a_pInput->DPAD_Left, i);
            bool rightPress = a_pInput->GamepadPressed(a_pInput->DPAD_Right, i);
            if (leftPress || rightPress)
            {
                for (unsigned j = 0; j<sba_Players.size(); j++)
                {
                    if (sba_Players[j]->PadID == i)
                    {
                        sba::SPlayer* p = sba_Players[j];
                        int s = 0;
                        const char* path = "../data/ships/";
                        bool right = false;

                        std::string file = a_pWindow->GetFileAtIndex(s, path);
                        std::string lastFile = file;
                        std::string working = file;
                        bool found = false;
                        //aslong as the file is not right
                        while (!right)
                        {
                            if (!found&&file.substr(0, file.find_last_of(".")) == p->Ship->GetName())
                            { 
                                //if he has not been found and the file is the current one
                                working = file;
                                found = true; //current one found
                            }
                            else if (found)
                            {
                                //if he has been found
                                if (file.substr(file.find_last_of(".") + 1) == "ship")
                                {
                                    //this is a ship, take it!
                                    working = file;
                                    break;
                                }
                            }
                            if (!found||rightPress)
                                s++;
                            else if (leftPress)
                                s--;
                            if (s >= 0)
                                file = a_pWindow->GetFileAtIndex(s, path);
                            if (s < 0||lastFile == file)
                            {
                                file = working;
                                break;
                            }
                            else
                                lastFile = file;
                        }

                        std::string name = file.substr(0, file.find_last_of("."));

                        file = path + file;
                        TheBrick::CSerializer serializer;
                        serializer.OpenRead(file.c_str());
                        ong::vec3 pos = ong::vec3(0.0f, 0.0f, 0.0f);
                        SAFE_DELETE(p->Ship);
                        p->Ship = new TheBrick::CSpaceship(*sba_World, name);
                        p->Ship->Deserialize(serializer, *sba_BrickManager, *sba_World);
                        serializer.Close();
                        break;
                    }
                }
            }
        }


        return 1;
    }

    void CLobby::Render(PuRe_Renderer* a_pRenderer,PuRe_Timer* a_pTimer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        Position.X = 100.0f;
        Position.Y = 1080.0f;
        Position.Y -= 100.0f;

        PuRe_Color color = PuRe_Color();

        if (this->m_pNavigation->GetFocusedElementId() == 0)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Start", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);

        if (this->m_pNavigation->GetFocusedElementId() == 1)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        Position.Y = 100.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Back", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);

        Position.X = 1920.0f - 600.0f;
        Position.Y = 1080.0f - 300.0f;
        color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
        Position.X += 300.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Ship", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
        Position.Y -= 64.0f;
        Position.X -= 300.0f;
        color = PuRe_Color(1.0f, 1.0f, 1.0f);
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            std::string name = "Player " + std::to_string(sba_Players[i]->ID);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, name.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
            Position.X += 300.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Players[i]->Ship->GetName(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
            Position.Y -= 64.0f;
            Position.X -= 300.0f;
        }
        int localPlayer = 0;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
                localPlayer++;
        }
        for (int i = 0; i < 4 - localPlayer; i++)
        {
            PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f);
            color.A = ((sin(a_pTimer->GetTotalElapsedSeconds()*2.0f))+0.8f)/2.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Press A to join ...", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 20.0f, color);
            Position.Y -= 64.0f;
        }

        if (sba_Network->IsConnected())
        {
            Position.X = 1920.0f - 500.0f;
            Position.Y = 1080.0f - 50.0f;
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Name: " + sba_Network->m_Name, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 22.0f, color);
            Position.Y -= 64.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "IP: " + sba_Network->m_IP, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 22.0f, color);
            Position.Y -= 64.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Port: " + sba_Network->m_Port, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 22.0f, color);
        }
    }
}