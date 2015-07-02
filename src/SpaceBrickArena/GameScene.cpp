#include "include/GameScene.h"

#include "TheBrick/Conversion.h"

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

        float fShoot = sba_Input->Axis(Input::EAxis::Type::GameShoot, a_PlayerIdx);
        bool bShoot = false;
        if (a_PlayerIdx == 0)
            bShoot = sba_Input->ButtonIsPressed(Input::EButton::Type::GameShoot,a_PlayerIdx);

        if (bShoot||fShoot == 1.0f)
            input.Shoot = true;

        PuRe_Vector2F Move = sba_Input->Direction(Input::EDirection::Type::GameMove, a_PlayerIdx);
        if (Move.X > 0.1f || Move.X < -0.1f)
            input.MoveX = (char)(Move.X*100.0f);
        if (Move.Y > 0.1f || Move.Y < -0.1f)
            input.MoveY = (char)(Move.Y*100.0f);



        if (a_PlayerIdx == 0)
        {
            if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameThrust, a_PlayerIdx))
                input.Thrust = 100.0f;
        }
        float fThrust = sba_Input->Axis(Input::EAxis::Type::GameThrust, a_PlayerIdx);
        if (fThrust > 0.1f)
            input.Thrust = fThrust*100.0f;

        float Spin = sba_Input->Axis(Input::EAxis::Type::GameSpin, a_PlayerIdx);
        if (Spin < -0.1f || Spin > 0.1f)
            input.Spin = (char)(Spin*100.0f);

        return input;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::ProcessInput(std::vector<CBullet*>& a_rBullets, SPlayer* a_pPlayer, sba::SInputData* a_Input, float a_DeltaTime)
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
        PuRe_Vector3F forward = TheBrick::OngToPuRe(ong::rotate(ong::vec3(0, 0, 1), ship->m_pBody->getOrientation()));
        //PuRe_Vector3F forward = PuRe_Vector3F(0.0f,0.0f,1.0f)*this->m_Cameras[camID]->GetAim();
        PuRe_Vector3F forw = PuRe_Vector3F(forward.X,forward.Y,forward.Z);
        if (a_pPlayer->m_ShootCooldown == 0.0f&&a_Input->Shoot)
        {
            ship->Shoot(a_rBullets, a_pPlayer, forw);
            a_pPlayer->m_ShootCooldown = 0.25f;
        }
        else if (a_pPlayer->m_ShootCooldown != 0.0f)
        {
            a_pPlayer->m_ShootCooldown-= a_DeltaTime;
            if (a_pPlayer->m_ShootCooldown < 0.0f)
                a_pPlayer->m_ShootCooldown = 0.0f;
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
            ship->Spin(a_Input->Spin/100.0f);

        ship->Update(a_DeltaTime);
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
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
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
        this->UpdateGame(this->m_Bullets, this->m_Items, seconds);
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            sba::SInputData input = this->HandleInput(sba_Players[i]->PadID,aInput);
            this->ProcessInput(this->m_Bullets, sba_Players[i], &input, seconds);
        }
        sba::Space::Instance()->UpdatePhysics(aTimer);
        this->m_EndTime -= seconds;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::HandleNetwork()
    {
        this->m_pNetwork->UpdateNetwork(this->m_Bullets, this->m_Items,this->m_pApplication->GetInput(), this->m_pApplication->GetTimer()->GetElapsedSeconds(), this->m_EndTime, &CGameScene::HandleInput, &CGameScene::ProcessInput, &CGameScene::UpdateGame);
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::StartGame()
    {
        if (sba_Network->IsConnected())
            this->m_pNetwork->Initialize();

        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            sba_Players[i]->Ship->CalculateData();
            ong::vec3 pos = ong::vec3(10.0f, 10.0f, 10.0f);
            pos.x += sba_Players[i]->ID*30.0f;
            sba_Players[i]->Ship->m_pBody->setPosition(pos);
        }

        if (!sba_Map->GetMapData(this->m_Asteroids,this->m_Items) ) //Map doesn't exist!! we end here
            this->m_EndTime = -1000;

        this->m_TimeLimit = 5.0f;
        this->m_EndTime = 60.0f*this->m_TimeLimit; //seconds * Minutes
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
        sba_Renderer->AddTarget(PuRe_Vector2I(gdesc.ResolutionWidth, gdesc.ResolutionHeight));
        if (sba_Options->GetValue("SSAO") == "On")
        {
            sba_Renderer->SetSSAO(0, sba_Space->m_SSAOMaterial, sba_Space->m_pNoiseTexture);
        }



        this->m_pFontMaterial = graphics->LoadMaterial("../data/effects/font/default");
        this->m_pUIMaterial = graphics->LoadMaterial("../data/effects/UI/default");
        this->m_pPostMaterial = graphics->LoadMaterial("../data/effects/Post/default");
        this->m_pSkyMaterial = graphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pPointLightMaterial = graphics->LoadMaterial("../data/effects/PointLight/default");
        this->m_pDirectionalLightMaterial = graphics->LoadMaterial("../data/effects/DirectionalLight/default");
        this->m_pParticleMaterial = graphics->LoadMaterial("../data/effects/particles/default");
        this->m_pPointLight = new PuRe_PointLight(graphics);
        this->m_pDirectionalLight = new PuRe_DirectionalLight(graphics);
        this->m_pFont = new PuRe_Font(graphics, "../data/textures/font.png");
        this->m_pParticle1Sprite = new PuRe_Sprite(graphics, "../data/textures/partikel_1.png");
        this->m_pParticle2Sprite = new PuRe_Sprite(graphics, "../data/textures/partikel_2.png");
        this->m_pParticle3Sprite = new PuRe_Sprite(graphics, "../data/textures/partikel_3.png");
        this->m_pUICam = new PuRe_Camera(fsize, PuRe_CameraProjection::Orthogonal);

        this->m_pUI = new CGUI(graphics);

        //Create for each player a camera
        for (int i = 0; i<this->m_LocalPlayers; i++)
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

        #ifdef Skybox
            this->m_pSkyBox = new PuRe_SkyBox(graphics, sba_Map->GetSkybox());
        #endif

            if (sba_Network->IsConnected())
                this->m_pNetwork = new CGameNetwork();

        this->StartGame();

    }

    // **************************************************************************
    // **************************************************************************
    int CGameScene::Update(PuRe_Application* a_pApplication)
    {
        PuRe_Timer* timer = a_pApplication->GetTimer();
        if (sba_Network->IsConnected())
        {
            this->m_pNetwork->Update(timer->GetElapsedSeconds());
        }

        //Handle ESC Button
        if (this->m_EndTime+10.0f < 0.0f||a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC))
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


        if (sba_Network->IsConnected())
            this->HandleNetwork();
        else
            this->HandleLocal();

        int camID = 0;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
            {
                sba::CSpaceship* playerShip;
                if (camID == 0 && this->m_Test != -1)
                    playerShip = sba_Players[this->m_Test]->Ship;
                else
                    playerShip = sba_Players[i]->Ship;
                this->m_Cameras[camID]->UpdateData(sba_Players[i]->PadID, playerShip, a_pApplication->GetInput(), a_pApplication->GetTimer());
                PuRe_QuaternionF rotation = this->m_Cameras[camID]->GetQuaternion();

                if (this->m_SpaceEmitter[camID]->GetAmount() < 200)
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
                if (sba_Players[i]->m_Points > sba_Players[this->m_WonIndex]->m_Points)
                {
                    this->m_WonIndex = i;
                    this->m_WonID = sba_Players[i]->ID;
                    points = sba_Players[i]->m_Points;

                }
            }
            this->m_Won = true;
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
            sba_Renderer->Draw(0, true, this->m_SpaceEmitter[i], this->m_pParticleMaterial, this->m_pParticle1Sprite);
        //player emitter
        for (int i = 0; i<sba_Players.size(); i++)
            sba_Players[i]->Ship->DrawEmitter(this->m_pParticle1Sprite, this->m_pParticleMaterial);
        ////////////////////////////////////////////////////

        /////////////  DRAW UI  ///////////////////////
        this->m_pUI->DisplayUI(this->m_pFont, this->m_pFontMaterial, this->m_EndTime, this->m_WonID);
        ////////////////////////////////////////////////////

        //////////////////// POST SCREEN ////////////////////////////////
        sba_Renderer->Set(0, (float)this->m_TextureID, "textureID");
        PuRe_Vector3F size = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < this->m_LocalPlayers; i++)
        {
            switch (this->m_LocalPlayers)
            {
            case 1:
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                sba_Renderer->Render(0,0, this->m_Cameras[i], this->m_pPostMaterial,sba_FinalMaterial, size);
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Render(i, 1, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);
                break;
            case 2:
                size.X = i*gdesc.ResolutionWidth / 2.0f;
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                sba_Renderer->Render(0, 0, this->m_Cameras[i], this->m_pPostMaterial, sba_FinalMaterial, size);
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Render(i, 1, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);
                break;
            default:
                sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
                size.X = (i % 2)*gdesc.ResolutionWidth / 2.0f;
                size.Y = (1 - ((int)(i / 2)))*gdesc.ResolutionHeight / 2.0f;
                sba_Renderer->Render(0, 0, this->m_Cameras[i], this->m_pPostMaterial, sba_FinalMaterial, size);
                sba_Renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
                sba_Renderer->Render(i, 1, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);
                break;
            }
        }
        if (this->m_LocalPlayers == 3)
        {
            size.X = gdesc.ResolutionWidth / 2.0f;
            size.Y = 0.0f;
            sba_Renderer->Set(0, PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
            sba_Renderer->Render(0, 0, this->m_Cameras[3], this->m_pPostMaterial, sba_FinalMaterial, size);
        }
        size.X = 0.0f;
        size.Y = 0.0f;
        sba_Renderer->Render(0, 2, this->m_pUICam, this->m_pUIMaterial, sba_FinalMaterial, size);
        sba_Renderer->End();
        ////////////////////////////////////////////////////


    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Exit()
    {
        if (sba_Network->IsConnected())
            SAFE_DELETE(this->m_pNetwork);
        SAFE_DELETE(this->m_pUI);
        SAFE_DELETE(this->m_pParticle1Sprite);
        SAFE_DELETE(this->m_pParticle2Sprite);
        SAFE_DELETE(this->m_pParticle3Sprite);
        for (unsigned int i = 0; i < this->m_SpaceEmitter.size(); i++)
            SAFE_DELETE(this->m_SpaceEmitter[i]);
        // DELETE MATERIALS
        SAFE_DELETE(this->m_pParticleMaterial);
        SAFE_DELETE(this->m_pDirectionalLightMaterial);
        SAFE_DELETE(this->m_pPointLightMaterial);
        SAFE_DELETE(this->m_pFontMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pSkyMaterial);
        SAFE_DELETE(this->m_pUIMaterial);
        // DELETE OBJECTS
        for (unsigned int i = 0; i < this->m_Items.size(); i++)
            SAFE_DELETE(this->m_Items[i]);
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            SAFE_DELETE(this->m_Bullets[i]);
        for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
            SAFE_DELETE(this->m_Asteroids[i]);
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            SAFE_DELETE(sba_Players[i]->Ship);
            SAFE_DELETE(sba_Players[i]);
        }
        sba_Players.clear();
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pPointLight);
        SAFE_DELETE(this->m_pDirectionalLight);
        // DELETE CAMERAS
        for (unsigned int i = 0; i < this->m_Cameras.size(); i++)
            SAFE_DELETE(this->m_Cameras[i]);
        SAFE_DELETE(this->m_pUICam);
        // DELETE RENDERER
        SAFE_DELETE(this->m_pFont);
        sba_BrickManager->RebuildRenderInstances();
    }
}