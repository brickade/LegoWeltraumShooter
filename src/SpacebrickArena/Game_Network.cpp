#include "include/Game_Network.h"
#include "include/BrickManager.h"

namespace sba
{
    CGameNetwork::CGameNetwork()
    {
    }

    // **************************************************************************
    // **************************************************************************
    CGameNetwork::~CGameNetwork()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CGameNetwork::Initialize()
    {
        this->m_PhysicTime = 0.0f;
        this->m_PhysicFrame = 0;
        this->m_Run = true;
        memset(m_buffer, -1, sizeof(PlayOutBuffer) * BufferSize);
        printf("set block mode true\n");
        if (sba_Network->GetHost())
        {
            sba_Network->m_Mutex.lock();
            printf("\n");
            printf("Resetting Buffer ... \n");
            for (int i = 0; i < BufferSize; ++i)
            {
                m_buffer[i].Frame = i;
                memset(m_buffer[i].Inputs, 0, sizeof(sba::SInputData) * sba::MaxPlayers);
                memset(&m_numGot[i].Player, 0, sizeof(bool)* sba::MaxPlayers);
                m_send[i] = 0;
                m_numReceived[i] = 0;
            }
            printf("Buffer Reset!\n");
            //send 6 frames from self, because server is also a player
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                if (sba_Players[i]->PadID != -1)
                {
                    for (int j = 0; j < sba_Delay; j++)
                    {
                        memset(&m_buffer[j - m_PhysicFrame].Inputs[sba_Players[i]->ID], 0, sizeof(sba::SInputData));
                        m_numReceived[j - m_PhysicFrame]++;
                        m_numGot[j - m_PhysicFrame].Player[sba_Players[i]->ID] = true;
                    }
                }
                else
                {
                    std::thread rthread(&CGameNetwork::Receive, this, sba_Players[i]->NetworkInformation);
                    rthread.detach();
                }
            }
            printf("Send init to all.\n");
            sba::SHeadPacket sh;
            sh.Type = sba::EPacket::Init;
            std::vector<SOCKET> sendSockets;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                if (sba_Players[i]->PadID == -1)
                {
                    bool send = false;
                    for (unsigned int n = 0; n < sendSockets.size(); n++)
                    {
                        if (sba_Players[i]->NetworkInformation == sendSockets[n])
                        {
                            send = true;
                            break;
                        }
                    }
                    if (!send)
                    {
                        sba_Network->Send((char*)&sh, sizeof(sba::SHeadPacket), sba_Players[i]->NetworkInformation, false);
                        sendSockets.push_back(sba_Players[i]->NetworkInformation);
                    }
                }
            }
            sendSockets.clear();
            sba_Network->m_Mutex.unlock();
            printf("Set Initial Input for Host.\n");

        }
        else
        {
            std::thread rthread(&CGameNetwork::Receive, this, sba_Network->GetSocket());
            rthread.detach();
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CGameNetwork::UpdateNetwork(std::vector<SItemData>& a_rRespawnItems, std::vector<CAsteroid*>& a_rAsteroids, std::vector<ExplosionEmitter>& a_rExplosions, std::vector<CBullet*>& a_rBullets, std::vector<CItem*>& a_rItems, PuRe_IInput* a_pInput, float a_DeltaTime, float& a_rEndTime, hInput hInput, pInput pInput, gUpdate gUpdate)
    {
        sba_Network->m_Mutex.lock();
        if (sba_Network->GetHost())
        {
            for (int i = 0; i < BufferSize; ++i)
            {
                for (unsigned int j = 0; j < sba_Players.size(); j++)
                {
                    if (!this->m_numGot[i].Player[j] && sba_Players[j]->Timeout)
                    {
                        m_numReceived[i]++;
                        this->m_numGot[i].Player[sba_Players[j]->ID] = true;
                    }
                }
                sba::SInputsPacket packet;
                //if all player send something
                if (!m_send[i] && m_numReceived[i] >= sba_Players.size())
                {
                    packet.Head.Type = sba::EPacket::CTick;
                    packet.Frame = m_buffer[i].Frame;
                    packet.Players = sba_Players.size();
                    memcpy(packet.Input, m_buffer[i].Inputs, sizeof(sba::SInputData)*sba_Players.size());
                    //send tick with input
                    printf("send tick %d\n", m_buffer[i].Frame);
                    std::vector<SOCKET> sendSockets;
                    for (unsigned int j = 0; j < sba_Players.size(); ++j)
                    {
                        if (!sba_Players[j]->Timeout&&sba_Players[j]->PadID == -1)
                        {
                            bool send = false;
                            for (unsigned int n = 0; n < sendSockets.size(); n++)
                            {
                                if (sba_Players[j]->NetworkInformation == sendSockets[n])
                                {
                                    send = true;
                                    break;
                                }
                            }
                            if (!send)
                            {
                                sba_Network->Send((char*)&packet, sizeof(sba::SInputsPacket), sba_Players[j]->NetworkInformation, false);
                                sendSockets.push_back(sba_Players[j]->NetworkInformation);
                            }
                        }
                    }
                    sendSockets.clear();
                    m_send[i] = 1;
                }
            }
        }

        m_PhysicTime += a_DeltaTime;
        bool inputExists = true;
        while (m_PhysicTime >= 1.0f / 60.0f && inputExists)
        {
            if (sba_Network->GetHost())
            {
                inputExists = m_send[0] == 1;
            }
            else
                inputExists = m_buffer[0].Frame == this->m_PhysicFrame;

            if (inputExists)
            {
                this->m_Timeout = 0;
                printf("Input exists, do Physik for Frame %i!\n", this->m_PhysicFrame);
                sba::SInputPacket ipacket;
                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    if (sba_Players[i]->PadID != -1)
                    {
                        memset(&ipacket, 0, sizeof(sba::SInputPacket));
                        ipacket.Head.Type = sba::EPacket::STick;
                        ipacket.Frame = this->m_PhysicFrame + sba_Delay;
                        ipacket.Input = hInput(sba_Players[i]->PadID,a_pInput);
                        ipacket.Input.Player = sba_Players[i]->ID;

                        if (!sba_Network->GetHost())
                        {
                            printf("send package %d\n", ipacket.Frame);
                            sba_Network->SendHost((char*)&ipacket, sizeof(sba::SInputPacket), false);
                        }
                        else
                        {
                            printf("Handle own Input %d\n", ipacket.Frame);
                            m_buffer[ipacket.Frame - m_PhysicFrame].Inputs[sba_Players[i]->ID] = ipacket.Input;
                            m_numReceived[ipacket.Frame - m_PhysicFrame]++;
                            m_numGot[ipacket.Frame - m_PhysicFrame].Player[sba_Players[i]->ID] = true;
                        }
                    }
                }

                //Now handle input

                PlayOutBuffer* buffer = &this->m_buffer[0];

                sba::SInputData* input;
                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    unsigned int id = buffer->Inputs[i].Player;
                    for (unsigned int j = 0; j < sba_Players.size(); j++)
                    {
                        if (sba_Players[j]->ID == id)
                        {
                            input = &buffer->Inputs[i];
                            pInput(a_rBullets,sba_Players[j], input, 1 / 60.0f,this->m_PhysicFrame);
                            break;
                        }
                    }
                }

                memcpy(m_buffer, m_buffer + 1, sizeof(PlayOutBuffer) * BufferSize - 1);
                if (sba_Network->GetHost())
                {
                    memcpy(m_numReceived, m_numReceived + 1, sizeof(unsigned int) * BufferSize - 1);
                    memcpy(m_send, m_send + 1, sizeof(bool) * BufferSize - 1);
                    memcpy(m_numGot, m_numGot + 1, sizeof(GotBuffer) * BufferSize - 1);
                    memset(m_numGot[BufferSize - 1].Player, 0, sizeof(bool)*sba::MaxPlayers);
                    m_numReceived[BufferSize - 1] = 0;
                    m_send[BufferSize - 1] = 0;
                    m_buffer[BufferSize - 1].Frame = this->m_PhysicFrame + BufferSize;
                }

                sba::Space::Instance()->World->step(1 / 60.0f);
                this->m_PhysicTime -= 1.0f / 60.0f;
                printf("Physic: %i\n", this->m_PhysicFrame);
                this->m_PhysicFrame++;
                a_rEndTime -= 1.0f / 60.0f;
                assert(this->m_PhysicFrame != 2147483647);


                gUpdate(a_rRespawnItems,a_rAsteroids,a_rExplosions, a_rBullets, a_rItems, 1.0f / 60.0f, this->m_PhysicFrame);

                sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances

            } //if input exists

        } //while physic run



        if (sba_Network->GetHost())
        {
            //Kein Physic Frame seit 5 Sekunden
            if (this->m_Timeout > 5.0f)
            {
                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    if (!m_numGot[0].Player[i])
                        sba_Players[i]->Timeout = true;
                }
            }
        }

        sba_Network->m_Mutex.unlock();
        return this->m_Run;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameNetwork::Receive(SOCKET s)
    {
        sba_Network->SetBlockMode(s, true);
        printf("Start Thread!\n");
        const int bufferSize = 1024;
        char buffer[bufferSize];
        while (this->m_Run)
        {
            memset(buffer, 0, bufferSize);
            long dataLeft = sba_Network->Receive(buffer, bufferSize, s, false);
            if (dataLeft == -1)
            {
                printf("Connection lost\n");
                this->m_Run = false;
                break;
            }
            sba_Network->m_Mutex.lock();
            while (dataLeft > 0)
            {
                printf("Data left %d!\n", dataLeft);
                sba::SReceivePacket* Packet = (sba::SReceivePacket*)buffer;
                long packetSize = 4;
                if (Packet->Head.Type == sba::EPacket::Init)
                {
                    printf("Received INIT %l!\n", dataLeft);
                    sba::SInputPacket package;
                    memset(&package, 0, sizeof(sba::SInputPacket));
                    package.Head.Type = sba::EPacket::STick;
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Players[i]->PadID != -1)
                        {
                            //send first 6 frames
                            package.Input.Player = sba_Players[i]->ID;
                            printf("Send initial Data!\n");
                            for (int j = 0; j < sba_Delay; ++j)
                            {
                                package.Frame = j;
                                sba_Network->SendHost((char*)&package, sizeof(sba::SInputPacket), false);
                            }
                        }
                    }
                    packetSize = sizeof(sba::SHeadPacket);
                }
                if (Packet->Head.Type == sba::EPacket::STick)
                {
                    sba::SInputPacket* IPacket = (sba::SInputPacket*)Packet;
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (!sba_Players[i]->Timeout&&sba_Players[i]->ID == IPacket->Input.Player)
                        {
                            m_buffer[IPacket->Frame - m_PhysicFrame].Inputs[IPacket->Input.Player] = IPacket->Input;
                            m_numReceived[IPacket->Frame - m_PhysicFrame]++;
                            m_numGot[IPacket->Frame - m_PhysicFrame].Player[IPacket->Input.Player] = true;
                            printf("received tick %d from player %d\n", IPacket->Frame, IPacket->Input.Player);
                            break;
                        }
                    }
                    //ship left
                    packetSize = sizeof(sba::SInputPacket);
                }
                else if (Packet->Head.Type == sba::EPacket::CTick)
                {
                    sba::SInputsPacket* IPacket = (sba::SInputsPacket*)Packet;
                    PlayOutBuffer* pbuffer = &this->m_buffer[IPacket->Frame - this->m_PhysicFrame];
                    pbuffer->Frame = IPacket->Frame;
                    memcpy(pbuffer->Inputs, IPacket->Input, sizeof(sba::SInputData)*IPacket->Players);
                    printf("received tick %d\n", IPacket->Frame);
                    //ship left
                    packetSize = sizeof(sba::SInputsPacket);
                }
                dataLeft -= packetSize;
                memcpy(buffer, buffer + (int)packetSize, bufferSize - packetSize);
            }
            sba_Network->m_Mutex.unlock();
        }
        printf("End Thread!\n");
        
    }

    // **************************************************************************
    // **************************************************************************
    void CGameNetwork::Update(float a_DeltaTime)
    {
        m_Timeout += a_DeltaTime;
    }
}