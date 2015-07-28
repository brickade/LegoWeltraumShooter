#include "include/Menu_Lobby.h"

namespace Menu
{

    CLobby::CLobby(PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics) : m_pWindow(a_pWindow)
    {
        this->m_pNavigation = new sba::CNavigation(1, 2);
        this->m_GameEnd = false;
        this->m_Start = false;
        this->m_Focus = false;
        this->m_pMapSprites = new sba::CSpriteReader(a_pGraphics, "../data/textures/ui/lobby_maps.png", "../data/textures/ui/lobby_maps.txt");
    }

    CLobby::~CLobby()
    {
        //we left, send to server that we left
        if (!this->m_Start)
        {
            sba::SLeftPacket lp;
            lp.Head.Type = sba::EPacket::Left;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                if (sba_Players[i]->PadID != -1)
                {
                    lp.Who = sba_Players[i]->ID;
                    printf("Left: %i", lp.Who);
                    sba_Network->SendHost((char*)&lp, sizeof(sba::SLeftPacket), true);
                }
            }
        }
        this->m_Run = false;
        this->m_Start = true;
        for (unsigned int i = 0; i < this->m_Threads.size(); i++)
        {
            if (this->m_Threads[i]->joinable())
                this->m_Threads[i]->join();
            SAFE_DELETE(this->m_Threads[i]);
        }
        this->m_Threads.clear();
        SAFE_DELETE(this->m_pNavigation);
        SAFE_DELETE(this->m_pMapSprites);
    }

    void CLobby::ListenLoop()
    {
        printf("Thread Start LISTEN!\n");
        sba_Network->SetBlockMode(sba_Network->GetSocket(), false);
        const int len = 1024;
        char buffer[len];
        while (this->m_Run)
        {
            memset(buffer, 0, len);
            if (sba_Network->Listen())
            {
                if (sba_Players.size() < sba::MaxPlayers)
                {
                    SOCKET s = sba_Network->Accept();
                    if (this->m_Start || !this->m_Run)
                        break;
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
                        printf("User %i joined!\n", ID);
                        printf("Send user all other Users\n");
                        for (unsigned i = 0; i < sba_Players.size(); i++)
                        {
                            printf("Send User %i!\n", sba_Players[i]->ID);
                            //send the Data to the Server
                            sba::SUserPacket up;
                            up.Head.Type = sba::EPacket::User;
                            up.ID = sba_Players[i]->ID;
                            int nameSize = (sizeof(char)*sba::MaxLength);
                            memset(up.Name, 0, sba::MaxLength + 1);
                            strncpy(up.Name, sba_Players[i]->Ship->GetName().c_str(), sba_Players[i]->Ship->GetName().size());
                            up.Name[sba_Players[i]->Ship->GetName().length()] = '\0';
                            //set data dynamic because we set our data self
                            int size = sizeof(up.Head.Type) + sizeof(up.ID) + nameSize;
                            //now send the packet to the host
                            printf("Send Ship Name and his ID %i!!\n", up.ID);
                            sba_Network->Send((char*)&up, size, s, true);
                            //Get the Data from the ship the player has
                            printf("Start sending Bricks!!\n");
                            sba::SBrickPacket bp;
                            bp.Head.Type = sba::EPacket::Brick;
                            bp.ID = sba_Players[i]->ID;
                            bp.BrickNum = sba_Players[i]->Ship->m_pBricks.size();
                            printf("Send Bricks\n");
                            for (unsigned int j = 0; j < sba_Players[i]->Ship->m_pBricks.size(); j++)
                                bp.Bricks[j] = sba_Players[i]->Ship->GetBrick(j);
                            sba_Network->Send((char*)&bp, sizeof(sba::SBrickPacket), s, true);
                            printf("All Bricks Sent!\n");
                        }
                        printf("Now Send IAM Packet!\n");
                        sba::SPlayer* p = new sba::SPlayer();
                        p->ID = ID;
                        p->NetworkInformation = s;
                        p->PadID = -1;
                        p->ShipID = -1;
                        p->Ship = NULL;
                        sba_Players.push_back(p);
                        //tell the user who he is
                        sba::SIamPacket iam;
                        iam.Head.Type = sba::EPacket::IAm;
                        iam.ID = ID;
                        iam.Pad = 0;
                        memcpy(iam.Map, sba_Map->GetName().c_str(), sba::MaxName);
                        sba_Network->Send((char*)&iam, sizeof(sba::SIamPacket), s, true);
                        printf("IAm Packet send to ID %i!\n", ID);

                        std::thread* rThread = new std::thread(&CLobby::ReceiveData, this, s);
                        this->m_Threads.push_back(rThread);
                        sba_Network->m_Mutex.unlock();
                    }
                }
            }
        }
        printf("Thread End LISTEN!\n");
    }

    void CLobby::ReceiveData(SOCKET s)
    {
        printf("Thread Start LOBBY!\n");
        sba_Network->SetBlockMode(s, false);
        //int shipSize = sizeof(unsigned int)*sizeof(ong::Transform)*sizeof(PuRe_Color) * 1000; 
        const int len = 16384;
        char buffer[len];
        char buffer2[len];
        memset(buffer, 0, len);
        memset(buffer2, 0, len);
        int packets = 0;
        int leftData = 0;

        bool locked = false;
        long packetSize = 0;
        while (this->m_Run)
        {

            long dataLeft = sba_Network->Receive(buffer, len, s, false);
            if (this->m_Start || !this->m_Run)
                break;
            if (dataLeft > 0)
            {
                //we received something, copy received into our buffer + offset
                memcpy(buffer2 + leftData, buffer, dataLeft);
                leftData += dataLeft;

                sba_Network->m_Mutex.lock();
                printf("Mutex Lock RD!\n");
                locked = true;

            }
            else if (dataLeft == -1) //data empty, seems closed
            {
                if (sba_Network->GetHost())
                {
                    sba::SLeftPacket sleft;
                    sleft.Head.Type = sba::EPacket::Left;
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Players[i]->NetworkInformation == s) //send to all that are not local
                        {
                            sleft.Who = sba_Players[i]->ID;
                            for (unsigned int j = 0; j < sba_Players.size(); j++)
                                sba_Network->Send((char*)&sleft, sizeof(sba::SLeftPacket), sba_Players[j]->NetworkInformation, true);

                            printf("Deleted Player %i!\n", sleft.Who);
                            sba_Space->DeletePlayer(i);
                            --i;
                        }
                    }
                }
                else
                {
                    this->m_GameEnd = true;
                    this->m_Run = false;
                }
                break;
            }

            while (leftData > 0) //aslong as we have data left
            {
                sba::SReceivePacket* rpacket = (sba::SReceivePacket*)buffer2;
                packetSize = 0;
                if (rpacket->Head.Type == sba::EPacket::Map&&leftData >= sizeof(sba::SMapPacket))
                {
                    sba::SMapPacket* smap = (sba::SMapPacket*)rpacket;
                    printf("Map changed to %s!\n", smap->Name);
                    sba_Map->SetMap(smap->Name);
                    packetSize = sizeof(sba::SMapPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Left&&leftData >= sizeof(sba::SLeftPacket))
                {
                    sba::SLeftPacket* sleft = (sba::SLeftPacket*)rpacket;
                    printf("%i left\n", sleft->Who);
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Network->GetHost() && sba_Players[i]->PadID == -1) //send to all that are not local
                            sba_Network->Send((char*)sleft, sizeof(sba::SLeftPacket), sba_Players[i]->NetworkInformation, true);
                        if (sleft->Who == sba_Players[i]->ID) //client and host sees this
                        {
                            printf("Deleted Player %i!\n", sleft->Who);
                            sba_Space->DeletePlayer(i);
                            --i;
                        }
                    }
                    packetSize = sizeof(sba::SLeftPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Start&&leftData >= sizeof(sba::SHeadPacket))
                {
                    printf("GAME START\n");
                    this->m_Start = true;
                    this->m_Run = false;
                    packetSize = sizeof(sba::SHeadPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::LobbyEnd&&leftData >= sizeof(sba::SHeadPacket))
                {
                    printf("GAME END\n");
                    this->m_GameEnd = true;
                    this->m_Run = false;
                    packetSize = sizeof(sba::SHeadPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::WhoamI&&leftData >= sizeof(sba::SWhoamIPacket))
                {
                    sba::SWhoamIPacket* wpackets = (sba::SWhoamIPacket*)rpacket;
                    //only received by the player, when a client localy pressed a
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
                    printf("Client created\n");
                    sba::SPlayer* p = new sba::SPlayer();
                    p->ID = ID;
                    p->NetworkInformation = s;
                    p->PadID = -1;
                    p->ShipID = -1;
                    p->Ship = NULL;
                    sba_Players.push_back(p);
                    printf("User %i joined remote locally!\n", ID);
                    //tell the user who he is
                    sba::SIamPacket iam;
                    iam.Head.Type = sba::EPacket::IAm;
                    iam.ID = ID;
                    iam.Pad = wpackets->Pad;
                    printf("Send Client to Clients\n");
                    sba_Network->Send((char*)&iam, sizeof(sba::SIamPacket), s, true);
                    packetSize = sizeof(sba::SWhoamIPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Brick&&leftData >= sizeof(sba::SBrickPacket))
                {
                    sba::SBrickPacket* bp = (sba::SBrickPacket*)rpacket;
                    std::vector<SOCKET> sendSockets; //to who we already send it
                    sendSockets.push_back(s);
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Network->GetHost())
                        {
                            if (sba_Players[i]->PadID == -1)
                            {
                                bool send = false;
                                for (unsigned int j = 0; j < sendSockets.size(); j++)
                                {
                                    if (sba_Players[i]->NetworkInformation == sendSockets[j])
                                    {
                                        send = true;
                                        break;
                                    }
                                }
                                if (!send)
                                {
                                    sba_Network->Send((char*)bp, sizeof(sba::SBrickPacket), sba_Players[i]->NetworkInformation, true);
                                    sendSockets.push_back(sba_Players[i]->NetworkInformation);

                                }

                            }
                        }
                        if (sba_Players[i]->ID == bp->ID)
                        {
                            for (int j = 0; j < bp->BrickNum; j++)
                                sba_Players[i]->Ship->AddBrick(bp->Bricks[j], sba_BrickManager->GetBrickArray(), *sba_World);
                            break;
                        }
                    }
                    sendSockets.clear();
                    packetSize = sizeof(sba::SBrickPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::User&&leftData >= sizeof(sba::SUserPacket))
                {
                    sba::SUserPacket* up = (sba::SUserPacket*)rpacket;
                    //For Server: Client send that he changed his ship
                    //For Client: Client joined or changed ship
                    bool exist = false;
                    printf("Receiving Client Data %i!\n", up->ID);
                    int nameSize = (sizeof(char)*sba::MaxLength);
                    int size = sizeof(up->Head.Type) + sizeof(up->ID) + nameSize;
                    up->Name[sba::MaxLength + 1] = '\0';
                    //existing clients
                    std::vector<SOCKET> sendSockets; //to who we already send it
                    sendSockets.push_back(s);
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Network->GetHost())
                        {
                            //if host, only send if the player is not the one we received this Data from (because they can be split screen)
                            printf("Try User %i to %i\n", up->ID, sba_Players[i]->ID);
                            printf("S1 %i S2 %i\n", s, sba_Players[i]->NetworkInformation);
                            if (sba_Players[i]->PadID == -1)
                            {
                                bool send = false;
                                for (unsigned j = 0; j < sendSockets.size(); j++)
                                {
                                    if (sba_Players[i]->NetworkInformation == sendSockets[j])
                                    {
                                        send = true;
                                        break;
                                    }
                                }
                                if (!send)
                                {
                                    printf("Send User %i to %i\n", up->ID, sba_Players[i]->ID);
                                    sba_Network->Send((char*)up, size, sba_Players[i]->NetworkInformation, true);
                                    sendSockets.push_back(sba_Players[i]->NetworkInformation);
                                }
                            }
                        }
                        if (up->ID == sba_Players[i]->ID)
                        {
                            printf("Created ship %s for player %i!\n", up->Name, up->ID);
                            exist = true;
                            SAFE_DELETE(sba_Players[i]->Ship);
                            sba_Players[i]->Ship = new sba::CSpaceship(*sba_World, up->Name);
                            break;
                        }
                    }
                    sendSockets.clear();
                    //if he does not exist, create new
                    if (!exist)
                    {
                        sba::SPlayer* p = new sba::SPlayer();
                        p->ID = up->ID;
                        printf("User %i joined!\n", up->ID);
                        p->NetworkInformation = s; //save socket, only important for server
                        p->PadID = -1;
                        p->ShipID = -1;
                        p->Ship = new sba::CSpaceship(*sba_World, up->Name);
                        sba_Players.push_back(p);
                    }
                    packetSize = size;
                }
                else if (rpacket->Head.Type == sba::EPacket::IAm&&leftData >= sizeof(sba::SIamPacket))
                {
                    //Data received for clients only when the Server accepted him
                    sba::SIamPacket* iam = (sba::SIamPacket*)rpacket;

                    sba_Map->SetMap(iam->Map);

                    printf("Got who I am: %i!\n", iam->ID);
                    sba::SPlayer* p;
                    sba_Space->CreatePlayer(iam->Pad, this->m_pWindow);
                    p = sba_Players[sba_Players.size() - 1];
                    p->ID = iam->ID;
                    //send the Data to the Server
                    sba::SUserPacket up;
                    up.Head.Type = sba::EPacket::User;
                    up.ID = iam->ID;
                    memset(up.Name, 0, sba::MaxLength + 1);
                    strncpy(up.Name, p->Ship->GetName().c_str(), p->Ship->GetName().size());
                    up.Name[p->Ship->GetName().length()] = '\0';
                    int nameSize = (sizeof(char)*sba::MaxLength);
                    //set data dynamic because we set our data self
                    int size = sizeof(up.Head.Type) + sizeof(up.ID) + nameSize;
                    //now send the packet to the host
                    printf("Send Ship Name and who I am %i!!\n", up.ID);
                    sba_Network->SendHost((char*)&up, size, true);
                    //Get the Data from the ship the player has
                    sba::SBrickPacket bp;
                    bp.Head.Type = sba::EPacket::Brick;
                    bp.ID = p->ID;
                    bp.BrickNum = p->Ship->m_pBricks.size();
                    for (unsigned int i = 0; i < p->Ship->m_pBricks.size(); i++)
                        bp.Bricks[i] = p->Ship->GetBrick(i);
                    sba_Network->SendHost((char*)&bp, sizeof(sba::SBrickPacket), true);
                    packetSize = sizeof(sba::SIamPacket);
                }

                if (packetSize == 0) //our excepted Data is not full there, so break and receive more
                {
                    break;
                }
                //we handeld our packet, so remove it from the buffer
                printf("Data we had before: %i\n", leftData);
                leftData -= packetSize;
                printf("Data left: %i\n", leftData);
                memcpy(buffer2, buffer2 + (int)packetSize, len - packetSize);
            }
            if (locked)
            {
                locked = false;
                printf("Mutex UnLock RD!\n");
                memset(buffer, 0, len);
                sba_Network->m_Mutex.unlock();
            }
        }
        printf("Thread End LOBBY!\n");
    }

    void CLobby::Start()
    {
        if (sba_Network->IsConnected())
        {
            this->m_Run = true;
            if (sba_Network->GetHost())
            {
                std::thread* lThread = new std::thread(&CLobby::ListenLoop, this);
                this->m_Threads.push_back(lThread);
            }
            else
            {
                std::thread* rThread = new std::thread(&CLobby::ReceiveData, this, sba_Network->GetSocket());
                this->m_Threads.push_back(rThread);
            }
        }
    }

    int CLobby::Update(PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, int a_PlayerIdx)
    {
        if (sba_Network->IsConnected())
        {
            if (sba_Network->GetHost())
            {
                sba::SBroadcastPacket bp;
                bp.Head.Type = sba::Broadcast;
                bp.Players = sba_Players.size();
                memcpy(bp.IP, sba_Network->m_IP.c_str(), sba::MaxLength);
                memcpy(bp.Port, sba_Network->m_Port.c_str(), sba::MaxLength);
                memcpy(bp.Name, sba_Network->m_Name.c_str(), sba::MaxName);
                if ((int)a_pTimer->GetTotalElapsedSeconds() % 2 == 0)
                    sba_Network->Broadcast((char*)&bp, sizeof(sba::SBroadcastPacket)); //Now Spam it!
            }
            else if (this->m_GameEnd) //server ended connection
            {
                this->m_GameEnd = false;
                return 0;
            }
        }
        if (this->m_Start)
        {
            this->m_Run = false;
            printf("Delete Threads\n");
            for (unsigned int i = 0; i < this->m_Threads.size(); i++)
            {
                if (this->m_Threads[i]->joinable())
                    this->m_Threads[i]->join();
                SAFE_DELETE(this->m_Threads[i]);
            }
            this->m_Threads.clear();
            return 2;
        }
        PuRe_Vector2F Navigate = sba_Input->Direction(sba_Direction::Navigate, a_PlayerIdx);
        if (!this->m_Focus)
        {
            Navigate.X = 0.0f;
            if (this->m_pNavigation->Update(*a_pTimer, Navigate))
                sba_SoundPlayer->PlaySound("menu_over", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
        }

        if (this->m_pNavigation->GetFocusedElementId() == 1 && this->m_Focus)
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
                    if (sba_Network->IsConnected())
                    {
                        sba::SMapPacket mp;
                        memset(&mp, 0, sizeof(sba::SMapPacket));
                        mp.Head.Type = sba::EPacket::Map;
                        memcpy(mp.Name, working.c_str(), sizeof(char)*working.length());
                        std::vector<SOCKET> sendSockets; //to who we already send it
                        for (unsigned int m = 0; m < sba_Players.size(); m++)
                        {
                            if (sba_Players[m]->PadID == -1)
                            {
                                bool send = false;
                                for (unsigned n = 0; n < sendSockets.size(); n++)
                                {
                                    if (sba_Players[m]->NetworkInformation == sendSockets[n])
                                    {
                                        send = true;
                                        break;
                                    }
                                }
                                if (!send)
                                {
                                    sba_Network->Send((char*)&mp, sizeof(sba::SMapPacket), sba_Players[m]->NetworkInformation, true);
                                    sendSockets.push_back(sba_Players[m]->NetworkInformation);
                                }
                            }
                        }
                        sendSockets.clear();
                    }
                }
            }
        }
        if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            switch (this->m_pNavigation->GetFocusedElementId())
            {
            case 0: //Start
                //change this to 1 for min 2 players
                if (sba_Players.size() > 0)
                {
                    if (sba_Network->IsConnected())
                    {
                        sba_Network->m_Mutex.lock();
                        if (sba_Network->GetHost())
                        {
                            printf("Game Start!\n");
                            this->m_Start = true;
                            sba::SHeadPacket hp;
                            hp.Type = sba::EPacket::Start;
                            std::vector<SOCKET> sendSockets; //to who we already send it
                            for (unsigned int m = 0; m < sba_Players.size(); m++)
                            {
                                if (sba_Players[m]->PadID == -1)
                                {
                                    bool send = false;
                                    for (unsigned n = 0; n < sendSockets.size(); n++)
                                    {
                                        if (sba_Players[m]->NetworkInformation == sendSockets[n])
                                        {
                                            send = true;
                                            break;
                                        }
                                    }
                                    if (!send)
                                    {
                                        sba_Network->Send((char*)&hp, sizeof(sba::SHeadPacket), sba_Players[m]->NetworkInformation, true);
                                        sendSockets.push_back(sba_Players[m]->NetworkInformation);
                                    }
                                }
                            }
                            sendSockets.clear();
                        }
                        sba_Network->m_Mutex.unlock();
                    }
                    else
                        this->m_Start = true;
                }
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
                if (sba_Network->IsConnected())
                {
                    sba_Network->m_Mutex.lock();
                    if (sba_Network->GetHost())
                    {
                        //host left, end game for all
                        sba::SHeadPacket hp;
                        hp.Type = sba::EPacket::LobbyEnd;
                        for (unsigned int i = 0; i < sba_Players.size(); i++)
                            sba_Network->Send((char*)&hp, sizeof(sba::SHeadPacket), sba_Players[i]->NetworkInformation, true);
                    }
                    else
                    {
                        //we left, send to server that we left
                        sba::SLeftPacket lp;
                        lp.Head.Type = sba::EPacket::Left;
                        for (unsigned int i = 0; i < sba_Players.size(); i++)
                        {
                            if (sba_Players[i]->PadID != -1)
                            {
                                lp.Who = sba_Players[i]->ID;
                                printf("Left: %i", lp.Who);
                                sba_Network->SendHost((char*)&lp, sizeof(sba::SLeftPacket), true);
                            }
                        }
                    }
                    sba_Network->m_Mutex.unlock();
                    this->m_Run = false;
                    for (unsigned int i = 0; i < this->m_Threads.size(); i++)
                    {
                        this->m_Threads[i]->join();
                        SAFE_DELETE(this->m_Threads[i]);
                    }
                    this->m_Threads.clear();
                }
                return 0;
                break;
            }
        }
        else if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            this->m_Focus = false;
        }

        for (int i = 1; i < 4; i++)
        {
            if (a_pInput->GamepadPressed(a_pInput->Pad_A, i))
            {
                if (sba_Network->IsConnected())
                    sba_Network->m_Mutex.lock();
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
                        if (sba_Network->IsConnected())
                        {
                            //only add one if there is already a local player
                            bool work = false;
                            for (unsigned int m = 0; m < sba_Players.size(); m++)
                            {
                                if (sba_Players[m]->PadID != -1)
                                {
                                    work = true;
                                    break;
                                }
                            }
                            //This is needed for when the client has to wait for the server to accept the client
                            if (work)
                            {
                                if (sba_Network->GetHost())
                                {
                                    sba_Space->CreatePlayer(i, a_pWindow);
                                    sba::SPlayer* p = sba_Players[sba_Players.size() - 1];
                                    //we are the host, so create new players freely

                                    //send the Data to the Server
                                    sba::SUserPacket up;
                                    up.Head.Type = sba::EPacket::User;
                                    up.ID = p->ID;
                                    int nameSize = (sizeof(char)*sba::MaxLength);
                                    memset(up.Name, 0, sba::MaxLength + 1);
                                    strncpy(up.Name, p->Ship->GetName().c_str(), p->Ship->GetName().size());
                                    up.Name[p->Ship->GetName().length()] = '\0';
                                    //set data dynamic because we set our data self
                                    int size = sizeof(up.Head.Type) + sizeof(up.ID) + nameSize;
                                    //now send the packet to the host
                                    std::vector<SOCKET> sendSockets; //to who we already send it
                                    for (unsigned int m = 0; m < sba_Players.size(); m++)
                                    {
                                        if (sba_Players[m]->PadID == -1)
                                        {
                                            bool send = false;
                                            for (unsigned n = 0; n < sendSockets.size(); n++)
                                            {
                                                if (sba_Players[m]->NetworkInformation == sendSockets[n])
                                                {
                                                    send = true;
                                                    break;
                                                }
                                            }
                                            if (!send)
                                            {
                                                printf("Send Ship Name and who I am to player %i!!\n", sba_Players[m]->ID);
                                                sba_Network->Send((char*)&up, size, sba_Players[m]->NetworkInformation, true);
                                                //Get the Data from the ship the player has
                                                sba::SBrickPacket bp;
                                                bp.Head.Type = sba::EPacket::Brick;
                                                bp.ID = p->ID;
                                                bp.BrickNum = p->Ship->m_pBricks.size();
                                                printf("Calculate Data for Bricks\n");
                                                for (unsigned int k = 0; k < p->Ship->m_pBricks.size(); k++)
                                                    bp.Bricks[k] = p->Ship->GetBrick(k);
                                                sba_Network->Send((char*)&bp, sizeof(sba::SBrickPacket), sba_Players[m]->NetworkInformation, true);
                                                printf("Send Data!\n");
                                                sendSockets.push_back(sba_Players[m]->NetworkInformation);
                                            }
                                        }
                                    }
                                    sendSockets.clear();
                                }
                                else
                                {
                                    sba::SWhoamIPacket wp;
                                    wp.Head.Type = sba::EPacket::WhoamI;
                                    wp.Pad = i;
                                    sba_Network->SendHost((char*)&wp, sizeof(sba::SWhoamIPacket), true);
                                    //asks the server who he is
                                }
                            }
                        }
                        else
                            sba_Space->CreatePlayer(i, a_pWindow);
                    }
                }
                if (sba_Network->IsConnected())
                    sba_Network->m_Mutex.unlock();
            }
            else if (a_pInput->GamepadPressed(a_pInput->Pad_B, i))
            {
                if (sba_Network->IsConnected())
                    sba_Network->m_Mutex.lock();

                if (!sba_Network->IsConnected() || sba_Network->IsConnected() && sba_Network->GetHost())
                {
                    int ID = -1;
                    //First get the ID and delete him
                    for (unsigned j = 0; j < sba_Players.size(); j++)
                    {
                        if (sba_Players[j]->PadID == i)
                        {
                            ID = sba_Players[j]->ID;
                            sba_Space->DeletePlayer(j);
                            break;
                        }
                    }
                    if (sba_Network->IsConnected() && ID != -1)
                    {
                        //we now have the ID and can send it to others
                        for (unsigned j = 0; j < sba_Players.size(); j++)
                        {
                            if (sba_Players[j]->PadID == -1)
                            {
                                //inform other players that one left
                                sba::SLeftPacket lp;
                                lp.Head.Type = sba::EPacket::Left;
                                lp.Who = ID;
                                sba_Network->Send((char*)&lp, sizeof(sba::SLeftPacket), sba_Players[j]->NetworkInformation, true);

                            }
                        }
                    }
                }
                else if (sba_Network->IsConnected() && !sba_Network->GetHost())
                {
                    int ID = -1;
                    //First get the ID
                    for (unsigned j = 0; j < sba_Players.size(); j++)
                    {
                        if (sba_Players[j]->PadID == i)
                        {
                            ID = sba_Players[j]->ID;
                            break;
                        }
                    }
                    if (ID != -1)
                    {
                        //now send that he left
                        sba::SLeftPacket lp;
                        lp.Head.Type = sba::EPacket::Left;
                        lp.Who = ID;
                        sba_Network->SendHost((char*)&lp, sizeof(sba::SLeftPacket), true);
                    }
                }

                if (sba_Network->IsConnected())
                    sba_Network->m_Mutex.unlock();
            }
        }

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
                if (sba_Network->IsConnected())
                    sba_Network->m_Mutex.lock();

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

                            if (sba_Network->IsConnected())
                            {
                                //send the Data to the Server
                                sba::SUserPacket up;
                                up.Head.Type = sba::EPacket::User;
                                up.ID = p->ID;
                                memset(up.Name, 0, sba::MaxLength + 1);
                                strncpy(up.Name, p->Ship->GetName().c_str(), p->Ship->GetName().size());
                                up.Name[p->Ship->GetName().length()] = '\0';
                                int nameSize = (sizeof(char)*sba::MaxLength);
                                //set data dynamic because we set our data self
                                int size = sizeof(up.Head.Type) + sizeof(up.ID) + nameSize;
                                //now send the packet to the host
                                printf("Send Ship Name and who I am %i!!\n", up.ID);
                                std::vector<SOCKET> sendSockets; //to who we already send it
                                //brick Package
                                sba::SBrickPacket bp;
                                bp.Head.Type = sba::EPacket::Brick;
                                bp.ID = p->ID;
                                bp.BrickNum = p->Ship->m_pBricks.size();
                                for (unsigned int l = 0; l < p->Ship->m_pBricks.size(); l++)
                                    bp.Bricks[l] = p->Ship->GetBrick(l);
                                if (sba_Network->GetHost())
                                {
                                    for (unsigned int m = 0; m < sba_Players.size(); m++)
                                    {
                                        //if host, only send if the player is not the one we received this Data from (because they can be split screen)
                                        if (sba_Players[m]->PadID == -1)
                                        {
                                            bool send = false;
                                            for (unsigned n = 0; n < sendSockets.size(); n++)
                                            {
                                                if (sba_Players[m]->NetworkInformation == sendSockets[n])
                                                {
                                                    send = true;
                                                    break;
                                                }
                                            }
                                            if (!send)
                                            {
                                                printf("Send User %i to %i\n", up.ID, sba_Players[m]->ID);
                                                sba_Network->Send((char*)&up, size, sba_Players[m]->NetworkInformation, true);
                                                printf("Send Bricks to %i\n", sba_Players[m]->ID);
                                                sba_Network->Send((char*)&bp, sizeof(sba::SBrickPacket), sba_Players[m]->NetworkInformation, true);
                                                sendSockets.push_back(sba_Players[m]->NetworkInformation);
                                            }
                                        }
                                    }
                                    sendSockets.clear();
                                }
                                else
                                {
                                    printf("Send User %i to Host\n", up.ID);
                                    sba_Network->SendHost((char*)&up, size, true);
                                    printf("Send Bricks to Host\n");
                                    sba_Network->SendHost((char*)&bp, sizeof(sba::SBrickPacket), true);
                                }
                            } //only for the network
                        }
                        break;
                    }
                }
                if (sba_Network->IsConnected())
                    sba_Network->m_Mutex.unlock();
            }
        }


        return 1;
    }

    void CLobby::Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Timer* a_pTimer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
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

        PuRe_Color color = PuRe_Color();

        if (this->m_pNavigation->GetFocusedElementId() == 0)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "START", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

        if (this->m_pNavigation->GetFocusedElementId() == 1)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        Position.Y -= 100.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "MAP: ", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);
        Position.X += 200.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 1 && this->m_Focus)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Map->GetName().c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

        Position.Y = 100.0f;
        Position.X = 100.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 2)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        Position.Y = 100.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "BACK", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 40.0f, color);

        Position.X = 1100;
        Position.Y = 1000;
        color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "PLAYER", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 30.0f, color);
        Position.X += 470.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "SHIP", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 30.0f, color);
        Position.Y -= 64.0f;
        Position.X -= 470.0f;
        color = PuRe_Color(1.0f, 1.0f, 1.0f);
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            std::string name = "PLAYER " + std::to_string(sba_Players[i]->ID + 1);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, name.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 26.0f, color);
            Position.X += 470.0f;
            if (sba_Players[i]->Ship != NULL)
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Players[i]->Ship->GetName(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 26.0f, color);
            Position.Y -= 64.0f;
            Position.X -= 470.0f;
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
            color.A = ((sin(a_pTimer->GetTotalElapsedSeconds()*2.0f)) + 0.8f) / 2.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "PRESS A TO JOIN", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 26.0f, color);
            Position.Y -= 64.0f;
        }

        if (sba_Network->IsConnected())
        {
            Position.X = 1920.0f - 500.0f;
            Position.Y = 1080.0f - 50.0f;
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "NAME: " + sba_Network->m_Name, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 26.0f, color);
            Position.Y -= 64.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "IP: " + sba_Network->m_IP, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 26.0f, color);
            Position.Y -= 64.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "PORT: " + sba_Network->m_Port, Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 26.0f, color);
        }
    }
}