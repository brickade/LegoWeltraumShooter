#include "include/Lobby_Network.h"
#include "include/NetworkHandler.h"
#include "include/Player.h"
#include "include/GameMap.h"
#include "include/BrickManager.h"

namespace sba
{

    CLobbyNetwork::CLobbyNetwork(PuRe_IWindow* a_pWindow, PuRe_Timer* a_pTimer) : m_pWindow(a_pWindow), m_pTimer(a_pTimer)
    {

    }
    // **************************************************************************
    // **************************************************************************
    CLobbyNetwork::~CLobbyNetwork()
    {
        this->Exit();
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::Start()
    {
        if (sba_Network->IsConnected())
        {
            this->m_Started = false;
            this->m_End = false;
            this->m_Running = true;
#ifdef NETWORKDEBUG
            printf("Running!\n");
            #endif
            if (sba_Network->GetHost())
            {
                std::thread* lThread = new std::thread(&CLobbyNetwork::Listen, this);
                this->m_Threads.push_back(lThread);
            }
            else
            {
                std::thread* rThread = new std::thread(&CLobbyNetwork::Receive, this, sba_Network->GetSocket());
                this->m_Threads.push_back(rThread);
            }
        }
    }


    // **************************************************************************
    // **************************************************************************
    bool CLobbyNetwork::IsEnded() const
    {
        return this->m_End;
    }


    // **************************************************************************
    // **************************************************************************
    bool CLobbyNetwork::IsStarted() const
    {
        return this->m_Started;
    }


    // **************************************************************************
    // **************************************************************************
    bool CLobbyNetwork::IsShip() const
    {
        return this->m_ShipSelect;
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::GoShip()
    {
        //send that everyone should go to ship select mode
        if (sba_Network->IsConnected())
        {
            this->m_ShipSelect = true;
            sba::SHeadPacket hp;
            hp.Type = sba::EPacket::Ship;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
                if (sba_Players[i]->PadID == -1)
                    sba_Network->Send((char*)&hp, sizeof(sba::SHeadPacket), sba_Players[i]->NetworkInformation, true);
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::GoBack()
    {
        //send that everyone should go to Menu mode
        if (sba_Network->IsConnected())
        {
            this->m_ShipSelect = false;
            sba::SHeadPacket hp;
            hp.Type = sba::EPacket::Menu;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
                if (sba_Players[i]->PadID == -1)
                    sba_Network->Send((char*)&hp, sizeof(sba::SHeadPacket), sba_Players[i]->NetworkInformation, true);
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::Exit()
    {
        if (sba_Network->IsConnected())
        {
            this->m_End = true;
            this->m_Running = false;
            if (!this->m_Started)
            {
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
#ifdef NETWORKDEBUG
                            printf("Left: %i", lp.Who);
                            #endif
                            sba_Network->SendHost((char*)&lp, sizeof(sba::SLeftPacket), true);
                        }
                    }
                }
            }
            for (unsigned int i = 0; i < this->m_Threads.size(); i++)
            {
                if (this->m_Threads[i]->joinable())
                    this->m_Threads[i]->join();
                SAFE_DELETE(this->m_Threads[i]);
            }
            this->m_Threads.clear();
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::SendUserData(sba::SPlayer* a_Player)
    {
        if(sba_Network->IsConnected())
        {
            sba_Network->m_Mutex.lock();
            //send the Data to the Server
            sba::SUserPacket up;
            up.Head.Type = sba::EPacket::User;
            up.ID = a_Player->ID;
            memset(up.Name, 0, sba::MaxName + 1);
            strncpy(up.Name, a_Player->Ship->GetName().c_str(), a_Player->Ship->GetName().size());
            up.Name[a_Player->Ship->GetName().length()] = '\0';
            int nameSize = (sizeof(char)*sba::MaxName);
            //set data dynamic because we set our data self
            int size = sizeof(up.Head.Type) + sizeof(up.ID) + nameSize;
            //now send the packet to the host
#ifdef NETWORKDEBUG
            printf("Send Ship Name and who I am %i!!\n", up.ID);
            #endif
            std::vector<SOCKET> sendSockets; //to who we already send it
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
#ifdef NETWORKDEBUG
                            printf("Send User %i to %i\n", up.ID, sba_Players[m]->ID);
                            #endif
                            sba_Network->Send((char*)&up, size, sba_Players[m]->NetworkInformation, true);
                        }
                    }
                }
                sendSockets.clear();
            }
            else
            {
#ifdef NETWORKDEBUG
                printf("Send User %i to Host\n", up.ID);
                #endif
                sba_Network->SendHost((char*)&up, size, true);
            }
            sba_Network->m_Mutex.unlock();
        } //only for the network
    }


    // **************************************************************************
    // **************************************************************************
    bool CLobbyNetwork::DeletePlayer(int a_Index)
    {
        if (sba_Network->IsConnected())
        {
            sba_Network->m_Mutex.lock();
            int ID = -1;
            //First get the ID
            for (unsigned j = 0; j < sba_Players.size(); j++)
            {
                if (sba_Players[j]->PadID == a_Index)
                {
                    ID = sba_Players[j]->ID;
                    break;
                }
            }
            //IF Host, send left to all players
            if (sba_Network->GetHost())
            {
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
                return true;
            }
            else 
            {
                //else send left to only host
                if (ID != -1)
                {
                    //now send that he left
                    sba::SLeftPacket lp;
                    lp.Head.Type = sba::EPacket::Left;
                    lp.Who = ID;
                    sba_Network->SendHost((char*)&lp, sizeof(sba::SLeftPacket), true);
                }
            }
            sba_Network->m_Mutex.unlock();
        }
        return false;
    }


    // **************************************************************************
    // **************************************************************************
    bool CLobbyNetwork::JoinPlayer(int a_Index)
    {
        if (sba_Network->IsConnected())
        {
            sba_Network->m_Mutex.lock();
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
                    sba_Space->CreatePlayer(a_Index, this->m_pWindow);
                    sba::SPlayer* p = sba_Players[sba_Players.size() - 1];
                    //we are the host, so create new players freely

                    //send the Data to the Server
                    sba::SUserPacket up;
                    up.Head.Type = sba::EPacket::User;
                    up.ID = p->ID;
                    int nameSize = (sizeof(char)*sba::MaxName);
                    memset(up.Name, 0, sba::MaxName + 1);
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
#ifdef NETWORKDEBUG
                                printf("Send Ship Name and who I am to player %i!!\n", sba_Players[m]->ID);
                                #endif
                                sba_Network->Send((char*)&up, size, sba_Players[m]->NetworkInformation, true);
                                //Get the Data from the ship the player has
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
                    wp.Pad = a_Index;
                    sba_Network->SendHost((char*)&wp, sizeof(sba::SWhoamIPacket), true);
                    //asks the server who he is
                }
            }
            sba_Network->m_Mutex.unlock();
            return true;
        }
        return false;
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::SendCommand()
    {
        //if hes in a Network, ask the players to send their Bricks
        if (sba_Network->IsConnected())
        {
            sba_Network->m_Mutex.lock();
            if (sba_Network->GetHost())
            {

                sba_Delay = 10;

#ifdef NETWORKDEBUG
                printf("Send the Bricks!!!\n");
                #endif
                sba::SPingPacket hp;
                hp.PTime = this->m_pTimer->GetTotalElapsedMilliseconds();
#ifdef NETWORKDEBUG
                printf("Send Delay Time %i\n",hp.PTime);
                #endif
                hp.Type = sba::EPacket::SendCommand;
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
                            sba_Network->Send((char*)&hp, sizeof(sba::SPingPacket), sba_Players[m]->NetworkInformation, true);
                            sendSockets.push_back(sba_Players[m]->NetworkInformation);

#ifdef NETWORKDEBUG
                            printf("Also send local Ships to him\n");
                            #endif
                            sba::SBrickPacket bp;
                            bp.Head.Type = sba::EPacket::Brick;
                            bp.PTime = 0;
                            //Go through all local players and send the ship
                            for (unsigned int k = 0; k < sba_Players.size(); k++)
                            {
                                if (sba_Players[k]->PadID != -1)
                                {
                                    bp.ID = sba_Players[k]->ID;
                                    bp.BrickNum = sba_Players[k]->Ship->m_pBricks.size();
                                    for (unsigned int l = 0; l < sba_Players[k]->Ship->m_pBricks.size(); l++)
                                        bp.Bricks[l] = sba_Players[k]->Ship->GetBrick(l);
                                    sba_Network->Send((char*)&bp, sizeof(sba::SBrickPacket), sba_Players[m]->NetworkInformation, true);
                                }
                            }
                        }
                    }
                }
                sendSockets.clear();
            }
            sba_Network->m_Mutex.unlock();
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::SendMap(std::string a_Map)
    {
        if (sba_Network->IsConnected())
        {
            sba::SMapPacket mp;
            memset(&mp, 0, sizeof(sba::SMapPacket));
            mp.Head.Type = sba::EPacket::Map;
            memcpy(mp.Name, a_Map.c_str(), sizeof(char)*a_Map.length());
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


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::Broadcast()
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
                if ((int)this->m_pTimer->GetTotalElapsedMilliseconds() % 2 == 0)
                    sba_Network->Broadcast((char*)&bp, sizeof(sba::SBroadcastPacket)); //Now Spam it!
            }
        }
    }


    // **************************************************************************
    // **************************************************************************
    bool CLobbyNetwork::CheckSend()
    {
        bool gotAll = true;
        //In Network, we check if we got all, and then send the Start
        if (sba_Network->IsConnected())
        {
            sba_Network->m_Mutex.lock();
            if (sba_Network->GetHost())
            {
                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    if (sba_Players[i]->Ship == NULL || sba_Players[i]->Ship->m_pBricks.size() == 0)
                    {
                        gotAll = false;
                        break;
                    }
                }
            }
            if (gotAll)
            {
                if (sba_Network->GetHost())
                {
#ifdef NETWORKDEBUG
                    printf("Game Start!\n");
                    #endif
                    sba::SStartPacket hp;
                    hp.Delay = sba_Delay;
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
                                sba_Network->Send((char*)&hp, sizeof(sba::SStartPacket), sba_Players[m]->NetworkInformation, true);
                                sendSockets.push_back(sba_Players[m]->NetworkInformation);
                            }
                        }
                    }
                    sendSockets.clear();
                }
            }
            sba_Network->m_Mutex.unlock();
        }
        return gotAll;
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::Listen()
    {
#ifdef NETWORKDEBUG
        printf("Thread Start LISTEN!\n");
        #endif
        sba_Network->SetBlockMode(sba_Network->GetSocket(), false);
        const int len = 1024;
        char buffer[len];
        while (this->m_Running)
        {
            memset(buffer, 0, len);
            if (sba_Network->Listen())
            {
                if (sba_Players.size() < sba::MaxPlayers)
                {
                    SOCKET s = sba_Network->Accept();
                    if (!this->m_Running)
                        break;
                    if (s > 0)
                    {
                        sba_Network->m_Mutex.lock();
#ifdef NETWORKDEBUG
                        printf("Client connected!\n");
                        #endif
                        int ID = 0; // 0 is Host
                        for (unsigned int i = 0; i < sba_Players.size(); i++)
                        {
                            if (ID == sba_Players[i]->ID)
                            {
                                ID++;
                                i = 0;
                            }
                        }
#ifdef NETWORKDEBUG
                        printf("User %i joined!\n", ID);
                        printf("Send user all other Users\n");
                        #endif
                        for (unsigned i = 0; i < sba_Players.size(); i++)
                        {
#ifdef NETWORKDEBUG
                            printf("Send User %i!\n", sba_Players[i]->ID);
                            #endif
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
#ifdef NETWORKDEBUG
                            printf("Send Ship Name and his ID %i!!\n", up.ID);
                            #endif
                            sba_Network->Send((char*)&up, size, s, true);
                        }
#ifdef NETWORKDEBUG
                        printf("Now Send IAM Packet!\n");
                        #endif
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
#ifdef NETWORKDEBUG
                        printf("IAm Packet send to ID %i!\n", ID);
                        #endif
                        std::thread* rThread = new std::thread(&CLobbyNetwork::Receive, this, s);
                        this->m_Threads.push_back(rThread);
                        sba_Network->m_Mutex.unlock();
                    }
                }
            }
        }
#ifdef NETWORKDEBUG
        printf("Thread End LISTEN!\n");
        #endif
    }


    // **************************************************************************
    // **************************************************************************
    void CLobbyNetwork::Receive(SOCKET s)
    {
#ifdef NETWORKDEBUG
        printf("Thread Start LOBBY!\n");
        #endif
        sba_Network->SetBlockMode(s, false);

        const int len = 16384;
        char buffer[len];
        char buffer2[len];
        memset(buffer, 0, len);
        memset(buffer2, 0, len);
        int packets = 0;
        int leftData = 0;

        bool locked = false;
        long packetSize = 0;
        while (this->m_Running)
        {

            long dataLeft = sba_Network->Receive(buffer, len, s, false);
            if (!this->m_Running)
                break;
            if (dataLeft > 0)
            {
                //we received something, copy received into our buffer + offset
                memcpy(buffer2 + leftData, buffer, dataLeft);
                leftData += dataLeft;

                sba_Network->m_Mutex.lock();
#ifdef NETWORKDEBUG
                printf("Mutex Lock RD!\n");
                #endif
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

#ifdef NETWORKDEBUG
                            printf("Deleted Player %i!\n", sleft.Who);
                            #endif
                            sba_Space->DeletePlayer(i);
                            --i;
                        }
                    }
                }
                else
                {
                    this->m_Running = false;
                    this->m_End = true;
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
#ifdef NETWORKDEBUG
                    printf("Map changed to %s!\n", smap->Name);
                    #endif
                    sba_Map->SetMap(smap->Name);
                    packetSize = sizeof(sba::SMapPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Left&&leftData >= sizeof(sba::SLeftPacket))
                {
                    sba::SLeftPacket* sleft = (sba::SLeftPacket*)rpacket;
#ifdef NETWORKDEBUG
                    printf("%i left\n", sleft->Who);
                    #endif
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Network->GetHost() && sba_Players[i]->PadID == -1) //send to all that are not local
                            sba_Network->Send((char*)sleft, sizeof(sba::SLeftPacket), sba_Players[i]->NetworkInformation, true);
                        if (sleft->Who == sba_Players[i]->ID) //client and host sees this
                        {
#ifdef NETWORKDEBUG
                            printf("Deleted Player %i!\n", sleft->Who);
                            #endif
                            sba_Space->DeletePlayer(i);
                            --i;
                        }
                    }
                    packetSize = sizeof(sba::SLeftPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::SendCommand&&leftData >= sizeof(sba::SPingPacket))
                {
#ifdef NETWORKDEBUG
                    printf("Received a Command that we should send our Ship\n");
                    #endif
                    sba::SPingPacket* rpacket = (sba::SPingPacket*)buffer2;
                    sba::SBrickPacket bp;
                    bp.Head.Type = sba::EPacket::Brick;
                    bp.PTime = rpacket->PTime;

#ifdef NETWORKDEBUG
                    printf("Got Delay Time %i\n", bp.PTime);
                    #endif
                    //Go through all local players and send the ship
                    for (unsigned int m = 0; m < sba_Players.size(); m++)
                    {
                        if (sba_Players[m]->PadID != -1)
                        {
                            bp.ID = sba_Players[m]->ID;
                            bp.BrickNum = sba_Players[m]->Ship->m_pBricks.size();
                            for (unsigned int i = 0; i < sba_Players[m]->Ship->m_pBricks.size(); i++)
                                bp.Bricks[i] = sba_Players[m]->Ship->GetBrick(i);
                            sba_Network->SendHost((char*)&bp, sizeof(sba::SBrickPacket), true);
                        }
                    }

                    packetSize = sizeof(sba::SPingPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Menu&&leftData >= sizeof(sba::SHeadPacket))
                {
#ifdef NETWORKDEBUG
                    printf("GoTo Menu\n");
                    #endif
                    this->m_ShipSelect = false;
                    packetSize = sizeof(sba::SHeadPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Ship&&leftData >= sizeof(sba::SHeadPacket))
                {
#ifdef NETWORKDEBUG
                    printf("GoTo Ship Selection\n");
                    #endif
                    this->m_ShipSelect = true;
                    packetSize = sizeof(sba::SHeadPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Start&&leftData >= sizeof(sba::SStartPacket))
                {
                    sba::SStartPacket* spackets = (sba::SStartPacket*)rpacket;
                    sba_Delay = spackets->Delay;
#ifdef NETWORKDEBUG
                    printf("GAME START\n");
                    printf("Delay: %i\n",sba_Delay);
                    #endif
                    this->m_Running = false;
                    this->m_Started = true;
                    packetSize = sizeof(sba::SStartPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::LobbyEnd&&leftData >= sizeof(sba::SHeadPacket))
                {
#ifdef NETWORKDEBUG
                    printf("GAME END\n");
                    #endif
                    this->m_Running = false;
                    this->m_End = true;
                    packetSize = sizeof(sba::SHeadPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::WhoamI&&leftData >= sizeof(sba::SWhoamIPacket))
                {
                    sba::SWhoamIPacket* wpackets = (sba::SWhoamIPacket*)rpacket;
#ifdef NETWORKDEBUG
                    //only received by the player, when a client localy pressed a
                    printf("Client connected!\n");
                    #endif
                    int ID = 0; // 0 is Host
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (ID == sba_Players[i]->ID)
                        {
                            ID++;
                            i = 0;
                        }
                    }
#ifdef NETWORKDEBUG
                    printf("Client created\n");
                    #endif
                    sba::SPlayer* p = new sba::SPlayer();
                    p->ID = ID;
                    p->NetworkInformation = s;
                    p->PadID = -1;
                    p->ShipID = -1;
                    p->Ship = NULL;
                    sba_Players.push_back(p);
#ifdef NETWORKDEBUG
                    printf("User %i joined remote locally!\n", ID);
                    #endif
                    //tell the user who he is
                    sba::SIamPacket iam;
                    iam.Head.Type = sba::EPacket::IAm;
                    iam.ID = ID;
                    iam.Pad = wpackets->Pad;
                    memcpy(iam.Map, sba_Map->GetName().c_str(), sba::MaxName);
#ifdef NETWORKDEBUG
                    printf("Send Client to Clients\n");
                    #endif
                    sba_Network->Send((char*)&iam, sizeof(sba::SIamPacket), s, true);
                    packetSize = sizeof(sba::SWhoamIPacket);
                }
                else if (rpacket->Head.Type == sba::EPacket::Brick&&leftData >= sizeof(sba::SBrickPacket))
                {
#ifdef NETWORKDEBUG
                    printf("Got Bricks\n");
                    #endif
                    sba::SBrickPacket* bp = (sba::SBrickPacket*)rpacket;

                    if (sba_Network->GetHost())
                    {
#ifdef NETWORKDEBUG
                        printf("Timer: %i\n", this->m_pTimer->GetTotalElapsedMilliseconds());
                        printf("GotTimer: %i\n", bp->PTime);
                        #endif
                        int diff = (int)(this->m_pTimer->GetTotalElapsedMilliseconds() - bp->PTime);
#ifdef NETWORKDEBUG
                        printf("Diff: %i\n", diff);
                        #endif
                        if (diff > sba_Delay)
                            sba_Delay = diff;
#ifdef NETWORKDEBUG
                        printf("Delay: %i\n",sba_Delay);
                        #endif
                    }


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
#ifdef NETWORKDEBUG
                                    printf("Send Bricks to Clients!\n");
                                    #endif
                                    sba_Network->Send((char*)bp, sizeof(sba::SBrickPacket), sba_Players[i]->NetworkInformation, true);
                                    sendSockets.push_back(sba_Players[i]->NetworkInformation);

                                }

                            }
                        }
                        if (sba_Players[i]->ID == bp->ID)
                        {
#ifdef NETWORKDEBUG
                            printf("Updating ID %i\n", bp->ID);
                            #endif
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
#ifdef NETWORKDEBUG
                    printf("Receiving Client Data %i!\n", up->ID);
                    #endif
                    int nameSize = (sizeof(char)*sba::MaxName);
                    int size = sizeof(up->Head.Type) + sizeof(up->ID) + nameSize;
                    up->Name[sba::MaxName + 1] = '\0';
                    //existing clients
                    std::vector<SOCKET> sendSockets; //to who we already send it
                    sendSockets.push_back(s);
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Network->GetHost())
                        {
                            //if host, only send if the player is not the one we received this Data from (because they can be split screen)
#ifdef NETWORKDEBUG
                            printf("Try User %i to %i\n", up->ID, sba_Players[i]->ID);
                            printf("S1 %i S2 %i\n", s, sba_Players[i]->NetworkInformation);
                            #endif
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
#ifdef NETWORKDEBUG
                                    printf("Send User %i to %i\n", up->ID, sba_Players[i]->ID);
                                    #endif
                                    sba_Network->Send((char*)up, size, sba_Players[i]->NetworkInformation, true);
                                    sendSockets.push_back(sba_Players[i]->NetworkInformation);
                                }
                            }
                        }
                        if (up->ID == sba_Players[i]->ID)
                        {
#ifdef NETWORKDEBUG
                            printf("Created ship %s for player %i!\n", up->Name, up->ID);
                            #endif
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
#ifdef NETWORKDEBUG
                        printf("User %i joined!\n", up->ID);
                        #endif
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

#ifdef NETWORKDEBUG
                    printf("Got who I am: %i!\n", iam->ID);
                    #endif
                    sba::SPlayer* p;
                    sba_Space->CreatePlayer(iam->Pad, this->m_pWindow);
                    p = sba_Players[sba_Players.size() - 1];
                    p->ID = iam->ID;
                    //send the Data to the Server
                    sba::SUserPacket up;
                    up.Head.Type = sba::EPacket::User;
                    up.ID = iam->ID;
                    memset(up.Name, 0, sba::MaxName + 1);
                    strncpy(up.Name, p->Ship->GetName().c_str(), p->Ship->GetName().size());
                    up.Name[p->Ship->GetName().length()] = '\0';
                    int nameSize = (sizeof(char)*sba::MaxName);
                    //set data dynamic because we set our data self
                    int size = sizeof(up.Head.Type) + sizeof(up.ID) + nameSize;
                    //now send the packet to the host
#ifdef NETWORKDEBUG
                    printf("Send Ship Name and who I am %i!!\n", up.ID);
                    #endif
                    sba_Network->SendHost((char*)&up, size, true);
                    packetSize = sizeof(sba::SIamPacket);
                }

                if (packetSize == 0) //our excepted Data is not full there, so break and receive more
                {
                    break;
                }
                //we handeld our packet, so remove it from the buffer
#ifdef NETWORKDEBUG
                printf("Data we had before: %i\n", leftData);
                #endif
                leftData -= packetSize;
#ifdef NETWORKDEBUG
                printf("Data left: %i\n", leftData);
                #endif
                memcpy(buffer2, buffer2 + (int)packetSize, len - packetSize);
            }
            if (locked)
            {
                locked = false;
#ifdef NETWORKDEBUG
                printf("Mutex UnLock RD!\n");
                #endif
                memset(buffer, 0, len);
                sba_Network->m_Mutex.unlock();
            }
        }
#ifdef NETWORKDEBUG
        printf("Thread End LOBBY!\n");
        #endif
    }

}