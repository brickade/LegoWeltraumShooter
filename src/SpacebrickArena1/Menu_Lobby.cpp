#include "include/Menu_Lobby.h"

namespace Menu
{

    CLobby::CLobby(PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics, PuRe_Timer* a_pTimer) : m_pWindow(a_pWindow)
    {
        this->m_pNavigation = new sba::CNavigation(1, 2);
        this->m_Start = false;
        this->m_Focus = false;
        this->m_pMapSprites = new sba::CSpriteReader(a_pGraphics, "../data/textures/ui/lobby_maps.png", "../data/textures/ui/lobby_maps.txt");
        this->m_ShipSelect = false;
        this->m_pNetwork = new sba::CLobbyNetwork(a_pWindow, a_pTimer);
        this->m_ShipSelect = false;
        this->m_pUnknownShip = new PuRe_Sprite(a_pGraphics, "../data/textures/unknownship.png");
    }

    CLobby::~CLobby()
    {
        //we left, send to server that we left
        SAFE_DELETE(this->m_pUnknownShip);
        SAFE_DELETE(this->m_pNetwork);
        SAFE_DELETE(this->m_pNavigation);
        SAFE_DELETE(this->m_pMapSprites);
    }

    void CLobby::Start()
    {
        this->m_pNetwork->Start();
    }

