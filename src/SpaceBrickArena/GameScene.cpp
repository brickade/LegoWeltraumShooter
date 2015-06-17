#include "include/GameScene.h"

#include "TheBrick/Conversion.h"

namespace Game
{


    // **************************************************************************
    // **************************************************************************
    void CGameScene::ListenLoop()
    {
        while (this->m_Run)
        {
            if (this->m_pNetwork->Listen())
            {
                SOCKADDR_IN* clientData = NULL;
                if (sba_Players.size() < sba::MaxPlayers)
                {
                    SOCKET s = this->m_pNetwork->Accept();
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
                    sba_Players.push_back(p);
                    printf("User %i joined!\n", ID);
                    //Tell him who he is
                    sba::SLeftPacket lPacket;
                    lPacket.Head.Type = sba::EPacket::IAm;
                    lPacket.Who = ID;
                    this->m_pNetwork->Send((char*)&lPacket, sizeof(sba::SLeftPacket), s);

                    //Send to JOINER all existing players
                    lPacket.Head.Type = sba::EPacket::CJoin;
                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        lPacket.Who = sba_Players[i]->ID;
                        printf("Sending to %i Data of player %i\n", ID, lPacket.Who);
                        this->m_pNetwork->Send((char*)&lPacket, sizeof(sba::SLeftPacket), s);
                        //Same call send this player about the JOINER
                        if (sba_Players[i]->ID != ID&&sba_Players[i]->ID != 0)
                        {
                            lPacket.Who = ID;
                            this->m_pNetwork->Send((char*)&lPacket, sizeof(sba::SLeftPacket), sba_Players[i]->NetworkInformation);
                        }
                    }
                    std::thread HandleThread(&CGameScene::ReceiveData, this, s);
                    HandleThread.detach();
                }

            }
        }

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::ReceiveData(SOCKET s)
    {
        const int bufferSize = 1024;
        char buffer[bufferSize];
        while (this->m_Run)
        {
            memset(buffer, 0, bufferSize);
            long dataLeft = this->m_pNetwork->Receive(buffer, bufferSize, s);
            if (dataLeft == -1)
                break;
            m_Mutex.lock();
            while (dataLeft > 0)
            {
                printf("Data left %d!\n", dataLeft);
                sba::SReceivePacket* Packet = (sba::SReceivePacket*)buffer;
                if (Packet->Head.Type == sba::EPacket::Left)
                {
                    sba::SLeftPacket* lPacket = (sba::SLeftPacket*)Packet;

                    for (unsigned int i = 0; i < sba_Players.size(); i++)
                    {
                        if (sba_Players[i]->ID == lPacket->Who)
                        {
                            SAFE_DELETE(sba_Players[i]);
                            sba_Players.erase(sba_Players.begin() + i);
                        }
                    }
                    printf("User % i left!\n", lPacket->Who);
                    //Send to everyone else that one left
                    if (this->m_pNetwork->GetHost())
                    {
                        for (unsigned int i = 1; i < sba_Players.size(); i++)
                        {
                            this->m_pNetwork->Send((char*)lPacket, sizeof(sba::SLeftPacket), sba_Players[i]->NetworkInformation);
                        }
                    }
                    //ship left
                    long packetSize = sizeof(sba::SLeftPacket);
                    dataLeft -= packetSize;
                    memcpy(buffer, buffer + (int)packetSize, bufferSize - packetSize);
                }
                else if (Packet->Head.Type == sba::EPacket::IAm)
                {
                    sba::SLeftPacket* LPacket = (sba::SLeftPacket*)Packet;
                    this->m_ID = LPacket->Who;
                    printf("I am %i!\n", this->m_ID);
                    //ship left
                    long packetSize = sizeof(sba::SLeftPacket);
                    dataLeft -= packetSize;
                    memcpy(buffer, buffer + (int)packetSize, bufferSize - packetSize);
                }
                else if (Packet->Head.Type == sba::EPacket::CJoin)
                {
                    sba::SLeftPacket* LPacket = (sba::SLeftPacket*)Packet;
                    sba::SPlayer* p = new sba::SPlayer();
                    p->ID = LPacket->Who;
                    //if (p->ID == this->m_ID)
                    //    this->m_ArrayID = sba_Players.size();
                    p->NetworkInformation = s;
                    sba_Players.push_back(p);
                    printf("User %i joined!\n", p->ID);

                    //ship left
                    long packetSize = sizeof(sba::SLeftPacket);
                    dataLeft -= packetSize;
                    memcpy(buffer, buffer + (int)packetSize, bufferSize - packetSize);

                }
                else if (Packet->Head.Type == sba::EPacket::Start)
                {
                    this->StartGame();
                    long packetSize = sizeof(sba::SHeadPacket);
                    dataLeft -= packetSize;
                    memcpy(buffer, buffer + (int)packetSize, bufferSize - packetSize);
                }
                else if (Packet->Head.Type == sba::EPacket::STick)
                {
                    sba::SInputPacket* IPacket = (sba::SInputPacket*)Packet;
                    m_buffer[IPacket->Frame - m_PhysicFrame].Inputs[IPacket->Input.Player] = IPacket->Input;
                    m_numReceived[IPacket->Frame - m_PhysicFrame]++;
                    printf("received tick %d from player %d\n", IPacket->Frame, IPacket->Input.Player);
                    //ship left
                    long packetSize = sizeof(sba::SInputPacket);
                    dataLeft -= packetSize;
                    memcpy(buffer, buffer + (int)packetSize, bufferSize - packetSize);
                }
                else if (Packet->Head.Type == sba::EPacket::CTick)
                {
                    sba::SInputsPacket* IPacket = (sba::SInputsPacket*)Packet;
                    PlayOutBuffer* pbuffer = &this->m_buffer[IPacket->Frame - this->m_PhysicFrame];
                    pbuffer->Frame = IPacket->Frame;
                    memcpy(pbuffer->Inputs, IPacket->Input, sizeof(sba::SInputData)*IPacket->Players);
                    printf("received tick %d\n", IPacket->Frame);
                    //ship left
                    long packetSize = sizeof(sba::SInputsPacket);
                    dataLeft -= packetSize;
                    memcpy(buffer, buffer + (int)packetSize, bufferSize - packetSize);
                }
                else
                    dataLeft -= sizeof(sba::SHeadPacket);
            }
            m_Mutex.unlock();
        }
    }

