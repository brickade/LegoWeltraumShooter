#include "include/GameScene.h"

#include "TheBrick/Conversion.h"


#include "Onager\Profiler.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx, bool a_Network)
    {
        this->m_pApplication = a_pApplication;
        this->m_Test = -1;
    }

    // **************************************************************************
    // **************************************************************************
    sba::SInputData CGameScene::HandleInput(int a_PlayerIdx, PuRe_IInput* a_pInput)
    {
        sba::SInputData input;
        memset(&input, 0, sizeof(sba::SInputData));

        input.MG = sba_Input->Axis(Input::EAxis::Type::GameShootMG, a_PlayerIdx) > 0.0f;
        input.Laser = sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootLaser, a_PlayerIdx);
        input.Rocket = sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootRocket, a_PlayerIdx);
        input.Torpedo = sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootTorpedo, a_PlayerIdx);
        input.Mine = sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootMine, a_PlayerIdx);

        PuRe_Vector2F Move;
        if (sba_Controls[a_PlayerIdx].Move == 1)
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_1, a_PlayerIdx);
        else if (sba_Controls[a_PlayerIdx].Move == 2)
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_2, a_PlayerIdx);
        else if (sba_Controls[a_PlayerIdx].Move == 3)
        {
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_1, a_PlayerIdx);
            Move.Y = -Move.Y;
        }
        else if (sba_Controls[a_PlayerIdx].Move == 4)
        {
            Move = sba_Input->Direction(Input::EDirection::Type::GameMove_2, a_PlayerIdx);
            Move.Y = -Move.Y;
        }

        if (Move.X > 0.1f || Move.X < -0.1f)
            input.MoveX = (char)(Move.X*100.0f);
        if (Move.Y > 0.1f || Move.Y < -0.1f)
            input.MoveY = -(char)(Move.Y*100.0f);



        float fThrust;
        if (a_PlayerIdx == 0)
        {
            if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameThrust, a_PlayerIdx))
                fThrust = 100;
        }

        if (sba_Controls[a_PlayerIdx].Thrust == 1)
            fThrust = sba_Input->Axis(Input::EAxis::Type::GameThrust_1, a_PlayerIdx);
        else if (sba_Controls[a_PlayerIdx].Thrust == 1)
            fThrust = sba_Input->Axis(Input::EAxis::Type::GameThrust_2, a_PlayerIdx);
        else
            fThrust = sba_Input->Axis(Input::EAxis::Type::GameThrust_3, a_PlayerIdx);

        input.Thrust = (char)(((fThrust + 1.0f) / 2.0f)*100.0f);

        float Spin;
        if (sba_Controls[a_PlayerIdx].Spin == 1)
            Spin = sba_Input->Axis(Input::EAxis::Type::GameSpin_1, a_PlayerIdx);
        else
            Spin = sba_Input->Axis(Input::EAxis::Type::GameSpin_2, a_PlayerIdx);

        if (Spin < -0.1f || Spin > 0.1f)
            input.Spin = -(char)(Spin*100.0f);

        return input;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::ProcessInput(std::vector<CBullet*>& a_rBullets, SPlayer* a_pPlayer, sba::SInputData* a_Input, float a_DeltaTime,int a_Time)
    {
        CSpaceship* ship = a_pPlayer->Ship;
        int camID = 0;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
            {
                if (sba_Players[i]->Ship == ship)
                    break;
                else
                    camID++;
            }
        }
        float* cd = &a_pPlayer->MGCD;
        if (*cd == 0.0f&&a_Input->MG)
        {
            ship->Shoot(TheBrick::MG - 100, a_rBullets, a_pPlayer);
            *cd = 0.25f;
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }
        cd = &a_pPlayer->RocketCD;
        if (*cd == 0.0f&&a_Input->Rocket)
        {
            ship->Shoot(TheBrick::Rocket - 100, a_rBullets, a_pPlayer);
            *cd = 2.0f;
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }
        cd = &a_pPlayer->TorpedoCD;
        if (*cd == 0.0f&&a_Input->Torpedo)
        {
            ship->Shoot(TheBrick::Torpedo - 100, a_rBullets, a_pPlayer);
            *cd = 1.0f;
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }
        cd = &a_pPlayer->LaserCD;
        if (*cd == 0.0f&&a_Input->Laser)
        {
            ship->Shoot(TheBrick::Laser - 100, a_rBullets, a_pPlayer);
            *cd = 0.5f;
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }
        cd = &a_pPlayer->MineCD;
        if (*cd == 0.0f&&a_Input->Mine)
        {
            ship->Shoot(TheBrick::Mine - 100, a_rBullets, a_pPlayer);
            *cd = 1.0f;
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }

        if (a_Input->Thrust != 0)
            ship->Thrust(a_Input->Thrust / 100.0f);

        PuRe_Vector2F Move = PuRe_Vector2F(0.0f, 0.0f);
        if (a_Input->MoveX != 0)
            Move.X = a_Input->MoveX / 100.0f;
        if (a_Input->MoveY != 0)
            Move.Y = a_Input->MoveY / 100.0f;
        if (Move.Length() > 0.1f)
            ship->Move(Move);

        if (a_Input->Spin != 0)
            ship->Spin(a_Input->Spin / 100.0f);

        ship->Update(a_pPlayer->ID, a_DeltaTime, a_Time);
    }

    void CGameScene::UpdateGame(std::vector<CBullet*>& a_rBullets, std::vector<CItem*>& a_rItems, float a_Deltatime)
    {
        //Update Bullets
        for (unsigned int i = 0; i < a_rBullets.size(); i++)
        {
            a_rBullets[i]->Update(a_Deltatime);

            if (a_rBullets[i]->m_Collided || a_rBullets[i]->m_lifeTime > 5.0f)
            {
                SAFE_DELETE(a_rBullets[i]);
                if (a_rBullets.begin() + i < a_rBullets.end())
                    a_rBullets.erase(a_rBullets.begin() + i);
                i--;
            }
        }
        //Update Items
        for (unsigned int i = 0; i < a_rItems.size(); i++)
        {
            a_rItems[i]->Update(a_Deltatime);

            if (a_rItems[i]->m_Collided)
            {
                SAFE_DELETE(a_rItems[i]);
                if (a_rItems.begin() + i < a_rItems.end())
                    a_rItems.erase(a_rItems.begin() + i);
                i--;
            }
        }

        //Set Player Respawn
        int listeners = 1;
        ong::vec3 pos;
        ong::vec3 forw = ong::vec3(0.0f, 0.0f, 1.0f);
        ong::vec3 up = ong::vec3(0.0f, 1.0f, 0.0f);
        ong::Quaternion q;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {

            if (sba_Players[i]->PadID != -1)
            {
                pos = sba_Players[i]->Ship->m_pBody->getWorldCenter();
                q = sba_Players[i]->Ship->m_pBody->getOrientation();
                forw = ong::vec3(0.0f, 0.0f, 1.0f);
                forw = ong::rotate(forw, q);
                up = ong::vec3(0.0f, 0.0f, 1.0f);
                up = ong::rotate(up, q);
                sba_SoundPlayer->SetListenPosition(listeners, TheBrick::OngToPuRe(pos), PuRe_Vector3F(), TheBrick::OngToPuRe(forw), TheBrick::OngToPuRe(up));
                listeners++;
            }

            if (sba_Players[i]->Ship->m_Respawn == 0 && sba_Players[i]->Ship->m_Life <= 0)
            {
                sba_Players[i]->Ship->m_Respawn = 5.0f;
                sba_Players[i]->Ship->m_Life = 0;
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::HandleLocal()
    {
        PuRe_IInput* aInput = this->m_pApplication->GetInput();
        PuRe_Timer* aTimer = this->m_pApplication->GetTimer();
        float seconds = aTimer->GetElapsedSeconds();
        int tseconds = (int)aTimer->GetTotalElapsedSeconds();
        if (seconds < 1.0f)
        {
            //Lagg avoid
            if (this->m_StartTimer > 0.2f)
            {
			    ong_START_PROFILE(UPDATE_GAME);
                this->UpdateGame(this->m_Bullets, this->m_Items, seconds);
			    ong_END_PROFILE(UPDATE_GAME);

                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    sba::SInputData input = this->HandleInput(sba_Players[i]->PadID, aInput);
                    this->ProcessInput(this->m_Bullets, sba_Players[i], &input, seconds, tseconds);
                }

			    ong_START_PROFILE(UPDATE_PHYSICS);
                sba::Space::Instance()->UpdatePhysics(aTimer);
			    ong_END_PROFILE(UPDATE_PHYSICS);
                this->m_EndTime -= seconds;
            }
            else
                this->m_StartTimer += seconds;
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::HandleNetwork()
    {
        this->m_pNetwork->UpdateNetwork(this->m_Bullets, this->m_Items, this->m_pApplication->GetInput(), this->m_pApplication->GetTimer()->GetElapsedSeconds(), this->m_EndTime, &CGameScene::HandleInput, &CGameScene::ProcessInput, &CGameScene::UpdateGame);

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::StartGame()
    {
        if (sba_Network->IsConnected())
            this->m_pNetwork->Initialize();

        int listeners = 1;

        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            sba_Players[i]->Ship->CalculateData();
            ong::vec3 pos = ong::vec3(-125.0f, 0.0f, -300.0f);
            pos.x += sba_Players[i]->ID*50.0f;
            sba_Players[i]->Ship->m_pBody->setPosition(pos);

            if (sba_Players[i]->PadID != -1)
                listeners++;

        }

        sba_SoundPlayer->SetListeners(listeners);
        sba_SoundPlayer->SetListenPosition(0, PuRe_Vector3F(), PuRe_Vector3F(), PuRe_Vector3F(0.0f, 0.0f, 1.0f), PuRe_Vector3F(0.0f, 1.0f, 0.0f));

        if (!sba_Map->GetMapData(this->m_Asteroids, this->m_Items,this->m_Lights)) //Map doesn't exist!! we end here
            this->m_EndTime = -1000;

        this->m_TimeLimit = 5.0f;
        this->m_EndTime = 60.0f*this->m_TimeLimit; //seconds * Minutes
        this->m_StartTimer = 0.0f;
        sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_GraphicsDescription gdesc = graphics->GetDescription();
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
        sba_Renderer->AddTarget(size);
        sba_Renderer->AddTarget(PuRe_Vector2I(1920, 1080));
        if (sba_Options->GetValue("SSAO") == "On")
        {
            sba_Renderer->SetSSAO(0, sba_Space->m_SSAOMaterial, sba_Space->m_pNoiseTexture);
        }

        this->m_pSphere = new PuRe_Sphere(graphics);

        if (sba_SkyBoxName != sba_Map->GetSkybox())
        {
            SAFE_DELETE(sba_SkyBox);
            sba_SkyBox = new PuRe_SkyBox(graphics, sba_Map->GetSkybox());
            sba_SkyBoxName = sba_Map->GetSkybox();
        }
        this->m_pShieldMaterial = graphics->LoadMaterial("../data/effects/shield/default");
        this->m_pFontMaterial = graphics->LoadMaterial("../data/effects/font/default");
        this->m_pUIMaterial = graphics->LoadMaterial("../data/effects/UI/default");
        this->m_pPostMaterial = graphics->LoadMaterial("../data/effects/Post/default");
        this->m_pPointLightMaterial = graphics->LoadMaterial("../data/effects/PointLight/default");
        this->m_pDirectionalLightMaterial = graphics->LoadMaterial("../data/effects/DirectionalLight/default");
        this->m_pParticleMaterial = graphics->LoadMaterial("../data/effects/particles/default");
        this->m_pEngineMaterial = graphics->LoadMaterial("../data/effects/engine/default");
        this->m_pSpriteMaterial = graphics->LoadMaterial("../data/effects/sprite/default");
        this->m_pPointLight = new PuRe_PointLight(graphics);
        this->m_pDirectionalLight = new PuRe_DirectionalLight(graphics);
        this->m_pFont = new PuRe_Font(graphics, "../data/textures/font.png");
        this->m_pLockSprite = new PuRe_Sprite(graphics, "../data/textures/lock.png");
        this->m_pParticle1Sprite = new PuRe_Sprite(graphics, "../data/textures/partikel_1.png");
        this->m_pParticle2Sprite = new PuRe_Sprite(graphics, "../data/textures/partikel_2.png");
        this->m_pParticle3Sprite = new PuRe_Sprite(graphics, "../data/textures/partikel_3.png");
        this->m_pUICam = new PuRe_Camera(fsize, PuRe_CameraProjection::Orthogonal);

        this->m_pUI = new CGUI(graphics);

        //Create for each player a camera76
        for (int i = 0; i < this->m_LocalPlayers; i++)
        {
            CGameCamera* Cam = new CGameCamera(fsize, PuRe_Camera_Perspective);
            Cam->Initialize();
            this->m_Cameras.push_back(Cam);
            PuRe_ParticleEmitter* emitter = new PuRe_ParticleEmitter(PuRe_Vector3F(), PuRe_QuaternionF());
            this->m_SpaceEmitter.push_back(emitter);
        }

        if (this->m_LocalPlayers == 3)
        {
            CGameCamera* Cam = new CGameCamera(fsize, PuRe_Camera_Perspective);
            Cam->Initialize();
            this->m_Cameras.push_back(Cam);
        }

        this->m_TextureID = 0;

        if (sba_Network->IsConnected())
            this->m_pNetwork = new CGameNetwork();


        this->StartGame();

    }

    // **************************************************************************
    // **************************************************************************
    int CGameScene::Update(PuRe_Application* a_pApplication)
    {
		ong_START_PROFILE(UPDATE);

        PuRe_Timer* timer = a_pApplication->GetTimer();
        if (sba_Network->IsConnected())
        {
            this->m_pNetwork->Update(timer->GetElapsedSeconds());
        }

        //Handle ESC Button
        if (this->m_EndTime + 10.0f < 0.0f || a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC))
        {
            if (sba_Network->IsConnected())
            {
                SAFE_DELETE(sba_Network);
                sba_Network = new sba::CNetworkHandler(sba_Options);
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
                this->m_Test = sba_Players.size() - 1;
        }

        else if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->Right))
        {
            //this->m_TextureID++;
            //if (this->m_TextureID > 4)
            //    this->m_TextureID = 0;
            this->m_Test++;
            if (this->m_Test > sba_Players.size() - 1)
                this->m_Test = 0;
        }


        if (sba_Network->IsConnected())
            this->HandleNetwork();
        else
            this->HandleLocal();

        int camID = 0;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
            {

                sba_Players[i]->Update(timer->GetElapsedSeconds(), this->m_Origin, this->m_OriginDistance);
                sba::CSpaceship* playerShip;
                if (camID == 0 && this->m_Test != -1)
                    playerShip = sba_Players[this->m_Test]->Ship;
                else
                    playerShip = sba_Players[i]->Ship;
                if (playerShip->m_Life > 0.0f)
                    this->m_Cameras[camID]->UpdateData(sba_Players[i]->PadID, sba_Players[i], a_pApplication->GetInput(), a_pApplication->GetTimer());
                PuRe_QuaternionF rotation = this->m_Cameras[camID]->GetQuaternion();

                if (this->m_SpaceEmitter[camID]->GetAmount() < 200)
                {
                    for (int i = 0; i<10; i++)
                    {
                        PuRe_Vector3F pos = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
                        pos.X = ((std::rand() % 100) / 10.0f) - 5.0f;
                        pos.Y = ((std::rand() % 100) / 10.0f) - 5.0f;
                        pos.Z = (std::rand() % 100) / 2.0f;
                        PuRe_Vector3F size = PuRe_Vector3F(0.05f, 0.05f, 0.05f);
                        float rsize = (std::rand() % 10) / 10.0f;
                        size.X *= rsize;
                        size.Y *= rsize;
                        size.Z *= rsize;
                        PuRe_Vector3F velocity = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
                        PuRe_Color color = PuRe_Color(1.0f,1.0f,1.0f,1.0f);
                        this->m_SpaceEmitter[camID]->Spawn(0.5f, pos, size, velocity, rotation, color);
                    }
                }
                if (TheBrick::OngToPuRe(playerShip->m_pBody->getAngularVelocity()).Length() > 0.1f || TheBrick::OngToPuRe(playerShip->m_pBody->getLinearVelocity()).Length() > 0.1f)
                    this->m_SpaceEmitter[camID]->Update(a_pApplication->GetTimer()->GetElapsedSeconds());
                this->m_SpaceEmitter[camID]->m_Position = this->m_Cameras[camID]->GetPosition();
                this->m_SpaceEmitter[camID]->m_Rotation = rotation;

                camID++;
            }
        }
        if (this->m_LocalPlayers == 3)
        {
            float p = ((sin(timer->GetTotalElapsedSeconds() / 15.0f) + 1.0f) / 2.0f)*(sba_Players.size() - 1);
            float rotation = timer->GetTotalElapsedSeconds()*10.0f;
            //clamps rotation to 360
            int times = (int)rotation / 360;
            rotation = rotation - times * 360;

            int player = (int)p;
            this->m_Cameras[3]->SetPosition(TheBrick::OngToPuRe(sba_Players[player]->Ship->m_pBody->getWorldCenter()));
            this->m_Cameras[3]->SetRotation(PuRe_Vector3F(0.0f, rotation, 0.0f));
            this->m_Cameras[3]->Move(PuRe_Vector3F(0.0f, 0.0f, -100.0f));
        }



        if (!this->m_Won&&this->m_EndTime < 0.0f)
        {
            int points = 0;
            this->m_WonIndex = 0;
            this->m_WonID = 0;
            for (unsigned int i = 0; i < sba_Players.size(); i++)
            {
                if (sba_Players[i]->Points > sba_Players[this->m_WonIndex]->Points)
                {
                    this->m_WonIndex = i;
                    this->m_WonID = sba_Players[i]->ID;
                    points = sba_Players[i]->Points;

                }
            }
            this->m_Won = true;
        }

        //Delete objects outside
        ong::vec3 origin = TheBrick::PuReToOng(this->m_Origin);
        sba_Space->DelMiscOutside(origin, this->m_OriginDistance);
        for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
        {
            if (ong::length(this->m_Asteroids[i]->m_pBody->getWorldCenter() - origin) > this->m_OriginDistance)
            {
                delete this->m_Asteroids[i];
                this->m_Asteroids.erase(this->m_Asteroids.begin() + i);
                --i;
            }
        }

	ong_END_PROFILE(UPDATE);

        return 1;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Render(PuRe_Application* a_pApplication)
    {
		ong_START_PROFILE(RENDER);

        PuRe_Color clear = PuRe_Color(0.0f, 0.4f, 1.0f);
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();
        PuRe_Timer* timer = a_pApplication->GetTimer();

        sba_Renderer->Begin(clear);


        float seconds = timer->GetTotalElapsedSeconds();
        float greyscale[4];
        bool mapend[4];
        memset(greyscale, 0, sizeof(greyscale));
        memset(mapend, 0, sizeof(mapend));

        int camID = 0;
        for (unsigned int j = 0; j < sba_Players.size(); j++)
        {
            if (sba_Players[j]->PadID != -1)
            {
                if (sba_Players[j]->Ship->m_Life <= 0.0f)
                    greyscale[camID] = 1.0f;
                else
                {
                    float dist = (TheBrick::OngToPuRe(sba_Players[j]->Ship->m_pBody->getWorldCenter()) - this->m_Origin).Length();
                    if (dist > this->m_OriginDistance)
                    {
                        mapend[camID] = true;
                        greyscale[camID] = 0.5f;
                    }
                }
                camID++;

            }
        }


        /////////////  DRAW Light  ///////////////////////
        for (auto light : this->m_Lights)
        {
            switch (light->Type)
            {
            case ELightType::Directional:
                sba_Renderer->Draw(0, true, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, light->Position, light->Color);
                break;
            case ELightType::Point:
                sba_Renderer->Draw(0, true, this->m_pPointLight, this->m_pPointLightMaterial, light->Position, light->Color);
                break;
                default:
                break;
            }
        }
        /////////////  DRAW SKY  /////////////////////// 
        for (int i = 0; i < this->m_LocalPlayers; i++)
            sba_Renderer->Set(0, greyscale[i], "greyscale", i);
        sba_Renderer->Draw(0, true, sba_SkyBox, sba_SkyBoxMaterial);
        ////////////////////////////////////////////////////

        /////////////  DRAW BRICKS  ///////////////////////
        sba_BrickManager->Render();
        ////////////////////////////////////////////////////

        /////////////  DRAW Shield  ///////////////////////
        PuRe_IVertexBuffer* sphereBuffer = this->m_pSphere->GetBuffer();
        ////////////////////////////////////////////////////

        /////////////  DRAW Particles  ///////////////////////
        for (int i = 0; i < this->m_LocalPlayers; i++)
            sba_Renderer->Draw(1, false, this->m_SpaceEmitter[i], this->m_pParticleMaterial, this->m_pParticle1Sprite);
        //bullet emitter
        for (auto bullet : this->m_Bullets)
            bullet->DrawEmitter(this->m_pParticle1Sprite, this->m_pParticleMaterial, this->m_pPointLight, this->m_pPointLightMaterial);
        //player emitter
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->Ship->m_Shield != 0)
            {
                auto b = sba_Players[i]->Ship->m_pBody;
                auto tree = b->getBVTree();
                float length = ong::length(tree->aabb.e)*1.2f;
                PuRe_Vector3F pos = TheBrick::OngToPuRe(sba_Players[i]->Ship->m_pBody->getWorldCenter());
                sba_Renderer->Set(0, 1.0f, "intensity");
                sba_Renderer->Draw(0, true, sphereBuffer, sphereBuffer->GetSize(), PuRe_Primitive::Triangles, this->m_pShieldMaterial, pos, PuRe_MatrixF(), PuRe_Vector3F(), PuRe_Vector3F(length, length, length));
            }
            sba_Players[i]->Ship->DrawEmitter(this->m_pParticle1Sprite, this->m_pEngineMaterial, this->m_pPointLight, this->m_pPointLightMaterial);
        }
        /////////////////////////////////////////////////

        ///////////  DRAW UI  ///////////////////////
        this->m_pUI->DisplayUI(this->m_pFont, this->m_pFontMaterial, this->m_EndTime, this->m_WonID, mapend);
        camID = 0;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
            {
                PuRe_QuaternionF rotation = this->m_Cameras[camID]->GetQuaternion();
                PuRe_Vector3F ppos = TheBrick::OngToPuRe(sba_Players[i]->Ship->m_pBody->getWorldCenter());
                PuRe_Vector3F campos = this->m_Cameras[camID]->GetPosition();
                PuRe_Vector3F up = this->m_Cameras[camID]->GetUp();
                PuRe_Vector3F forward = this->m_Cameras[camID]->GetForward();

                PuReEngine::Core::Plane<float> Left = this->m_Cameras[camID]->GetFrustrumPlane(PuReEngine::Core::Planes::Left);
                PuReEngine::Core::Plane<float> Right = this->m_Cameras[camID]->GetFrustrumPlane(PuReEngine::Core::Planes::Right);
                PuReEngine::Core::Plane<float> Top = this->m_Cameras[camID]->GetFrustrumPlane(PuReEngine::Core::Planes::Top);
                PuReEngine::Core::Plane<float> Bottom = this->m_Cameras[camID]->GetFrustrumPlane(PuReEngine::Core::Planes::Bottom);
                PuReEngine::Core::Plane<float> Near = this->m_Cameras[camID]->GetFrustrumPlane(PuReEngine::Core::Planes::Near);

                PuRe_Vector3F aux, normal, point,rot;

                for (unsigned int j = 0; j < sba_Players.size(); j++)
                {
                    if (sba_Players[j]->PadID != i)
                    {
                        PuRe_Vector3F pos = TheBrick::OngToPuRe(sba_Players[j]->Ship->m_pBody->getWorldCenter());
                        pos += TheBrick::OngToPuRe(ong::rotate(ong::vec3(0.0f, 5.0f, 0.0f), sba_Players[j]->Ship->m_pBody->getOrientation()));
                        PuRe_Vector3F diffPos = (ppos - pos);
                        float diff = diffPos.Length() / 500.0f;

                        rot = PuRe_Vector3F(0.0f, 0.0f, 180 * PuRe_DegToRad);

                        if (diff > 1.0f)
                            diff = 1.0f;
                        if (diff < 0.1f)
                            diff = 0.1f;
                        float size = 0.1f*diff;


                        PuRe_Vector3F CamToPos = PuRe_Vector3F::Normalize(campos - pos);
                        PuRe_Vector3F ScreenPos;
                        bool inside = true;
                        float Val = -0.3f;

                        if ((diff = PuRe_Vector3F::Dot(CamToPos, Top.Normal)) > Val) //top
                        {
                            ScreenPos.Y = 0.4f;
                            ScreenPos.X = -diff;
                            inside = false;
                            rot.Z = 0.0f;
                            //if (i == 0)
                            //    printf("top\n");
                        }
                        else if ((diff = PuRe_Vector3F::Dot(CamToPos, Near.Normal)) > Val) //behind
                        {
                            ScreenPos.Y = -0.4f;
                            ScreenPos.X = -diff;
                            inside = false;
                            rot.Z = 180 * PuRe_DegToRad;
                            //if (i == 0)
                            //    printf("behind\n");
                        }
                        else if ((diff = PuRe_Vector3F::Dot(CamToPos, Left.Normal)) > Val) //right
                        {
                            ScreenPos.X = 0.7f;
                            ScreenPos.Y = diff;
                            inside = false;
                            rot.Z = 270*PuRe_DegToRad;
                            //if (i == 0)
                            //    printf("right\n");
                        }
                        else if ((diff = PuRe_Vector3F::Dot(CamToPos, Right.Normal)) > Val) //left
                        {
                            ScreenPos.X = -0.7f;
                            ScreenPos.Y = -diff;
                            inside = false;
                            rot.Z = 90 * PuRe_DegToRad;
                            //if (i == 0)
                            //    printf("left\n");
                        }
                        else if((diff = PuRe_Vector3F::Dot(CamToPos, Bottom.Normal)) > Val) //bottom
                        {
                            ScreenPos.Y = -0.4f;
                            ScreenPos.X = diff;
                            inside = false;
                            rot.Z = 180 * PuRe_DegToRad;
                            //if (i == 0)
                            //    printf("bottom\n");
                        }

                        if (!inside)
                        {
                            ScreenPos = (ScreenPos*rotation);
                            pos = campos;
                            pos += forward;
                            pos += ScreenPos;
                            size = 0.0005f;
                        }

                        PuRe_QuaternionF rotq = PuRe_QuaternionF(rot);
                        rotation *= rotq;

                        sba_Renderer->Draw(1, false, this->m_pLockSprite, this->m_pSpriteMaterial, pos, rotation.GetMatrix(), PuRe_Vector3F(), PuRe_Vector3F(size, size, size), PuRe_Color(), PuRe_Vector2F(), PuRe_Vector2F(), camID);
                    }
                }

                ///////////  DRAW SPHERE  ///////////////////////
                float intensity = (this->m_Cameras[camID]->GetPosition() - this->m_Origin).Length() / this->m_OriginDistance;
                if (intensity > 1.0f) intensity = 1.0f;
                intensity *= intensity;
                sba_Renderer->Set(0, intensity, "intensity", camID);
                if (intensity < 1.0f)
                    sba_Renderer->SetCulling(0, false, camID);
                sba_Renderer->Draw(0, true, sphereBuffer, sphereBuffer->GetSize(), PuRe_Primitive::Triangles, this->m_pShieldMaterial, this->m_Origin, PuRe_MatrixF(), PuRe_Vector3F(), PuRe_Vector3F(this->m_OriginDistance, this->m_OriginDistance, this->m_OriginDistance), PuRe_Color(), camID);
                sba_Renderer->SetCulling(0, true, camID);
                /////////////////////////////////////////////////
                camID++;
            }
        }
        //////////////////////////////////////////////////

        ////////////////// POST SCREEN ////////////////////////////////
		ong_START_PROFILE(POST_SCREEN)

        sba_Renderer->Set(0, (float)this->m_TextureID, "textureID");
        PuRe_Vector3F size = PuRe_Vector3F(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < this->m_LocalPlayers; i++)
        {
            switch (this->m_LocalPlayers)
            {
            case 1:
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Set(0, greyscale[i], "greyscale", i);
                sba_Renderer->Render(i, 0, this->m_Cameras[i], this->m_pPostMaterial, sba_FinalMaterial, size);
                sba_Renderer->Render(i, 1, this->m_Cameras[i], this->m_pUIMaterial, sba_FinalMaterial, size);
                sba_Renderer->Render(i, 2, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);
                break;
            case 2:
                size.X = i*gdesc.ResolutionWidth / 2.0f;
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Set(0, greyscale[i], "greyscale", i);
                sba_Renderer->Render(i, 0, this->m_Cameras[i], this->m_pPostMaterial, sba_FinalMaterial, size);
                sba_Renderer->Render(i, 1, this->m_Cameras[i], this->m_pUIMaterial, sba_FinalMaterial, size);
                sba_Renderer->Render(i, 2, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);
                break;
            default:
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Set(0, greyscale[i], "greyscale", i);
                size.X = (i % 2)*gdesc.ResolutionWidth / 2.0f;
                size.Y = (1 - ((int)(i / 2)))*gdesc.ResolutionHeight / 2.0f;
                sba_Renderer->Render(i, 0, this->m_Cameras[i], this->m_pPostMaterial, sba_FinalMaterial, size);
                sba_Renderer->Render(i, 1, this->m_Cameras[i], this->m_pUIMaterial, sba_FinalMaterial, size);
                sba_Renderer->Render(i, 2, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);
                break;
            }
        }
        if (this->m_LocalPlayers == 3)
        {
            size.X = gdesc.ResolutionWidth / 2.0f;
            size.Y = 0.0f;
            sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
            sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
            sba_Renderer->Render(3, 0, this->m_Cameras[3], this->m_pPostMaterial, sba_FinalMaterial, size);
            sba_Renderer->Render(3, 1, this->m_Cameras[3], this->m_pUIMaterial, sba_FinalMaterial, size);
        }



        size.X = 0.0f;
        size.Y = 0.0f;
        sba_Renderer->Render(0, 3, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);

        sba_Renderer->End();

		ong_END_PROFILE(POST_SCREEN)
        ////////////////////////////////////////////////////

		ong_END_PROFILE(RENDER);
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Exit()
    {

        ong_PRINT_PROFILE(stdout);

        sba_SoundPlayer->SetListeners(1);
        sba_SoundPlayer->SetListenPosition(0, PuRe_Vector3F(), PuRe_Vector3F(), PuRe_Vector3F(0.0f, 0.0f, 1.0f), PuRe_Vector3F(0.0f, 1.0f, 0.0f));

        if (sba_Network->IsConnected())
            SAFE_DELETE(this->m_pNetwork);
        SAFE_DELETE(this->m_pUI);
        SAFE_DELETE(this->m_pLockSprite);
        SAFE_DELETE(this->m_pParticle1Sprite);
        SAFE_DELETE(this->m_pParticle2Sprite);
        SAFE_DELETE(this->m_pParticle3Sprite);
        for (unsigned int i = 0; i < this->m_SpaceEmitter.size(); i++)
            SAFE_DELETE(this->m_SpaceEmitter[i]);
        // DELETE MATERIALS
        SAFE_DELETE(this->m_pShieldMaterial);
        SAFE_DELETE(this->m_pSpriteMaterial);
        SAFE_DELETE(this->m_pParticleMaterial);
        SAFE_DELETE(this->m_pEngineMaterial);
        SAFE_DELETE(this->m_pDirectionalLightMaterial);
        SAFE_DELETE(this->m_pPointLightMaterial);
        SAFE_DELETE(this->m_pFontMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pUIMaterial);
        // DELETE OBJECTS
        for (unsigned int i = 0; i < this->m_Items.size(); i++)
            SAFE_DELETE(this->m_Items[i]);
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            SAFE_DELETE(this->m_Bullets[i]);
        for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
            SAFE_DELETE(this->m_Asteroids[i]);
        for (unsigned int i = 0; i < this->m_Lights.size(); i++)
            SAFE_DELETE(this->m_Lights[i]);
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            SAFE_DELETE(sba_Players[i]->Ship);
            SAFE_DELETE(sba_Players[i]);
        }
        sba_Players.clear();
        SAFE_DELETE(this->m_pSphere);
        SAFE_DELETE(this->m_pPointLight);
        SAFE_DELETE(this->m_pDirectionalLight);
        // DELETE CAMERAS
        for (unsigned int i = 0; i < this->m_Cameras.size(); i++)
            SAFE_DELETE(this->m_Cameras[i]);
        SAFE_DELETE(this->m_pUICam);
        // DELETE RENDERER
        SAFE_DELETE(this->m_pFont);
        sba_BrickManager->RebuildRenderInstances();
        // reset destruction
        sba_Space->DestructionManager->Reset();
        sba_Space->ClearMiscObjects();

    }
}