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
        if (this->m_pNetwork->m_Host)
        {
            this->m_pInputBuffer = new InputData[this->m_Players.size()];
            memset(this->m_pInputBuffer, 0, sizeof(InputData)*this->m_Players.size());
            this->replay = fopen("replayHost.txt", "w");
        }
        else
        {
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
        sba::Space::Instance()->UpdatePhysics(this->m_pApplication->GetTimer());

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
                    this->m_pInputBuffer[IPacket->Input.Player] = IPacket->Input;
                }
                else if (Packet->Head.Type == 6)
                {
                    InputsPacket* IPacket = (InputsPacket*)Packet;

                    //save into playout
                    if (this->m_PlayOutBuffer.size() == 0)
                        this->m_PlayOutTime = this->m_pApplication->GetTimer()->GetTotalElapsedMilliseconds() + Delay;
                    else
                        this->m_PlayOutTime += (1 / 60) * 1000;

                    int bufferSize = sizeof(InputData)*this->m_Players.size();
                    //Create new playout buffer content and add it
                    PlayOut* play = new PlayOut();
                    play->pInputBuffer = new InputData[this->m_Players.size()];
                    memcpy(play->pInputBuffer, IPacket->Input, bufferSize);
                    play->time = this->m_PlayOutTime;
                    this->m_PlayOutBuffer.push_back(play);
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
            if (this->m_pNetwork->m_Host)
            {
                bool allSend = true;
                for (unsigned int i = 1; i < this->m_Players.size(); i++)
                {
                    if (this->m_pInputBuffer[i].Player == 0)
                        allSend = false;
                }
                if (allSend)
                {
                    int bufferSize = sizeof(InputData)*this->m_Players.size();
                    InputsPacket inputs;
                    inputs.Head.Type = 6;
                    inputs.Players = this->m_Players.size();
                    memcpy(inputs.Input, this->m_pInputBuffer, bufferSize);
                    //Server received Data from all Clients
                    for (unsigned int i = 0; i < this->m_Players.size(); i++)
                    {
                        this->m_pNetwork->Send((char*)&inputs, sizeof(InputsPacket), this->m_Players[i]->NetworkInformation);
                    }

                    //save into playout
                    if (this->m_PlayOutBuffer.size() == 0)
                        this->m_PlayOutTime = this->m_pApplication->GetTimer()->GetTotalElapsedMilliseconds() + Delay;
                    else
                        this->m_PlayOutTime += (1 / 60) * 1000;

                    //Create new playout buffer content and add it
                    PlayOut* play = new PlayOut();
                    play->pInputBuffer = new InputData[this->m_Players.size()];
                    memcpy(play->pInputBuffer, this->m_pInputBuffer, bufferSize);
                    play->time = this->m_PlayOutTime;
                    this->m_PlayOutBuffer.push_back(play);

                    //clear input Buffer
                    memset(this->m_pInputBuffer, 0, bufferSize);

                }
            }


            bool physicRun = true;
            while (physicRun)
            {
                if (this->m_PlayOutBuffer.size() > 0)
                {
                    int index = -1;
                    for (unsigned int i = 0; i < this->m_PlayOutBuffer.size(); i++)
                    {
                        if (this->m_PlayOutBuffer[i]->pInputBuffer[0].Frame == this->m_PhysicFrame)
                        {
                            index = i;
                            break;
                        }
                            
                    }
                    if (index > -1&&this->m_PlayOutBuffer[index]->time < a_pApplication->GetTimer()->GetTotalElapsedMilliseconds())
                    {
                        std::string  replayText = "Frame: " + std::to_string(this->m_PlayOutBuffer[index]->pInputBuffer[0].Frame) + "\n";
                        fwrite(replayText.c_str(), sizeof(char), replayText.length(), this->replay);
                        replayText = "ID Shoot Thrust MoveX MoveY Spin\n";
                        fwrite(replayText.c_str(), sizeof(char), replayText.length(), this->replay);
                        
                        for (unsigned int i = 0; i < this->m_Players.size(); i++)
                        {
                            unsigned int id = this->m_PlayOutBuffer[index]->pInputBuffer[i].Player;
                            for (unsigned int j = 0; j < this->m_Players.size(); j++)
                            {
                                if (this->m_Players[j]->ID == id)
                                {
                                    replayText = std::to_string(id) += " ";

                                    TheBrick::CSpaceship* ship = this->m_Players[j]->Ship;
                                    if (this->m_PlayOutBuffer[index]->pInputBuffer[i].Shoot)
                                    {
                                        ship->Shoot(this->m_Bullets, sba::Space::Instance()->BrickManager);
                                        replayText += "1 ";
                                    }
                                    else
                                        replayText += "0 ";

                                    if (this->m_PlayOutBuffer[index]->pInputBuffer[i].Thrust)
                                    {
                                        printf("ID: %i THRUST\n",id);
                                        ship->Thrust(1.0f);
                                        replayText += "1 ";
                                    }
                                    else
                                        replayText += "0 ";

                                    PuRe_Vector2F Move = PuRe_Vector2F(0.0f, 0.0f);
                                    if (this->m_PlayOutBuffer[index]->pInputBuffer[i].MoveX == 1)
                                        Move.X = 1.0f;
                                    else if (this->m_PlayOutBuffer[index]->pInputBuffer[i].MoveX == 2)
                                        Move.X = -1.0f;
                                    if (this->m_PlayOutBuffer[index]->pInputBuffer[i].MoveY == 1)
                                        Move.Y = 1.0f;
                                    else if (this->m_PlayOutBuffer[index]->pInputBuffer[i].MoveY == 2)
                                        Move.Y = -1.0f;
                                    if (Move.Length() > 0.2f)
                                        ship->Move(Move);

                                    replayText += std::to_string(this->m_PlayOutBuffer[index]->pInputBuffer[i].MoveX) + " ";
                                    replayText += std::to_string(this->m_PlayOutBuffer[index]->pInputBuffer[i].MoveY) + " ";

                                    if (this->m_PlayOutBuffer[index]->pInputBuffer[i].Spin == 1)
                                        this->m_Players[j]->Ship->Spin(1.0f);
                                    else if (this->m_PlayOutBuffer[index]->pInputBuffer[i].Spin == 2)
                                        this->m_Players[j]->Ship->Spin(-1.0f);

                                    replayText += std::to_string(this->m_PlayOutBuffer[index]->pInputBuffer[i].Spin) + " ";

                                    ship->Update(a_pApplication->GetTimer()->GetElapsedSeconds());

                                    replayText += "\n";
                                    fwrite(replayText.c_str(), sizeof(char), replayText.length(), this->replay);
                                    break;
                                }
                            }

                        }
                        sba::Space::Instance()->World->step(1 / 60.0f);
                        //sba::Space::Instance()->UpdatePhysics(a_pApplication->GetTimer());
                        sba::Space::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances
                        printf("Physic: %i\n",this->m_PhysicFrame);
                        this->m_PhysicFrame++;
                    }
                    else
                        physicRun = false;
                }
                else
                    physicRun = false;
            }


            InputPacket ipacket;
            memset(&ipacket, 0, sizeof(InputPacket));
            ipacket.Head.Type = 5;
            ipacket.Input.Frame = this->m_PhysicFrame;
            ipacket.Input.Player = this->m_ID;
            if (a_pApplication->GetInput()->GamepadPressed(a_pApplication->GetInput()->Pad_A, 0))
            {
                ipacket.Input.Shoot = true;
                //InputBasePacket IPacket;
                //IPacket.Head.Type = 5;
                //IPacket.Input = 0;
                //IPacket.Who = this->m_ID;
                //if (this->m_pNetwork->m_Host)
                //{
                //    this->m_Players[this->m_ArrayID]->Ship->Shoot(this->m_Bullets, sba::Space::Instance()->BrickManager);
                //    for (unsigned int i = 1; i < this->m_Players.size(); i++)
                //        this->m_pNetwork->Send((char*)&IPacket, sizeof(InputBasePacket), this->m_Players[i]->NetworkInformation);
                //}
                //else
                //{
                //    this->m_pNetwork->SendHost((char*)&IPacket, sizeof(InputBasePacket));
                //}
            }

            PuRe_Vector2F Move = a_pApplication->GetInput()->GetGamepadLeftThumb(0);
            if (Move.X > 0.5f)
                ipacket.Input.MoveX = 1;
            else if (Move.X < -0.5f)
                ipacket.Input.MoveX = 2;
            if (Move.Y > 0.5f)
                ipacket.Input.MoveY = 1;
            else if (Move.Y < -0.5f)
                ipacket.Input.MoveY = 2;
       /*     if (Move.Length() > 0.5f)
            {
                MovePacket MPacket;
                MPacket.InputBase.Head.Type = 5;
                MPacket.InputBase.Input = 1;
                MPacket.InputBase.Who = this->m_ID;
                MPacket.Move = Move;
                if (this->m_pNetwork->m_Host)
                {
                    this->m_Players[this->m_ArrayID]->Ship->Move(Move);
                    for (unsigned int i = 1; i < this->m_Players.size(); i++)
                        this->m_pNetwork->Send((char*)&MPacket, sizeof(MovePacket), this->m_Players[i]->NetworkInformation);
                }
                else
                    this->m_pNetwork->SendHost((char*)&MPacket, sizeof(MovePacket));
            }*/

            float Thrust = a_pApplication->GetInput()->GetGamepadRightTrigger(0);
            if (Thrust > 0.2f)
            {
                ipacket.Input.Thrust = true;
                //ThrustPacket TPacket;
                //TPacket.InputBase.Head.Type = 5;
                //TPacket.InputBase.Input = 2;
                //TPacket.InputBase.Who = this->m_ID;
                //TPacket.Thrust = Thrust;
                //if (this->m_pNetwork->m_Host)
                //{
                //    this->m_Players[this->m_ArrayID]->Ship->Thrust(Thrust);
                //    for (unsigned int i = 1; i < this->m_Players.size(); i++)
                //        this->m_pNetwork->Send((char*)&TPacket, sizeof(ThrustPacket), this->m_Players[i]->NetworkInformation);
                //}
                //else
                //    this->m_pNetwork->SendHost((char*)&TPacket, sizeof(ThrustPacket));
            }

            //float Spin = 0.0f;
            if (a_pApplication->GetInput()->GamepadIsPressed(a_pApplication->GetInput()->Left_Shoulder, 0))
                ipacket.Input.Spin = 2;
            else if (a_pApplication->GetInput()->GamepadIsPressed(a_pApplication->GetInput()->Right_Shoulder, 0))
                ipacket.Input.Spin = 1;
           /* if (Spin > 0.2f || Spin < -0.2f)
            {
                ThrustPacket TPacket;
                TPacket.InputBase.Head.Type = 5;
                TPacket.InputBase.Input = 3;
                TPacket.InputBase.Who = this->m_ID;
                TPacket.Thrust = Spin;
                if (this->m_pNetwork->m_Host)
                {
                    this->m_Players[this->m_ArrayID]->Ship->Spin(Spin);
                    for (unsigned int i = 1; i < this->m_Players.size(); i++)
                        this->m_pNetwork->Send((char*)&TPacket, sizeof(ThrustPacket), this->m_Players[i]->NetworkInformation);
                }
                else
                    this->m_pNetwork->SendHost((char*)&TPacket, sizeof(ThrustPacket));
            }*/
            if (!this->m_pNetwork->m_Host)
            {
                this->m_pNetwork->SendHost((char*)&ipacket, sizeof(InputPacket));
            }
            else
            {
                this->m_pInputBuffer[0] = ipacket.Input;
            }
            //for (unsigned int i = 0; i < this->m_Players.size(); i++)
            //{
            //    TheBrick::CSpaceship* player = this->m_Players[i]->Ship;
            //    //player->HandleInput(i, a_pApplication->GetInput(), a_pApplication->GetTimer()->GetElapsedSeconds(), this->m_Bullets, sba::Space::Instance()->BrickManager);

            //    //this->m_Cameras[i]->Update(i,this->m_Players[i]->Ship, a_pApplication->GetInput(), a_pApplication->GetTimer());
            //    player->Update(a_pApplication->GetTimer()->GetElapsedSeconds());
            //}
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
        for (unsigned int i = 0; i < this->m_PlayOutBuffer.size(); i++)
        {
            SAFE_DELETE_ARRAY(this->m_PlayOutBuffer[i]->pInputBuffer);
            SAFE_DELETE(this->m_PlayOutBuffer[i]);
        }
        this->m_PlayOutBuffer.clear();
        SAFE_DELETE_ARRAY(this->m_pInputBuffer);
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