    int CLobby::Update(PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, int a_PlayerIdx)
    {
        this->m_pNetwork->Broadcast();

        if (sba_Network->IsConnected())
            if (!sba_Network->GetHost())
            {
                this->m_Start = this->m_pNetwork->IsStarted();
                this->m_ShipSelect = this->m_pNetwork->IsShip();
                if (!this->m_Start&&this->m_pNetwork->IsEnded())
                {
                    this->m_ShipSelect = false;
                    this->m_pNetwork->Exit();
                    return 0;
                }
            }

        if (this->m_Start)
        {
            if (this->m_pNetwork->CheckSend())
            {
                this->m_pNetwork->Exit();
                return 2;
            }
        }
        PuRe_Vector2F Navigate = sba_Input->Direction(sba_Direction::Navigate, a_PlayerIdx);
        if (!this->m_Focus)
        {
            Navigate.X = 0.0f;
            this->m_pNavigation->Update(*a_pTimer, Navigate);
            if (this->m_ShipSelect&&this->m_pNavigation->GetFocusedElementId() >= 2)
            {
                if (Navigate.Y < 0.0f)
                    this->m_pNavigation->SetFocusedElementId(0);
                else
                    this->m_pNavigation->SetFocusedElementId(1);
            }
        }

        if (!this->m_ShipSelect&&this->m_pNavigation->GetFocusedElementId() == 1 && this->m_Focus)
        {
            bool canchange = true;
            if (sba_Network->IsConnected())
                if (!sba_Network->GetHost())
                    canchange = false;
            if (canchange)
            {
                bool leftPress = false;
                bool rightPress = false;
                if (Navigate.X > 0.5f)
                    rightPress = true;
                else if (Navigate.X < -0.5f)
                    leftPress = true;
                if (leftPress || rightPress)
                {
                    std::string file = a_pWindow->GetFileAtIndex(0, "../data/maps/");
                    std::string lastFile = file;
                    std::string working = sba_Map->GetName();
                    bool found = false;
                    bool right = false;
                    int s = 0;
                    //aslong as the file is not right
                    while (!right)
                    {
                        if (!found&&file == working + ".map")
                            found = true; //current one found
                        else if (found)
                        {
                            //if he has been found
                            if (file.substr(file.find_last_of(".") + 1) == "map")
                            {
                                //this is a map, take it!
                                working = file.substr(0, file.find_last_of("."));
                                break;
                            }
                        }
                        if (!found || rightPress)
                            s++;
                        else if (leftPress)
                            s--;
                        if (s >= 0)
                            file = a_pWindow->GetFileAtIndex(s, "../data/maps/");
                        if (s < 0 || lastFile == file)
                        {
                            file = working;
                            break;
                        }
                        else
                            lastFile = file;
                    }
                    sba_Map->SetMap(working.c_str());
                    this->m_pNetwork->SendMap(working);
                }
            }
        }
        if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            if (this->m_ShipSelect)
            {
                if (this->m_pNavigation->GetFocusedElementId() == 0)
                {
                    this->m_pNetwork->SendCommand();
                    this->m_Start = true;
                }
                else if (this->m_pNavigation->GetFocusedElementId() == 1)
                {
                    this->m_ShipSelect = false;
                    if (sba_Network->IsConnected())
                        if (sba_Network->GetHost())
                            this->m_pNetwork->GoBack();
                    this->m_pNavigation->SetFocusedElementId(0);
                }
            }
            else
            {
                switch (this->m_pNavigation->GetFocusedElementId())
                {
                case 0: //Start
                    //change this to 1 for min 2 players
                    //this->m_pNetwork->SendCommand();
                    //this->m_Start = true;
                    if (sba_Network->IsConnected())
                        if (sba_Network->GetHost())
                            this->m_pNetwork->GoShip();
                    this->m_ShipSelect = true;
                    break;
                case 1: //Map
                    if (sba_Network->IsConnected())
                    {
                        if (sba_Network->GetHost())
                            this->m_Focus = true;
                    }
                    else
                        this->m_Focus = true;
                    break;
                case 2: //Back
                    this->m_pNetwork->Exit();
                    return 0;
                    break;
                }
            }
        }
        else if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            if (this->m_ShipSelect)
                this->m_pNavigation->SetFocusedElementId(1);
            else
                this->m_Focus = false;
        }
        if (!this->m_ShipSelect)
        {
            for (int i = 1; i < 4; i++)
            {
                if (a_pInput->GamepadPressed(a_pInput->Pad_A, i))
                {
                    if (sba_Players.size() < sba::MaxPlayers)
                    {
                        bool exist = false;
                        for (unsigned j = 0; j < sba_Players.size(); j++)
                        {
                            if (sba_Players[j]->PadID == i)
                            {
                                exist = true;
                                break;
                            }
                        }
                        if (!exist)
                        {
                            if (!this->m_pNetwork->JoinPlayer(i))
                                sba_Space->CreatePlayer(i, a_pWindow);
                        }
                    }
                }
                else if (a_pInput->GamepadPressed(a_pInput->Pad_B, i))
                {
                    //delete player on network
                    if (!this->m_pNetwork->DeletePlayer(i))
                    {
                        //if host or not connected
                        for (unsigned j = 0; j < sba_Players.size(); j++)
                        {
                            if (sba_Players[j]->PadID == i)
                            {
                                sba_Space->DeletePlayer(j);
                                break;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                PuRe_Vector2F Navigate = sba_Input->Direction(sba_Direction::Navigate, i);
                bool leftPress = Navigate.X > 0.0f;
                bool rightPress = Navigate.X < 0.0f;
                if (i == 0 && this->m_Focus)
                {
                    leftPress = 0.0f;
                    rightPress = 0.0f;
                }
                if (leftPress || rightPress)
                {
                    for (unsigned j = 0; j < sba_Players.size(); j++)
                    {
                        if (sba_Players[j]->PadID == i)
                        {
                            sba::SPlayer* p = sba_Players[j];
                            if (p->ID != -1) //doesnt work if he hasn't been accepted
                            {
                                unsigned int sID = p->ShipID; //save old ID
                                SAFE_DELETE(p->Ship);
                                p->Ship = NULL;
                                std::vector<TheBrick::CBrickInstance**> engines;
                                std::vector<TheBrick::CBrickInstance**> cockpits;
                                std::vector<TheBrick::CBrickInstance**> weapons;
                                while (p->Ship == NULL)
                                {
                                    bool last = false;
                                    int lastID = p->ShipID;
                                    if (rightPress)
                                        p->ShipID++;
                                    else
                                        p->ShipID--;

                                    if (rightPress&&lastID == sba_ShipManager->GetShipCount() - 1)
                                    {
                                        p->ShipID = 0;
                                        last = true;
                                    }
                                    else if (leftPress&&lastID == 0)
                                    {
                                        p->ShipID = sba_ShipManager->GetShipCount() - 1;
                                        last = true;
                                    }
                                    engines.clear();
                                    cockpits.clear();
                                    weapons.clear();

                                    p->Ship = sba_ShipManager->GetShip(p->ShipID);
                                    p->Ship->GetEngines(engines);
                                    p->Ship->GetCockpits(cockpits);
                                    p->Ship->GetWeapons(weapons);

                                    if (engines.size() == 0 || cockpits.size() == 0 || weapons.size() == 0)
                                    {
                                        SAFE_DELETE(p->Ship);
                                        p->Ship = NULL;
                                    }
                                    else
                                        break;
                                    if (last)
                                    {
                                        p->ShipID = sID;
                                        p->Ship = sba_ShipManager->GetShip(p->ShipID);
                                        break;
                                    }
                                }

                                this->m_pNetwork->SendUserData(p);
                            }
                            break;
                        }
                    }
                }
            }
        }

        return 1;
    }

    void CLobby::Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Timer* a_pTimer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        PuRe_Vector3F highlight = Position;
        PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f);
        PuRe_Vector3F Size = PuRe_Vector3F(0.5f, 0.5f, 0.0f);
        if (this->m_ShipSelect)
        {
            //DRAW UI
            Position.X = 1920 / 2.0f;
            Position.Y = 1080 / 2.0f;
            a_pSpriteReader->Draw(1, a_pRenderer, "kasten_gross", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            Position.X -= 1920.0f / 5.0f;
            Position.Y += 1080 / 5.0f;
            a_pSpriteReader->Draw(1, a_pRenderer, "Auswahl_rechts_unten", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            Position.Y += 150.0f;
            Position.X -= 200.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player 1", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
            if (sba_Players.size() >= 1)
            {
                Position.Y -= 300.0f;
                Position.X -= 80.0f;
                if (sba_Players[0]->PadID != -1)
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Players[0]->Ship->GetName(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                else
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Unknown", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                Position.Y += 300.0f;
                Position.X += 80.0f;
            }
            Position.Y -= 150.0f;
            Position.X += 200.0f;
            if (sba_Players.size() >= 1)
            {
                Position.X -= 75.0f;
                Position.Y -= 25.0f;
                if (sba_Players[0]->PadID != -1)
                    sba_Renderer->Draw(1, false, (*sba_ShipManager)[sba_Players[0]->ShipID], sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                else
                    sba_Renderer->Draw(1, false, this->m_pUnknownShip, sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                Position.X += 75.0f;
                Position.Y += 25.0f;
            }
            Position.X += 1920.0f / 2.5f;
            a_pSpriteReader->Draw(1, a_pRenderer, "Auswahl_links_unten", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            Position.Y += 150.0f;
            Position.X -= 120.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player 2", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
            if (sba_Players.size() >= 2)
            {
                Position.Y -= 300.0f;
                Position.X -= 80.0f;
                if (sba_Players[1]->PadID != -1)
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Players[1]->Ship->GetName(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                else
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Unknown", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                Position.Y += 300.0f;
                Position.X += 80.0f;
            }
            Position.X += 120.0f;
            Position.Y -= 150.0f;
            if (sba_Players.size() >= 2)
            {
                Position.X -= 75.0f;
                Position.Y -= 25.0f;
                if (sba_Players[1]->PadID != -1)
                    sba_Renderer->Draw(1, false, (*sba_ShipManager)[sba_Players[1]->ShipID], sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                else
                    sba_Renderer->Draw(1, false, this->m_pUnknownShip, sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                Position.X += 75.0f;
                Position.Y += 25.0f;
            }
            Position.Y -= 1080 / 2.5f;
            a_pSpriteReader->Draw(1, a_pRenderer, "Auswahl_links_oben", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            Position.Y += 150.0f;
            Position.X -= 120.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player 3", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
            if (sba_Players.size() >= 3)
            {
                Position.Y -= 300.0f;
                Position.X -= 80.0f;
                if (sba_Players[2]->PadID != -1)
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Players[2]->Ship->GetName(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                else
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Unknown", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                Position.Y += 300.0f;
                Position.X += 80.0f;
            }
            Position.Y -= 150.0f;
            Position.X += 120.0f;
            if (sba_Players.size() >= 3)
            {
                Position.X -= 75.0f;
                Position.Y -= 25.0f;
                if (sba_Players[2]->PadID != -1)
                    sba_Renderer->Draw(1, false, (*sba_ShipManager)[sba_Players[2]->ShipID], sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                else
                    sba_Renderer->Draw(1, false, this->m_pUnknownShip, sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                Position.X += 75.0f;
                Position.Y += 25.0f;
            }
            Position.X -= 1920.0f / 2.5f;
            a_pSpriteReader->Draw(1, a_pRenderer, "Auswahl_rechts_oben", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            Position.Y += 150.0f;
            Position.X -= 200.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player 4", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
            if (sba_Players.size() >= 4)
            {
                Position.Y -= 300.0f;
                Position.X -= 80.0f;
                if (sba_Players[3]->PadID != -1)
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Players[3]->Ship->GetName(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                else
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Unknown", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
                Position.Y += 300.0f;
                Position.X += 80.0f;
            }
            Position.Y -= 150.0f;
            Position.X += 200.0f;
            if (sba_Players.size() >= 4)
            {
                Position.X -= 75.0f;
                Position.Y -= 25.0f;
                if (sba_Players[3]->PadID != -1)
                    sba_Renderer->Draw(1, false, (*sba_ShipManager)[sba_Players[3]->ShipID], sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                else
                    sba_Renderer->Draw(1, false, this->m_pUnknownShip, sba_Space->SpriteMaterial, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(), Size);
                Position.X += 75.0f;
                Position.Y += 25.0f;
            }
            Position.Y = 100.0f;
            Position.X = 100.0f;
            highlight = Position;
            highlight.X += 125.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 1)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "BACK", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
            Position.Y = 980.0f;
            highlight = Position;
            highlight.X += 125.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 0)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Start", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

        }
        else
        {
            //DRAW UI
            Position.X = 1450;
            Position.Y = 550;
            a_pSpriteReader->Draw(1, a_pRenderer, "lobby_kasten_tile", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            Position.X -= 1050;
            Position.Y -= 100;
            a_pSpriteReader->Draw(1, a_pRenderer, "lobby_map", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            Position.X += 1;
            Position.Y -= 12;
            this->m_pMapSprites->Draw(1, a_pRenderer, sba_Map->GetName().c_str(), a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            //Draw Bottom
            Position.X = 100.0f;
            Position.Y = 980.0f;
            highlight = Position;
            highlight.X += 125.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 0)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Start", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

            Position.Y -= 100.0f;
            highlight.Y = Position.Y;
            if (this->m_pNavigation->GetFocusedElementId() == 1 && !this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "MAP: ", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
            Position.X += 300.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 1 && this->m_Focus)
            {
                highlight.X += 300.0f;
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            }
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Map->GetName().c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);


            if (sba_Network->IsConnected())
            {
                Position.X -= 300.0f;
                Position.Y -= 100.0f;
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "IP: " + sba_Network->m_IP + ":" + sba_Network->m_Port, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
            }
            Position.Y = 100.0f;
            Position.X = 100.0f;
            highlight = Position;
            highlight.X += 125.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 2)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "BACK", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

            Position.X = 1100;
            Position.Y = 1000;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "PLAYER", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 30.0f, color);
            Position.Y -= 64.0f;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                std::string name = "PLAYER " + std::to_string(sba_Players[i]->ID + 1);
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, name.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 26.0f, color);
                Position.Y -= 64.0f;
            }
            int players = (sba::MaxPlayers - sba_Players.size()); //still guilty players
            for (int i = 0; i < players; i++)
            {
                color.A = ((sin(a_pTimer->GetTotalElapsedSeconds()*2.0f)) + 0.8f) / 2.0f;
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "PRESS A TO JOIN", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 26.0f, color);
                Position.Y -= 64.0f;
            }
        }
    }
}