#include "include/GameScene.h"

#include "TheBrick/Conversion.h"

namespace Game
{

    // **************************************************************************
    // **************************************************************************
    void CGameScene::ReceiveData(SOCKET s)
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
                    printf("Received INIT!\n", dataLeft);
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
                            for (int j = 0; j < sba::Delay; ++j)
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
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx, bool a_Network)
    {
        this->m_pApplication = a_pApplication;
        this->m_Test = -1;
    }

    // **************************************************************************
    // **************************************************************************
    sba::SInputData CGameScene::HandleInput(int a_PlayerIdx)
    {
        PuRe_IInput* aInput = this->m_pApplication->GetInput();

        sba::SInputData input;
        memset(&input, 0, sizeof(sba::SInputData));

        if (aInput->GamepadPressed(aInput->Pad_A, a_PlayerIdx))
            input.Shoot = true;

        PuRe_Vector2F Move = aInput->GetGamepadLeftThumb(a_PlayerIdx);
        if (Move.X > 0.5f)
            input.MoveX = 1;
        else if (Move.X < -0.5f)
            input.MoveX = 2;
        if (Move.Y > 0.5f)
            input.MoveY = 1;
        else if (Move.Y < -0.5f)
            input.MoveY = 2;

        float Thrust = aInput->GetGamepadRightTrigger(a_PlayerIdx);
        if (Thrust > 0.2f)
            input.Thrust = true;

        if (aInput->GamepadIsPressed(aInput->Left_Shoulder, a_PlayerIdx))
            input.Spin = 2;
        else if (aInput->GamepadIsPressed(aInput->Right_Shoulder, a_PlayerIdx))
            input.Spin = 1;

        return input;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::ProcessInput(TheBrick::CSpaceship* a_Ship, sba::SInputData* a_Input, float a_DeltaTime)
    {
        if (a_Input->Shoot)
            a_Ship->Shoot(this->m_Bullets);

        if (a_Input->Thrust)
            a_Ship->Thrust(1.0f);

        PuRe_Vector2F Move = PuRe_Vector2F(0.0f, 0.0f);
        if (a_Input->MoveX == 1)
            Move.X = 1.0f;
        else if (a_Input->MoveX == 2)
            Move.X = -1.0f;
        if (a_Input->MoveY == 1)
            Move.Y = 1.0f;
        else if (a_Input->MoveY == 2)
            Move.Y = -1.0f;
        if (Move.Length() > 0.2f)
            a_Ship->Move(Move);

        if (a_Input->Spin == 1)
            a_Ship->Spin(1.0f);
        else if (a_Input->Spin == 2)
            a_Ship->Spin(-1.0f);

        a_Ship->Update(a_DeltaTime);
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::HandleLocal()
    {
        PuRe_IInput* aInput = this->m_pApplication->GetInput();
        PuRe_Timer* aTimer = this->m_pApplication->GetTimer();
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            sba::SInputData input = this->HandleInput(sba_Players[i]->PadID);
            this->ProcessInput(sba_Players[i]->Ship, &input, aTimer->GetElapsedSeconds());
        }
        sba::Space::Instance()->UpdatePhysics(aTimer);
        this->m_TimeLimit -= aTimer->GetElapsedSeconds();
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::HandleNetwork()
    {
        PuRe_IInput* aInput = this->m_pApplication->GetInput();
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

        m_PhysicTime += this->m_pApplication->GetTimer()->GetElapsedSeconds();
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
                printf("Input exists, do Physik for Frame %i!\n",this->m_PhysicFrame);
                sba::SInputPacket ipacket;
                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    if (sba_Players[i]->PadID != -1)
                    {
                        memset(&ipacket, 0, sizeof(sba::SInputPacket));
                        ipacket.Head.Type = sba::EPacket::STick;
                        ipacket.Frame = this->m_PhysicFrame + sba::Delay;
                        ipacket.Input = this->HandleInput(sba_Players[i]->PadID);
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
                            this->ProcessInput(sba_Players[j]->Ship,input,1/60.0f);
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
                this->m_TimeLimit -= 1.0f/60.0f;
                assert(this->m_PhysicFrame != 2147483647);
                sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances
            } //if input exists

        } //while physic run



        if (sba_Network->GetHost())
        {
            //Kein Physic Frame seit 4 Sekunden
            if (this->m_Timeout > 4.0f)
            {
                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    if (!m_numGot[0].Player[i])
                        sba_Players[i]->Timeout = true;
                }
            }
        }

        sba_Network->m_Mutex.unlock();
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::StartGame()
    {
        if (sba_Network->IsConnected())
        {
            this->m_PhysicTime = 0.0f;
            this->m_PhysicFrame = 0;
            memset(m_buffer, -1, sizeof(PlayOutBuffer) * BufferSize);
            printf("set block mode true\n");
            this->m_Run = true;
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
                        for (int j = 0; j < sba::Delay; j++)
                        {
                            memset(&m_buffer[j - m_PhysicFrame].Inputs[sba_Players[i]->ID], 0, sizeof(sba::SInputData));
                            m_numReceived[j - m_PhysicFrame]++;
                            m_numGot[j - m_PhysicFrame].Player[sba_Players[i]->ID] = true;
                        }
                    }
                    else
                    {
                        std::thread rthread(&CGameScene::ReceiveData, this, sba_Players[i]->NetworkInformation);
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
                std::thread rthread(&CGameScene::ReceiveData, this, sba_Network->GetSocket());
                rthread.detach();
            }
        }

        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            sba_Players[i]->Ship->CalculateData();
            ong::vec3 pos = ong::vec3(10.0f, 10.0f, 10.0f);
            pos.x += sba_Players[i]->ID*10.0f;
            sba_Players[i]->Ship->m_pBody->setPosition(pos);
        }
        ong::vec3 start(50.0f, 50.0f, 50.0f);
        for (int i = 0; i < 10; i++)
        {
            TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(*sba_World, start + ong::vec3((i % 4)*10.0f, ((i * 5) % 2)*2.0f, i*5.0f));
            TheBrick::CSerializer serializer;
            serializer.OpenRead("../data/ships/asteroid.object");
            asteroid->Deserialize(serializer, sba_BrickManager->GetBrickArray(), *sba_World);
            this->m_Asteroids.push_back(asteroid);
            serializer.Close();
        }
        this->gameStart = true;
        this->m_TimeLimit = 60.0f*10.0f; //seconds * Minutes
        sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();
        this->m_LocalPlayers = 0;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
                this->m_LocalPlayers++;
        }
        //Camera
        sba_Renderer->DeleteTargets();
        PuRe_Vector2I size = PuRe_Vector2I(gdesc.ResolutionWidth, gdesc.ResolutionHeight);
        PuRe_Vector2F fsize = PuRe_Vector2F((float)1920, (float)1080);
        if (this->m_LocalPlayers == 2)
            size.X /= 2;
        else if (this->m_LocalPlayers > 2)
        {
            size.X /= 2;
            size.Y /= 2;
        }
        sba_Renderer->AddTarget(size);
        sba_Renderer->AddTarget(size);
        sba_Renderer->AddTarget(PuRe_Vector2I(gdesc.ResolutionWidth, gdesc.ResolutionHeight));
        if (sba::CIniReader::Instance()->GetValue("SSAO") == "On")
        {
            sba_Renderer->SetSSAO(0, sba_Space->m_SSAOMaterial, sba_Space->m_pNoiseTexture);
        }



        this->m_pFontMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/font/default");
        this->m_pUIMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/UI/default");
        this->m_pPostMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/Post/default");
        this->m_pSkyMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/skybox/default");
        this->m_pPointLightMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/PointLight/default");
        this->m_pDirectionalLightMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/DirectionalLight/default");
        this->m_pParticleMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/particles/default");
        this->m_pPointLight = new PuRe_PointLight(a_pApplication->GetGraphics());
        this->m_pDirectionalLight = new PuRe_DirectionalLight(a_pApplication->GetGraphics());
        this->m_pMinimap = new CMinimap(a_pApplication->GetGraphics());
        this->m_pFont = new PuRe_Font(a_pApplication->GetGraphics(), "../data/textures/font.png");
        this->m_pParticleSprite = new PuRe_Sprite(a_pApplication->GetGraphics(),"../data/textures/dust.png");
        this->m_pUICam = new PuRe_Camera(fsize, PuRe_CameraProjection::Orthogonal);
        //Create for each player a camera
        for (int i = 0; i<this->m_LocalPlayers; i++)
        {
            CGameCamera* Cam = new CGameCamera(fsize, PuRe_Camera_Perspective);
            Cam->Initialize();
            this->m_Cameras.push_back(Cam);
            PuRe_ParticleEmitter* emitter = new PuRe_ParticleEmitter(PuRe_Vector3F(), PuRe_QuaternionF());
            this->m_Emitters.push_back(emitter);
        }

        if (this->m_LocalPlayers == 3)
        {
            CGameCamera* Cam = new CGameCamera(fsize, PuRe_Camera_Perspective);
            Cam->Initialize();
            this->m_Cameras.push_back(Cam);
        }

        this->m_TextureID = 0;

        #ifdef Skybox
            this->m_pSkyBox = new PuRe_SkyBox(a_pApplication->GetGraphics(), "../data/textures/cube/");
        #endif

        this->StartGame();

    }

    // **************************************************************************
    // **************************************************************************
    int CGameScene::Update(PuRe_Application* a_pApplication)
    {
        PuRe_Timer* timer = a_pApplication->GetTimer();
        m_Timeout += timer->GetElapsedSeconds();

        //Handle ESC Button
        if (this->m_Timeout+10.0f < 0.0f||a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC))
        {
            if (sba_Network->IsConnected())
            {
                this->m_Run = false;
                SAFE_DELETE(sba_Network);
                sba_Network = new sba::CNetworkHandler();
            }
            return 5;
        }


        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->Left))
        {
            //this->m_TextureID--;
            //if (this->m_TextureID < 0)
            //    this->m_TextureID = 4;
            this->m_Test--;
            if (this->m_Test < 0)
                this->m_Test = sba_Players.size()-1;
        }

        else if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->Right))
        {
            //this->m_TextureID++;
            //if (this->m_TextureID > 4)
            //    this->m_TextureID = 0;
            this->m_Test++;
            if (this->m_Test > sba_Players.size()-1)
                this->m_Test = 0;
        }


        if (this->gameStart)
        {
            if (sba_Network->IsConnected())
                this->HandleNetwork();
            else
                this->HandleLocal();

            int camID = 0;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                if (sba_Players[i]->PadID != -1)
                {
                    TheBrick::CSpaceship* playerShip;
                    if (camID == 0 && this->m_Test != -1)
                        playerShip = sba_Players[this->m_Test]->Ship;
                    else
                        playerShip = sba_Players[i]->Ship;
                    this->m_Cameras[camID]->Update(0, playerShip, a_pApplication->GetInput(), a_pApplication->GetTimer());
                    PuRe_QuaternionF rotation = this->m_Cameras[camID]->GetQuaternion();

                    if (this->m_Emitters[camID]->GetAmount() < 200)
                    {
                        for (int i = 0; i<10; i++)
                        {
                            PuRe_Vector3F pos = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
                            pos.X = ((std::rand() % 100) / 10.0f) - 5.0f;
                            pos.Y = ((std::rand() % 100) / 10.0f) - 5.0f;
                            pos.Z = (std::rand() % 100) / 10.0f;
                            PuRe_Vector3F size = PuRe_Vector3F(0.05f, 0.05f, 0.05f);
                            float rsize = (std::rand() % 10) / 10.0f;
                            size.X *= rsize;
                            size.Y *= rsize;
                            size.Z *= rsize;
                            PuRe_Vector3F velocity = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
                            PuRe_Color color;
                            color.R = (std::rand() % 255) / 255.0f;
                            color.G = (std::rand() % 255) / 255.0f;
                            color.B = (std::rand() % 255) / 255.0f;
                            this->m_Emitters[camID]->Spawn(0.5f, pos, size, velocity, rotation, color);
                        }
                    }
                    if (TheBrick::OngToPuRe(playerShip->m_pBody->getAngularVelocity()).Length() > 0.1f || TheBrick::OngToPuRe(playerShip->m_pBody->getLinearVelocity()).Length() > 0.1f)
                        this->m_Emitters[camID]->Update(a_pApplication->GetTimer()->GetElapsedSeconds());
                    this->m_Emitters[camID]->m_Position = this->m_Cameras[camID]->GetPosition();
                    this->m_Emitters[camID]->m_Rotation = rotation;
                    camID++;
                }
            }
            if (this->m_LocalPlayers == 3)
            {
                float p = ((sin(timer->GetTotalElapsedSeconds() / 15.0f) + 1.0f) / 2.0f)*(sba_Players.size()-1);
                float rotation = timer->GetTotalElapsedSeconds()*10.0f;
                //clamps rotation to 360
                int times = (int)rotation / 360;
                rotation = rotation-times*360;

                int player = (int)p;
                this->m_Cameras[3]->SetPosition(TheBrick::OngToPuRe(sba_Players[p]->Ship->m_pBody->getWorldCenter()));
                this->m_Cameras[3]->SetRotation(PuRe_Vector3F(0.0f, rotation, 0.0f));
                this->m_Cameras[3]->Move(PuRe_Vector3F(0.0f, 0.0f, -100.0f));
            }
        }

        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
        {
            this->m_Bullets[i]->Update(a_pApplication->GetTimer()->GetElapsedSeconds());

            if (this->m_Bullets[i]->m_Collided||this->m_Bullets[i]->m_lifeTime > 5.0f)
            {
                SAFE_DELETE(this->m_Bullets[i]);
                if (this->m_Bullets.begin() + i < this->m_Bullets.end())
                    this->m_Bullets.erase(this->m_Bullets.begin() + i);
                i--;
            }
        }



        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->Ship->m_Respawn == 0&&sba_Players[i]->Ship->m_Life <= 0)
            {
                sba_Players[i]->Ship->m_Respawn = 5.0f;
                sba_Players[i]->Ship->m_Life = 0.0f;
            }
        }


        return 1;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Render(PuRe_Application* a_pApplication)
    {
        PuRe_Color clear = PuRe_Color(0.0f, 0.4f, 1.0f);
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();
        PuRe_Timer* timer = a_pApplication->GetTimer();

        sba_Renderer->Begin(clear);


        /////////////  DRAW Light  ///////////////////////
        sba_Renderer->Draw(0, true, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 0.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        /////////////  DRAW SKY  /////////////////////// 
        #ifdef Skybox
        sba_Renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyMaterial);
        #endif
        ////////////////////////////////////////////////////

        /////////////  DRAW BRICKS  ///////////////////////
        sba_BrickManager->Render();
        ////////////////////////////////////////////////////

        /////////////  DRAW Particles  ///////////////////////
        for (int i = 0; i<this->m_LocalPlayers; i++)
            sba_Renderer->Draw(0, true, this->m_Emitters[i], this->m_pParticleMaterial, this->m_pParticleSprite);
        ////////////////////////////////////////////////////

        /////////////  DRAW FONT  ///////////////////////
        PuRe_Color c = PuRe_Color(1.0f,1.0f,1.0f,1.0f);
        PuRe_Vector3F size = PuRe_Vector3F(32.0f,32.0f,0.0f);
        PuRe_Vector3F pos = PuRe_Vector3F(100.0f, 1080 - 100.0f, 0.0f);
        int local = 0;
        for (unsigned int i=0;i<sba_Players.size();i++)
        {
            if (sba_Players[i]->PadID != -1)
            {
                sba_Renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Life: " + std::to_string(sba_Players[i]->Ship->m_Life), pos, PuRe_MatrixF(), size, 36.0f, c, local);
                local++;
            }
        }

        if (this->m_Timeout + 10.0f > 0.0f)
        {
            size = PuRe_Vector3F(64.0f, 64.0f, 0.0f);
            pos = PuRe_Vector3F(1920.0f/2.0f - 200.0f,1080.0f/2.0f, 0.0f);
            sba_Renderer->Draw(2, false, this->m_pFont, this->m_pFontMaterial, "Player "+std::to_string(this->m_WinID)+" won!", pos, PuRe_MatrixF(), size, 72.0f, c, local);
        }
        ////////////////////////////////////////////////////


        //////////////////// POST SCREEN ////////////////////////////////
        sba_Renderer->Set(0, (float)this->m_TextureID, "textureID");
        size = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < this->m_LocalPlayers; i++)
        {
            switch (this->m_LocalPlayers)
            {
            case 1:
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                sba_Renderer->Render(0,0, this->m_Cameras[i], this->m_pPostMaterial, size);
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Render(i, 1, this->m_pUICam, this->m_pUIMaterial, size);
                break;
            case 2:
                size.X = i*gdesc.ResolutionWidth / 2.0f;
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                sba_Renderer->Render(0, 0, this->m_Cameras[i], this->m_pPostMaterial, size);
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Render(i, 1, this->m_pUICam, this->m_pUIMaterial, size);
                break;
            default:
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                size.X = (i % 2)*gdesc.ResolutionWidth / 2.0f;
                size.Y = (1 - ((int)(i / 2)))*gdesc.ResolutionHeight / 2.0f;
                sba_Renderer->Render(0, 0, this->m_Cameras[i], this->m_pPostMaterial, size);
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Render(i, 1, this->m_pUICam, this->m_pUIMaterial, size);
                break;
            }
        }
        if (this->m_LocalPlayers == 3)
        {
            size.X = gdesc.ResolutionWidth / 2.0f;
            size.Y = 0.0f;
            sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
            sba_Renderer->Render(0,0, this->m_Cameras[3], this->m_pPostMaterial, size);
        }
        sba_Renderer->Render(0, 2, this->m_pUICam, this->m_pUIMaterial, size);
        sba_Renderer->End();
        ////////////////////////////////////////////////////


    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Exit()
    {
        this->m_Run = false;
        SAFE_DELETE(this->m_pParticleSprite);
        for (unsigned int i = 0; i < this->m_Emitters.size(); i++)
            SAFE_DELETE(this->m_Emitters[i]);
        // DELETE MATERIALS
        SAFE_DELETE(this->m_pParticleMaterial);
        SAFE_DELETE(this->m_pDirectionalLightMaterial);
        SAFE_DELETE(this->m_pPointLightMaterial);
        SAFE_DELETE(this->m_pFontMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pSkyMaterial);
        SAFE_DELETE(this->m_pUIMaterial);
        // DELETE OBJECTS
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            SAFE_DELETE(this->m_Bullets[i]);
        for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
            SAFE_DELETE(this->m_Asteroids[i]);
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pPointLight);
        SAFE_DELETE(this->m_pDirectionalLight);
        // DELETE CAMERAS
        for (unsigned int i = 0; i < this->m_Cameras.size(); i++)
            SAFE_DELETE(this->m_Cameras[i]);
        SAFE_DELETE(this->m_pUICam);
        // DELETE RENDERER
        SAFE_DELETE(this->m_pMinimap);
        SAFE_DELETE(this->m_pFont);
    }
}