    // **************************************************************************
    // **************************************************************************
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx, bool a_Network)
    {
        this->m_pApplication = a_pApplication;
        this->m_Network = m_Network;
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
        {
            a_Ship->Shoot(this->m_Bullets, sba_BrickManager);
        }

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
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::HandleNetwork()
    {
        PuRe_IInput* aInput = this->m_pApplication->GetInput();
        m_Mutex.lock();
        if (this->m_pNetwork->GetHost())
        {
            for (int i = 0; i < BufferSize; ++i)
            {
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
                    for (unsigned int j = 1; j < sba_Players.size(); ++j)
                    {
                        this->m_pNetwork->Send((char*)&packet, sizeof(sba::SInputsPacket), sba_Players[j]->NetworkInformation);
                    }
                    m_send[i] = 1;
                }
            }
        }



        m_PhysicTime += this->m_pApplication->GetTimer()->GetElapsedSeconds();
        bool inputExists = true;
        while (m_PhysicTime >= 1.0f / 60.0f && inputExists)
        {
            if (this->m_pNetwork->GetHost())
            {
                inputExists = m_send[0] == 1;
            }
            else
                inputExists = m_buffer[0].Frame == this->m_PhysicFrame;

            if (inputExists)
            {
                printf("Input exists, do Physik!\n");
                sba::SInputPacket ipacket;
                memset(&ipacket, 0, sizeof(sba::SInputPacket));
                ipacket.Head.Type = sba::EPacket::STick;
                ipacket.Frame = this->m_PhysicFrame + sba::Delay;
                ipacket.Input = this->HandleInput(0);
                ipacket.Input.Player = this->m_ID;

                if (!this->m_pNetwork->GetHost())
                {
                    printf("send package %d\n", ipacket.Frame);
                    this->m_pNetwork->SendHost((char*)&ipacket, sizeof(sba::SInputPacket));
                }
                else
                {
                    printf("Handle own Input %d\n", ipacket.Frame);
                    m_buffer[ipacket.Frame - m_PhysicFrame].Inputs[0] = ipacket.Input;
                    m_numReceived[ipacket.Frame - m_PhysicFrame]++;
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
                if (this->m_pNetwork->GetHost())
                {
                    memcpy(m_numReceived, m_numReceived + 1, sizeof(unsigned int) * BufferSize - 1);
                    memcpy(m_send, m_send + 1, sizeof(bool) * BufferSize - 1);
                    m_numReceived[BufferSize - 1] = 0;
                    m_send[BufferSize - 1] = 0;
                    m_buffer[BufferSize - 1].Frame = this->m_PhysicFrame + BufferSize;
                }

                sba::Space::Instance()->World->step(1 / 60.0f);
                this->m_PhysicTime -= 1.0f / 60.0f;
                printf("Physic: %i\n", this->m_PhysicFrame);
                this->m_PhysicFrame++;
                assert(this->m_PhysicFrame != 2147483647);
                sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances
            } //if input exists

        } //while physic run

        m_Mutex.unlock();
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::GameSetup()
    {
        //Check last network state
        int networkState = this->m_pNetwork->GetState();

        this->m_pNetwork->Update(this->m_pApplication->GetInput(),sba::EUpdate::IP); //Update Network State

        //If he connected
        this->m_Run = true;
        if (networkState != 3 && this->m_pNetwork->GetState() == 3)
        {
            printf("Connecting!\n");
            this->m_pNetwork->Connect(true);
            //Add self as 0 if host
            if (this->m_pNetwork->GetHost())
            {
                printf("You are the Host!\n");
                sba::SPlayer* p = new sba::SPlayer();
                p->ID = 0;
                p->NetworkInformation = 0;
                //this->m_ArrayID = sba_Players.size();
                sba_Players.push_back(p);
                this->m_ID = p->ID;
                std::thread listenThread(&CGameScene::ListenLoop, this);
                listenThread.detach();
            }
            else
            {
                std::thread receiveThread(&CGameScene::ReceiveData, this,this->m_pNetwork->GetSocket());
                receiveThread.detach();
            }
        }

        if (this->m_pApplication->GetInput()->KeyPressed(this->m_pApplication->GetInput()->F3) && networkState == 3 && this->m_pNetwork->GetHost())
        {
            this->StartGame();
            //Send to everyone that the game started
            sba::SHeadPacket Packet;
            Packet.Type = sba::EPacket::Start;
            for (unsigned int i = 1; i < sba_Players.size(); i++)
            {
                this->m_pNetwork->Send((char*)&Packet, sizeof(sba::SHeadPacket), sba_Players[i]->NetworkInformation);
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::StartGame()
    {
        if (this->m_pNetwork != NULL)
        {
            this->m_PhysicTime = 0.0f;
            this->m_PhysicFrame = 0;
            if (this->m_pNetwork->GetHost())
            {
                printf("\n");
                printf("Resetting Buffer ... \n");
                for (int i = 0; i < BufferSize; ++i)
                {
                    m_buffer[i].Frame = i;
                    memset(m_buffer[i].Inputs, 0, sizeof(sba::SInputData) * sba::MaxPlayers);
                    m_send[i] = 0;
                    m_numReceived[i] = 0;
                }
                printf("Buffer Reset!\n");
                //send 6 frames from self, because server is also a player
                for (int i = 0; i < sba::Delay; i++)
                {
                    memset(&m_buffer[i - m_PhysicFrame].Inputs[0], 0, sizeof(sba::SInputData));
                    m_numReceived[i - m_PhysicFrame]++;
                }
                printf("Set Initial Input for Host.\n");
                printf("\n");

            }
            else
            {
                memset(m_buffer, -1, sizeof(PlayOutBuffer) * BufferSize);
                //send first 6 frames
                sba::SInputPacket package;
                memset(&package, 0, sizeof(sba::SInputPacket));
                package.Head.Type = sba::EPacket::STick;
                package.Input.Player = this->m_ID;

                for (int i = 0; i < sba::Delay; ++i)
                {
                    package.Frame = i;
                    this->m_pNetwork->SendHost((char*)&package, sizeof(sba::SInputPacket));
                }
                printf("Send initial Data!\n");
            }
        }
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            ong::vec3 pos = ong::vec3(10.0f, 10.0f, 10.0f);
            pos.x += sba_Players[i]->ID*10.0f;
            sba_Players[i]->Ship->m_pBody->setPosition(pos);
        }
        ong::vec3 start(50.0f, 50.0f, 50.0f);
        for (int i = 0; i < 10; i++)
        {
            TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(sba_BrickManager, *sba_World, start + ong::vec3((i % 4)*10.0f, ((i * 5) % 2)*2.0f, i*5.0f));
            TheBrick::CSerializer serializer;
            serializer.OpenRead("../data/ships/asteroid.object");
            asteroid->Deserialize(serializer, *sba_BrickManager, *sba_World);
            this->m_Asteroids.push_back(asteroid);
            serializer.Close();
        }
        this->gameStart = true;
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
        PuRe_Vector2F fsize = PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight);
        if (!this->m_pNetwork)
        {
            if (this->m_LocalPlayers == 2)
                size.X /= 2;
            else if (this->m_LocalPlayers > 2)
            {
                size.X /= 2;
                size.Y /= 2;
            }
        }
        sba_Renderer->AddTarget(size);
        sba_Renderer->AddTarget(PuRe_Vector2I(gdesc.ResolutionWidth, gdesc.ResolutionHeight));
        sba_Renderer->AddTarget(PuRe_Vector2I(gdesc.ResolutionWidth, gdesc.ResolutionHeight));
        if (sba::CIniReader::Instance()->GetValue("SSAO") == "On")
        {
            sba_Renderer->SetSSAO(0, sba_Space->m_SSAOMaterial, sba_Space->m_pNoiseTexture);
            sba_Renderer->SetSSAO(1, sba_Space->m_SSAOMaterial, sba_Space->m_pNoiseTexture);
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

            if(this->m_Network)
            {
                this->m_pNetwork = new sba::CNetworkHandler();
            this->m_ID = 0;
            this->m_PhysicFrame = 0;
            this->gameStart = false;
            }
            else
                this->StartGame();

    }

    // **************************************************************************
    // **************************************************************************
    int CGameScene::Update(PuRe_Application* a_pApplication)
    {
        PuRe_Timer* timer = a_pApplication->GetTimer();
        //Handle ESC Button
        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC))
        {
            return 0;
        }


        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->Left))
        {
            this->m_TextureID--;
            if (this->m_TextureID < 0)
                this->m_TextureID = 4;
        }

        else if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->Right))
        {
            this->m_TextureID++;
            if (this->m_TextureID > 4)
                this->m_TextureID = 0;
        }


        if (this->gameStart)
        {
            if (this->m_pNetwork != NULL)
                this->HandleNetwork();
            else
                this->HandleLocal();

            int camID = 0;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                if (sba_Players[i]->PadID != -1)
                {
                    TheBrick::CSpaceship* playerShip = sba_Players[i]->Ship;
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
                }
                camID++;
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
        else
            this->GameSetup();

        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
        {
            this->m_Bullets[i]->Update(a_pApplication->GetTimer()->GetElapsedSeconds());

            if (this->m_Bullets[i]->m_lifeTime > 5.0f)
            {
                SAFE_DELETE(this->m_Bullets[i]);
                if (this->m_Bullets.begin() + i < this->m_Bullets.end())
                    this->m_Bullets.erase(this->m_Bullets.begin() + i);
                i--;
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
        sba_BrickManager->Render(*sba_Renderer);
        ////////////////////////////////////////////////////

        /////////////  DRAW Particles  ///////////////////////
        for (int i = 0; i<this->m_LocalPlayers; i++)
            sba_Renderer->Draw(0, true, this->m_Emitters[i], this->m_pParticleMaterial, this->m_pParticleSprite);
        ////////////////////////////////////////////////////

        //////////////////////  NETWORK UI  /////////////////////////////
        if (this->m_pNetwork != NULL)
        {
            int nstate = this->m_pNetwork->GetState();
            if (nstate == 0)
                sba_Renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Press << 0 >> to Host and << 1 >> to Join", PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
            else if (nstate == 1)
                sba_Renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, ("IP: " + this->m_pNetwork->m_IP).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
            else if (nstate == 2)
                sba_Renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, ("Port: " + this->m_pNetwork->m_Port).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
        }
        //////////////////// POST SCREEN ////////////////////////////////
        sba_Renderer->Set(0, (float)this->m_TextureID, "textureID");
        sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
        PuRe_Vector3F size = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < this->m_LocalPlayers; i++)
        {
            switch (this->m_LocalPlayers)
            {
            case 1:
                sba_Renderer->Render(0, this->m_Cameras[i], this->m_pPostMaterial, size);
                break;
            case 2:
                size.X = i*gdesc.ResolutionWidth / 2.0f;
                sba_Renderer->Render(0, this->m_Cameras[i], this->m_pPostMaterial, size);
                break;
            default:
                size.X = (i % 2)*gdesc.ResolutionWidth / 2.0f;
                size.Y = (1 - ((int)(i / 2)))*gdesc.ResolutionHeight / 2.0f;
                sba_Renderer->Render(0, this->m_Cameras[i], this->m_pPostMaterial, size);
                break;
            }
        }
        if (this->m_LocalPlayers == 3)
        {
            size.X = gdesc.ResolutionWidth / 2.0f;
            size.Y = 0.0f;
            sba_Renderer->Render(0, this->m_Cameras[3], this->m_pPostMaterial, size);
        }
        sba_Renderer->Render(1, this->m_pUICam, this->m_pPostMaterial, size);
        sba_Renderer->End();
        ////////////////////////////////////////////////////


    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Exit()
    {
        this->m_Run = false;
        if (this->m_pNetwork != NULL)
        {
            //Send to Host that we left
            sba::SLeftPacket lPacket;
            lPacket.Head.Type = sba::EPacket::Left;
            lPacket.Who = this->m_ID;
            this->m_pNetwork->SendHost((char*)&lPacket, sizeof(sba::SLeftPacket));
        }
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