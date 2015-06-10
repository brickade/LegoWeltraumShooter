#include "include/GameScene.h"

#include "TheBrick/Conversion.h"

namespace Game
{
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_pApplication = a_pApplication;
    }

    void CGameScene::StartGame()
    {
        this->m_PhysicTime = 0.0f;
        this->m_PhysicFrame = 0;
        if (this->m_pNetwork->m_Host)
        {
            for (int i = 0; i < BufferSize; ++i)
            {
                m_buffer[i].Frame = i;
                memset(m_buffer[i].Inputs, 0, sizeof(InputData) * MaxPlayers);
                m_send[i] = 0;
                m_numReceived[i] = 0;
            }
            this->replay = fopen("replayHost.txt", "w");
            //send 6 frames from self, because server is also a player
            for (int i = 0; i < Delay; i++)
            {
                memset(&m_buffer[i - m_PhysicFrame].Inputs[0], 0, sizeof(InputData));
                m_numReceived[i - m_PhysicFrame]++;
            }

        }
        else
        {
            memset(m_buffer, -1, sizeof(PlayOutBuffer) * BufferSize);
            //send first 6 frames
            InputPacket package;
            memset(&package, 0, sizeof(InputPacket));
            package.Head.Type = 5;
            package.Input.Player = this->m_ID;

            for (int i = 0; i < Delay; ++i)
            {
                package.Frame = i;
                this->m_pNetwork->SendHost((char*)&package, sizeof(InputPacket));
            }

            this->replay = fopen("replayClient.txt", "w");
        }
        for (unsigned int i = 0; i < this->m_Players.size(); i++)
        {
            TheBrick::CSerializer serializer;
            serializer.OpenRead("../data/ships/banana.ship");
            ong::vec3 pos = ong::vec3(10.0f, 10.0f, 10.0f);
            pos.x += this->m_Players[i]->ID*10.0f;
            this->m_Players[i]->Ship = new TheBrick::CSpaceship(*sba::Space::Instance()->World, pos);
            this->m_Players[i]->Ship->Deserialize(serializer, *sba::Space::Instance()->BrickManager, *sba::Space::Instance()->World);
            serializer.Close();
        }
        ong::vec3 start(50.0f, 50.0f, 50.0f);
        for (int i = 0; i < 10; i++)
        {
            TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(sba::Space::Instance()->BrickManager, *sba::Space::Instance()->World, start + ong::vec3((i % 4)*10.0f, ((i * 5) % 2)*2.0f, i*5.0f));
            TheBrick::CSerializer serializer;
            serializer.OpenRead("../data/ships/asteroid.object");
            asteroid->Deserialize(serializer, *sba::Space::Instance()->BrickManager, *sba::Space::Instance()->World);
            this->m_Asteroids.push_back(asteroid);
            serializer.Close();
        }
        this->gameStart = true;
        sba::Space::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::ReceiveData()
    {
        char buffer[256];
        SOCKADDR_IN sender;
        while (true)
        {
            printf("Receiving Data!\n");
            if (this->m_pNetwork->Receive(buffer, 256, &sender) != -1)
            {
                ReceivePacket* Packet = (ReceivePacket*)buffer;
                if (Packet->Head.Type == 0 && this->m_Players.size() < MaxPlayers)
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
                    lPacket.Head.Type = 2;
                    lPacket.Who = ID;
                    this->m_pNetwork->Send((char*)&lPacket, sizeof(LeftPacket), sender);

                    //Send to JOINER all existing players
                    lPacket.Head.Type = 3;
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
                else if (Packet->Head.Type == 1)
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
                else if (Packet->Head.Type == 2)
                {
                    LeftPacket* LPacket = (LeftPacket*)Packet;
                    this->m_ID = LPacket->Who;
                    printf("I am %i!\n", this->m_ID);
                }
                else if (Packet->Head.Type == 3)
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
                else if (Packet->Head.Type == 4)
                {
                    this->StartGame();
                }
                else if (Packet->Head.Type == 5)
                {
                    InputPacket* IPacket = (InputPacket*)Packet;
                    m_Mutex.lock();
                    m_buffer[IPacket->Frame - m_PhysicFrame].Inputs[IPacket->Input.Player] = IPacket->Input;
                    m_numReceived[IPacket->Frame - m_PhysicFrame]++;
                    printf("received tick %d from player %d\n", IPacket->Frame, IPacket->Input.Player);
                    m_Mutex.unlock();
                }
                else if (Packet->Head.Type == 6)
                {
                    InputsPacket* IPacket = (InputsPacket*)Packet;
                    m_Mutex.lock();
                    PlayOutBuffer* buffer = &this->m_buffer[IPacket->Frame - this->m_PhysicFrame];
                    buffer->Frame = IPacket->Frame;
                    memcpy(buffer->Inputs, IPacket->Input, sizeof(InputData)*IPacket->Players);
                    printf("received tick %d\n", IPacket->Frame);
                    m_Mutex.unlock();
                }

                //for (unsigned int i = 0; i < this->m_Players.size(); i++)
                //{
                //    if (this->m_Players[i]->ID == IPacket->Who)
                //    {
                //        if (IPacket->Input == 0)
                //        {
                //            this->m_Players[i]->Ship->Shoot(this->m_Bullets, sba::Space::Instance()->BrickManager);
                //        }
                //        else if (IPacket->Input == 1)
                //        {
                //            MovePacket* MPacket = (MovePacket*)Packet;
                //            this->m_Players[i]->Ship->Move(MPacket->Move);
                //        }
                //        else if (IPacket->Input == 2)
                //        {
                //            ThrustPacket* TPacket = (ThrustPacket*)Packet;
                //            this->m_Players[i]->Ship->Thrust(TPacket->Thrust);
                //        }
                //        else if (IPacket->Input == 3)
                //        {
                //            ThrustPacket* TPacket = (ThrustPacket*)Packet;
                //            this->m_Players[i]->Ship->Spin(TPacket->Thrust);
                //        }
                //    }
                //    //send to everyone that someone shot if HOST
                //    if (this->m_pNetwork->m_Host&&i != 0)
                //    {
                //        if (IPacket->Input == 0)
                //        {
                //            this->m_pNetwork->Send((char*)IPacket, sizeof(InputBasePacket), this->m_Players[i]->NetworkInformation);
                //        }
                //        else if (IPacket->Input == 1)
                //        {
                //            MovePacket* MPacket = (MovePacket*)Packet;
                //            this->m_pNetwork->Send((char*)MPacket, sizeof(MovePacket), this->m_Players[i]->NetworkInformation);
                //        }
                //        else if (IPacket->Input == 2 || IPacket->Input == 3)
                //        {
                //            ThrustPacket* TPacket = (ThrustPacket*)Packet;
                //            this->m_pNetwork->Send((char*)TPacket, sizeof(ThrustPacket), this->m_Players[i]->NetworkInformation);
                //        }
                //    }

                //}
                //}
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
        //this->m_pSkyBox = new PuRe_SkyBox(a_pApplication->GetGraphics(), "../data/textures/cube/");
        this->m_pPointLight = new PuRe_PointLight(a_pApplication->GetGraphics());
        this->m_pDirectionalLight = new PuRe_DirectionalLight(a_pApplication->GetGraphics());
        this->m_pMinimap = new CMinimap(a_pApplication->GetGraphics());
        this->m_pFont = new PuRe_Font(a_pApplication->GetGraphics(), "../data/textures/font.png");
        this->m_pNetwork = new CNetworkHandler();
        this->m_pUICam = new PuRe_Camera(size, PuRe_CameraProjection::Orthogonal);
        this->gameStart = false;
        this->m_pNetwork->m_NetworkState = 0;
        this->m_ID = 0;
        this->m_PhysicFrame = 0;
        this->m_ArrayID = 0;
        CGameCamera* Cam = new CGameCamera(size, PuRe_Camera_Perspective);
        Cam->Initialize();
        this->m_Cameras.push_back(Cam);
        ////int playerNum = 2;
        //for (int i=0;i<playerNum;i++)
        //{
        //    CGameCamera* Cam = new CGameCamera(size, PuRe_Camera_Perspective);
        //    Cam->Initialize();
        //    this->m_Cameras.push_back(Cam);
        //    Player* p = new Player();
        //    p->ID = i;
        //    p->NetworkInformation = SOCKADDR_IN();
        //    this->m_Players.push_back(p);
        //}

        //this->m_pPlayerShip = new TheBrick::CSpaceship();
        //this->m_pPlayerShip->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
        //ong::vec3 start(50.0f, 50.0f, 50.0f);
        //for (int i = 0; i < 10; i++)
        //{
        //    TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(BrickBozz::Instance()->World, start + ong::vec3((i % 2)*5.0f,(i % 2) * 5.0f,i*2.0f));
        //    asteroid->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
        //    this->m_Asteroids.push_back(asteroid);
        //}
        //this->StartGame();
        this->m_TextureID = 0;

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
            m_Mutex.lock();
            if (this->m_pNetwork->m_Host)
            {
                for (int i = 0; i < BufferSize; ++i)
                {
                    InputsPacket packet;
                    //if all player send something
                    if (!m_send[i] && m_numReceived[i] >= this->m_Players.size())
                    {
                        packet.Head.Type = 6;
                        packet.Frame = m_buffer[i].Frame;
                        packet.Players = this->m_Players.size();
                        memcpy(packet.Input, m_buffer[i].Inputs, sizeof(InputData)*this->m_Players.size());
                        //send tick with input
                        printf("send tick %d\n", m_buffer[i].Frame);
                        for (int j = 1; j < this->m_Players.size(); ++j)
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
                    ipacket.Head.Type = 5;
                    ipacket.Frame = this->m_PhysicFrame + Delay;
                    ipacket.Input.Player = this->m_ID;
                    if (a_pApplication->GetInput()->GamepadPressed(a_pApplication->GetInput()->Pad_A, 0))
                        ipacket.Input.Shoot = true;

                    PuRe_Vector2F Move = a_pApplication->GetInput()->GetGamepadLeftThumb(0);
                    if (Move.X > 0.5f)
                        ipacket.Input.MoveX = 1;
                    else if (Move.X < -0.5f)
                        ipacket.Input.MoveX = 2;
                    if (Move.Y > 0.5f)
                        ipacket.Input.MoveY = 1;
                    else if (Move.Y < -0.5f)
                        ipacket.Input.MoveY = 2;

                    float Thrust = a_pApplication->GetInput()->GetGamepadRightTrigger(0);
                    if (Thrust > 0.2f)
                        ipacket.Input.Thrust = true;

                    if (a_pApplication->GetInput()->GamepadIsPressed(a_pApplication->GetInput()->Left_Shoulder, 0))
                        ipacket.Input.Spin = 2;
                    else if (a_pApplication->GetInput()->GamepadIsPressed(a_pApplication->GetInput()->Right_Shoulder, 0))
                        ipacket.Input.Spin = 1;

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
                    std::string  replayText = "Time: " + std::to_string(this->m_PhysicTime) + "\n";
                    fwrite(replayText.c_str(), sizeof(char), replayText.length(), this->replay);
                    replayText = "Frame: " + std::to_string(buffer->Frame) + "\n";
                    fwrite(replayText.c_str(), sizeof(char), replayText.length(), this->replay);
                    replayText = "ID Shoot Thrust MoveX MoveY Spin\n";
                    fwrite(replayText.c_str(), sizeof(char), replayText.length(), this->replay);

                    InputData* input;
                    for (unsigned int i = 0; i < this->m_Players.size(); i++)
                    {
                        unsigned int id = buffer->Inputs[i].Player;
                        for (unsigned int j = 0; j < this->m_Players.size(); j++)
                        {
                            if (this->m_Players[j]->ID == id)
                            {
                                input = &buffer->Inputs[i];
                                replayText = std::to_string(id) += " ";

                                TheBrick::CSpaceship* ship = this->m_Players[j]->Ship;
                                if (input->Shoot)
                                {
                                    ship->Shoot(this->m_Bullets, sba::Space::Instance()->BrickManager);
                                    replayText += "1 ";
                                }
                                else
                                    replayText += "0 ";

                                if (input->Thrust)
                                {
                                    ship->Thrust(1.0f);
                                    replayText += "1 ";
                                }
                                else
                                    replayText += "0 ";

                                PuRe_Vector2F Move = PuRe_Vector2F(0.0f, 0.0f);
                                if (input->MoveX == 1)
                                    Move.X = 1.0f;
                                else if (input->MoveX == 2)
                                    Move.X = -1.0f;
                                if (input->MoveY == 1)
                                    Move.Y = 1.0f;
                                else if (input->MoveY == 2)
                                    Move.Y = -1.0f;
                                if (Move.Length() > 0.2f)
                                    ship->Move(Move);

                                replayText += std::to_string(input->MoveX) + " ";
                                replayText += std::to_string(input->MoveY) + " ";

                                if (input->Spin == 1)
                                    this->m_Players[j]->Ship->Spin(1.0f);
                                else if (input->Spin == 2)
                                    this->m_Players[j]->Ship->Spin(-1.0f);

                                replayText += std::to_string(input->Spin) + " ";

                                ship->Update(1/60.0f);

                                replayText += "\n";
                                fwrite(replayText.c_str(), sizeof(char), replayText.length(), this->replay);
                                break;
                            }
                        }
                    }

                    memcpy(m_buffer, m_buffer + 1, sizeof(PlayOutBuffer) * BufferSize-1);
                    if (this->m_pNetwork->m_Host)
                    {
                        memcpy(m_numReceived, m_numReceived + 1, sizeof(int) * BufferSize - 1);
                        memcpy(m_send, m_send + 1, sizeof(int) * BufferSize - 1);
                        m_numReceived[BufferSize - 1] = 0;
                        m_send[BufferSize - 1] = 0;
                        m_buffer[BufferSize - 1].Frame = this->m_PhysicFrame + BufferSize;
                    }

                    sba::Space::Instance()->World->step(1 / 60.0f);
                    this->m_PhysicTime -= 1.0f / 60.0f;
                    printf("Physic: %i\n", this->m_PhysicFrame);
                    this->m_PhysicFrame++;
                    assert(this->m_PhysicFrame != 2147483647);
                    sba::Space::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances
                } //if input exists

            } //while physic run

            m_Mutex.unlock();

            this->m_Cameras[0]->Update(0, this->m_Players[this->m_ArrayID]->Ship, a_pApplication->GetInput(), a_pApplication->GetTimer());
        }
        else
        {

            //Check last network state
            int networkState = this->m_pNetwork->GetState();

            this->m_pNetwork->Update(a_pApplication->GetInput()); //Update Network State

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

            if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->F3) && networkState == 3 && this->m_pNetwork->m_Host)
            {
                //Send to everyone that
                HeadPacket Packet;
                Packet.Type = 4;
                for (unsigned int i = 1; i < this->m_Players.size(); i++)
                {
                    this->m_pNetwork->Send((char*)&Packet, sizeof(HeadPacket), this->m_Players[i]->NetworkInformation);
                }
                this->StartGame();
            }
        }

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

        PuRe_Renderer* renderer = sba::Space::Instance()->Renderer;
        renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));


        /////////////  DRAW Light  ///////////////////////
        renderer->Draw(0, true, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 0.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        /////////////  DRAW SKY  ///////////////////////
        //renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyMaterial);
        ////////////////////////////////////////////////////

        /////////////  DRAW BRICKS  ///////////////////////
        sba::Space::Instance()->BrickManager->Render(*sba::Space::Instance()->Renderer);
        ////////////////////////////////////////////////////

        /////////////  DRAW MINIMAP  ////////////////////////
        //PuRe_Vector3F minipos = PuRe_Vector3F(gdesc.ResolutionWidth - 150.0f, gdesc.ResolutionHeight - 150.0f, 128.0f);
        //PuRe_Vector3F rot = this->m_pCamera->GetRotation();
        //rot *= PuRe_DegToRad;
        //PuRe_MatrixF rotation = PuRe_QuaternionF(rot).GetMatrix();
        //this->m_pMinimap->Draw(renderer, this->m_pUIMaterial, minipos, rotation);
        // Draw Players
        if (this->gameStart)
        {
            /*           for (int i = 0; i < this->m_Players.size(); i++)
                       {
                       this->m_pMinimap->DrawPlayer(renderer, this->m_pUIMaterial, TheBrick::OngToPuRe(this->m_Players[i]->Ship->m_pBody->getWorldCenter()), this->m_MapBoundaries, rotation);
                       }*/
            //for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            //    this->m_Bullets[i]->Draw(a_pApplication->GetGraphics(), this->m_pCamera);
            //for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
            //    this->m_Asteroids[i]->Draw(a_pApplication->GetGraphics(), this->m_pCamera);
        }
        ////////////////////////////////////////////////////

        //////////////////////  NETWORK UI  /////////////////////////////
        int nstate = this->m_pNetwork->GetState();
        if (nstate == 0)
            renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Press << 0 >> to Host and << 1 >> to Join", PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
        else if (nstate == 1)
            renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, ("IP: " + this->m_pNetwork->m_IP).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);
        else if (nstate == 2)
            renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, ("Port: " + this->m_pNetwork->m_Port).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_MatrixF::Identity(), PuRe_Vector3F(32.0f, 32.0f, 32.0f), 32.0f);

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
        fclose(this->replay);
        //Send to Host that we left
        LeftPacket lPacket;
        lPacket.Head.Type = 1;
        lPacket.Who = this->m_ID;
        this->m_pNetwork->SendHost((char*)&lPacket, sizeof(LeftPacket));
        //Clear Memory
        for (unsigned int i = 0; i < this->m_Players.size(); i++)
        {
            SAFE_DELETE(this->m_Players[i]->Ship);
            SAFE_DELETE(this->m_Players[i]);
        }
        this->m_Players.clear();
        // DELETE MATERIALS
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