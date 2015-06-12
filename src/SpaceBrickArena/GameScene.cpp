#include "include/GameScene.h"

#include "TheBrick/Conversion.h"

namespace Game
{

    // **************************************************************************
    // **************************************************************************
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_pApplication = a_pApplication;
    }

    // **************************************************************************
    // **************************************************************************
    InputData CGameScene::HandleInput(int a_PlayerIdx)
    {
        PuRe_IInput* aInput = this->m_pApplication->GetInput();

        InputData input;
        memset(&input, 0, sizeof(InputData));

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
    void CGameScene::ProcessInput(TheBrick::CSpaceship* a_Ship, InputData* a_Input, float a_DeltaTime)
    {
        if (a_Input->Shoot)
            a_Ship->Shoot(this->m_Bullets, sba_BrickManager);

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
        for (unsigned int i = 0; i < this->m_Players.size(); i++)
        {
            InputData input = this->HandleInput(i);
            this->ProcessInput(this->m_Players[i]->Ship, &input, aTimer->GetElapsedSeconds());
        }
        sba::Space::Instance()->UpdatePhysics(aTimer);
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::HandleNetwork()
    {
        PuRe_IInput* aInput = this->m_pApplication->GetInput();
        m_Mutex.lock();
        if (this->m_pNetwork->m_Host)
        {
            for (int i = 0; i < BufferSize; ++i)
            {
                InputsPacket packet;
                //if all player send something
                if (!m_send[i] && m_numReceived[i] >= this->m_Players.size())
                {
                    packet.Head.Type = Packet::CTick;
                    packet.Frame = m_buffer[i].Frame;
                    packet.Players = this->m_Players.size();
                    memcpy(packet.Input, m_buffer[i].Inputs, sizeof(InputData)*this->m_Players.size());
                    //send tick with input
                    printf("send tick %d\n", m_buffer[i].Frame);
                    for (unsigned int j = 1; j < this->m_Players.size(); ++j)
                    {
                        this->m_pNetwork->Send((char*)&packet, sizeof(InputsPacket), this->m_Players[j]->NetworkInformation);
                    }
                    m_send[i] = 1;
                }
            }
        }



        m_PhysicTime += this->m_pApplication->GetTimer()->GetElapsedSeconds();
        bool inputExists = true;
        while (m_PhysicTime >= 1.0f / 60.0f && inputExists)
        {
            if (this->m_pNetwork->m_Host)
            {
                inputExists = m_send[0] == 1;
            }
            else
                inputExists = m_buffer[0].Frame == this->m_PhysicFrame;
            if (inputExists)
            {
                InputPacket ipacket;
                memset(&ipacket, 0, sizeof(InputPacket));
                ipacket.Head.Type = Packet::STick;
                ipacket.Frame = this->m_PhysicFrame + Delay;
                ipacket.Input = this->HandleInput(0);
                ipacket.Input.Player = this->m_ID;

                if (!this->m_pNetwork->m_Host)
                {
                    printf("send package %d\n", ipacket.Frame);
                    this->m_pNetwork->SendHost((char*)&ipacket, sizeof(InputPacket));
                }
                else
                {
                    printf("Handle own Input %d\n", ipacket.Frame);
                    m_buffer[ipacket.Frame - m_PhysicFrame].Inputs[0] = ipacket.Input;
                    m_numReceived[ipacket.Frame - m_PhysicFrame]++;
                }

                //Now handle input

                PlayOutBuffer* buffer = &this->m_buffer[0];

                InputData* input;
                for (unsigned int i = 0; i < this->m_Players.size(); i++)
                {
                    unsigned int id = buffer->Inputs[i].Player;
                    for (unsigned int j = 0; j < this->m_Players.size(); j++)
                    {
                        if (this->m_Players[j]->ID == id)
                        {
                            input = &buffer->Inputs[i];
                            this->ProcessInput(this->m_Players[j]->Ship,input,1/60.0f);
                            break;
                        }
                    }
                }

                memcpy(m_buffer, m_buffer + 1, sizeof(PlayOutBuffer) * BufferSize - 1);
                if (this->m_pNetwork->m_Host)
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

        this->m_pNetwork->Update(this->m_pApplication->GetInput()); //Update Network State

        //If he connected
        if (networkState != 3 && this->m_pNetwork->GetState() == 3)
        {
            printf("Connecting!\n");
            this->m_pNetwork->Connect();
            //Add self as 0 if host
            if (this->m_pNetwork->m_Host)
            {
                printf("You are the Host!\n");
                Player* p = new Player();
                p->ID = 0;
                p->NetworkInformation = SOCKADDR_IN();
                this->m_ArrayID = this->m_Players.size();
                this->m_Players.push_back(p);
                this->m_ID = p->ID;
            }
            //Start thread to listen
            std::thread receiveThread(&CGameScene::ReceiveData, this);
            receiveThread.detach();
        }

        if (this->m_pApplication->GetInput()->KeyPressed(this->m_pApplication->GetInput()->F3) && networkState == 3 && this->m_pNetwork->m_Host)
        {
            this->StartGame();
            //Send to everyone that the game started
            HeadPacket Packet;
            Packet.Type = Packet::Start;
            for (unsigned int i = 1; i < this->m_Players.size(); i++)
            {
                this->m_pNetwork->Send((char*)&Packet, sizeof(HeadPacket), this->m_Players[i]->NetworkInformation);
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
            if (this->m_pNetwork->m_Host)
            { 
                printf("\n");
                printf("Resetting Buffer ... \n");
                for (int i = 0; i < BufferSize; ++i)
                {
                    m_buffer[i].Frame = i;
                    memset(m_buffer[i].Inputs, 0, sizeof(InputData) * MaxPlayers);
                    m_send[i] = 0;
                    m_numReceived[i] = 0;
                }
                printf("Buffer Reset!\n");
                //send 6 frames from self, because server is also a player
                for (int i = 0; i < Delay; i++)
                {
                    memset(&m_buffer[i - m_PhysicFrame].Inputs[0], 0, sizeof(InputData));
                    m_numReceived[i - m_PhysicFrame]++;
                }
                printf("Set Initial Input for Host.\n");
                printf("\n");

            }
            else
            {
                memset(m_buffer, -1, sizeof(PlayOutBuffer) * BufferSize);
                //send first 6 frames
                InputPacket package;
                memset(&package, 0, sizeof(InputPacket));
                package.Head.Type = Packet::STick;
                package.Input.Player = this->m_ID;

                for (int i = 0; i < Delay; ++i)
                {
                    package.Frame = i;
                    this->m_pNetwork->SendHost((char*)&package, sizeof(InputPacket));
                }
            }
        }
        for (unsigned int i = 0; i < this->m_Players.size(); i++)
        {
            TheBrick::CSerializer serializer;
            serializer.OpenRead("../data/ships/banana.ship");
            ong::vec3 pos = ong::vec3(10.0f, 10.0f, 10.0f);
            pos.x += this->m_Players[i]->ID*10.0f;
            this->m_Players[i]->Ship = new TheBrick::CSpaceship(*sba_World, pos);
            this->m_Players[i]->Ship->Deserialize(serializer, *sba_BrickManager, *sba_World);
            serializer.Close();
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
    void CGameScene::ReceiveData()
    {
        char buffer[256];
        SOCKADDR_IN sender;
        while (true)
        {
            if (this->m_pNetwork->Receive(buffer, 256, &sender) != -1)
            {
                ReceivePacket* Packet = (ReceivePacket*)buffer;
                if (Packet->Head.Type == Packet::Join && this->m_Players.size() < MaxPlayers)
                {
                    int ID = 0; // 0 is Host
                    for (unsigned int i = 0; i < this->m_Players.size(); i++)
                    {
                        if (ID == this->m_Players[i]->ID)
                        {
                            ID++;
                            i = 0;
                        }
                    }
                    Player* p = new Player();
                    p->ID = ID;
                    p->NetworkInformation = sender;
                    this->m_Players.push_back(p);
                    printf("User %i joined!\n", ID);
                    //Tell him who he is
                    LeftPacket lPacket;
                    lPacket.Head.Type = Packet::IAm;
                    lPacket.Who = ID;
                    this->m_pNetwork->Send((char*)&lPacket, sizeof(LeftPacket), sender);

                    //Send to JOINER all existing players
                    lPacket.Head.Type = Packet::CJoin;
                    for (unsigned int i = 0; i < this->m_Players.size(); i++)
                    {
                        lPacket.Who = this->m_Players[i]->ID;
                        this->m_pNetwork->Send((char*)&lPacket, sizeof(LeftPacket), sender);
                        //Same call send this player about the JOINER
                        if (this->m_Players[i]->ID != ID)
                        {
                            lPacket.Who = ID;
                            this->m_pNetwork->Send((char*)&lPacket, sizeof(LeftPacket), this->m_Players[i]->NetworkInformation);
                        }
                    }
                }
                else if (Packet->Head.Type == Packet::Left)
                {
                    LeftPacket* lPacket = (LeftPacket*)Packet;

                    for (unsigned int i = 0; i < this->m_Players.size(); i++)
                    {
                        if (this->m_Players[i]->ID == lPacket->Who)
                        {
                            SAFE_DELETE(this->m_Players[i]);
                            this->m_Players.erase(this->m_Players.begin() + i);
                        }
                    }
                    printf("User % i left!\n", lPacket->Who);
                    //Send to everyone else that one left
                    if (this->m_pNetwork->m_Host)
                    {
                        for (unsigned int i = 0; i < this->m_Players.size(); i++)
                        {
                            this->m_pNetwork->Send((char*)lPacket, sizeof(LeftPacket), this->m_Players[i]->NetworkInformation);
                        }
                    }
                }
                else if (Packet->Head.Type == Packet::IAm)
                {
                    LeftPacket* LPacket = (LeftPacket*)Packet;
                    this->m_ID = LPacket->Who;
                    printf("I am %i!\n", this->m_ID);
                }
                else if (Packet->Head.Type == Packet::CJoin)
                {
                    LeftPacket* LPacket = (LeftPacket*)Packet;
                    Player* p = new Player();
                    p->ID = LPacket->Who;
                    if (p->ID == this->m_ID)
                        this->m_ArrayID = this->m_Players.size();
                    p->NetworkInformation = sender;
                    this->m_Players.push_back(p);
                    printf("User %i joined!\n", p->ID);

                }
                else if (Packet->Head.Type == Packet::Start)
                {
                    this->StartGame();
                }
                else if (Packet->Head.Type == Packet::STick)
                {
                    InputPacket* IPacket = (InputPacket*)Packet;
                    m_Mutex.lock();
                    m_buffer[IPacket->Frame - m_PhysicFrame].Inputs[IPacket->Input.Player] = IPacket->Input;
                    m_numReceived[IPacket->Frame - m_PhysicFrame]++;
                    printf("received tick %d from player %d\n", IPacket->Frame, IPacket->Input.Player);
                    m_Mutex.unlock();
                }
                else if (Packet->Head.Type == Packet::CTick)
                {
                    InputsPacket* IPacket = (InputsPacket*)Packet;
                    m_Mutex.lock();
                    PlayOutBuffer* buffer = &this->m_buffer[IPacket->Frame - this->m_PhysicFrame];
                    buffer->Frame = IPacket->Frame;
                    memcpy(buffer->Inputs, IPacket->Input, sizeof(InputData)*IPacket->Players);
                    printf("received tick %d\n", IPacket->Frame);
                    m_Mutex.unlock();
                }
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();

        //Camera
        PuRe_Vector2F size = PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight);
        //this->m_pMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/GameEffects/default/default"); //Kann weg
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
        this->m_pEmitter = new PuRe_ParticleEmitter(PuRe_Vector3F(), PuRe_QuaternionF());
        this->m_pParticleSprite = new PuRe_Sprite(a_pApplication->GetGraphics(),"../data/textures/dust.png");
        this->m_pUICam = new PuRe_Camera(size, PuRe_CameraProjection::Orthogonal);
        CGameCamera* Cam = new CGameCamera(size, PuRe_Camera_Perspective);
        Cam->Initialize();
        this->m_Cameras.push_back(Cam);
        this->m_TextureID = 0;

        #ifdef Skybox
            this->m_pSkyBox = new PuRe_SkyBox(a_pApplication->GetGraphics(), "../data/textures/cube/");
        #endif

        #ifdef NETWORKGAME
            this->m_pNetwork = new CNetworkHandler();
            this->m_pNetwork->m_NetworkState = 0;
            this->m_ID = 0;
            this->m_PhysicFrame = 0;
            this->m_ArrayID = 0;
            this->gameStart = false;
        #else
            Player* p = new Player();
            p->ID = 0;
            p->NetworkInformation = SOCKADDR_IN();
            this->m_ArrayID = this->m_Players.size();
            this->m_Players.push_back(p);
            this->m_ID = p->ID;
            this->StartGame();
        #endif

    }

    // **************************************************************************
    // **************************************************************************
    bool CGameScene::Update(PuRe_Application* a_pApplication)
    {
        //Handle ESC Button
        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC))
        {
            return true;
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

            TheBrick::CSpaceship* playerShip = this->m_Players[this->m_ArrayID]->Ship;
            this->m_Cameras[0]->Update(0, playerShip, a_pApplication->GetInput(), a_pApplication->GetTimer());
            PuRe_QuaternionF rotation = this->m_Cameras[0]->GetQuaternion();
            if (this->m_pEmitter->GetAmount() < 200)
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
                    this->m_pEmitter->Spawn(0.5f, pos, size, velocity, rotation, color);
                }
            }
            if (TheBrick::OngToPuRe(playerShip->m_pBody->getAngularVelocity()).Length() > 0.1f || TheBrick::OngToPuRe(playerShip->m_pBody->getLinearVelocity()).Length() > 0.1f)
                this->m_pEmitter->Update(a_pApplication->GetTimer()->GetElapsedSeconds());
            this->m_pEmitter->m_Position = this->m_Cameras[0]->GetPosition();
            this->m_pEmitter->m_Rotation = rotation;
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


        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Render(PuRe_Application* a_pApplication)
    {
        PuRe_Color clear = PuRe_Color(0.0f, 0.4f, 1.0f);
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();

        PuRe_Renderer* renderer = sba_Renderer;
        renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));


        /////////////  DRAW Light  ///////////////////////
        renderer->Draw(0, true, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 0.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        /////////////  DRAW SKY  /////////////////////// 
        #ifdef Skybox
            renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyMaterial);
        #endif
        ////////////////////////////////////////////////////

        /////////////  DRAW BRICKS  ///////////////////////
        sba_BrickManager->Render(*sba::Space::Instance()->Renderer);
        sba_BrickManager->Render(*sba_Renderer);
        ////////////////////////////////////////////////////

        /////////////  DRAW Particles  ///////////////////////
        renderer->Draw(0,true,this->m_pEmitter,this->m_pParticleMaterial,this->m_pParticleSprite);
        ////////////////////////////////////////////////////

        //////////////////////  NETWORK UI  /////////////////////////////
        if (this->m_pNetwork != NULL)
        {
            int nstate = this->m_pNetwork->GetState();
            if (nstate == 0)
                renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Press << 0 >> to Host and << 1 >> to Join", PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
            else if (nstate == 1)
                renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, ("IP: " + this->m_pNetwork->m_IP).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
            else if (nstate == 2)
                renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, ("Port: " + this->m_pNetwork->m_Port).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
        }
        //////////////////// POST SCREEN ////////////////////////////////
        renderer->Set(0, (float)this->m_TextureID, "textureID");
        renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
        PuRe_Vector3F size = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        renderer->Render(0, this->m_Cameras[0], this->m_pPostMaterial, size);
        renderer->Render(1, this->m_pUICam, this->m_pPostMaterial, size);
        renderer->End();
        ////////////////////////////////////////////////////


    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Exit()
    {
        sba::CIniReader::Instance()->Save();
        if (this->m_pNetwork != NULL)
        {
            //Send to Host that we left
            LeftPacket lPacket;
            lPacket.Head.Type = Packet::Left;
            lPacket.Who = this->m_ID;
            this->m_pNetwork->SendHost((char*)&lPacket, sizeof(LeftPacket));
        }
        //Clear Memory
        for (unsigned int i = 0; i < this->m_Players.size(); i++)
        {
            SAFE_DELETE(this->m_Players[i]->Ship);
            SAFE_DELETE(this->m_Players[i]);
        }
        this->m_Players.clear();
        SAFE_DELETE(this->m_pParticleSprite);
        SAFE_DELETE(this->m_pEmitter);
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