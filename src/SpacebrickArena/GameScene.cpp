#include "include/GameScene.h"

#include "TheBrick/Conversion.h"
#include "include/InputManager.h"
#include "include/GameMap.h"
#include "include/BrickManager.h"
#include "include/DestructionManager.h"


#include "Onager\Profiler.h"


const float g_Rand[] = { -56.0f, -250.0f, 45.0f, -360.0f, 106.0f,
256.0f, -221.0f, -343.0f, 281.0f, -91.0f,
-328.0f, 132.0f, 175.0f, 318.0f, 40.0f,
306.0f, -316.0f, 231.0f, -157.0f, 159.0f,
-150.0f, -150.0f, -400.0f, 183.0f, 145.0f,
-167.0f, 85.0f, -100.0f, 174.0f, -40.0f,
-134.0f, -142.0f, 66.0f, 184.0f, 363.0f,
397.0f, 38.0f, 358.0f, -355.0f, -201.0f,
-88.0f, -196.0f, -302.0f, 108.0f, 252.0f,
-286.0f, 212.0f, 337.0f, 215.0f, -365.0f,
-340.0f, -383.0f, 322.0f, -226.0f, 274.0f,
-306.0f, -1.0f, -256.0f, 351.0f, -177.0f,
147.0f, -201.0f, 384.0f, -224.0f, -251.0f,
-135.0f, -290.0f, 198.0f, 223.0f, -337.0f,
283.0f, -282.0f, -11.0f, -323.0f, 330.0f,
-232.0f, -321.0f, -117.0f, -150.0f, -19.0f,
-184.0f, 175.0f, 276.0f, 90.0f, 198.0f,
189.0f, 361.0f, 107.0f, -187.0f, -438.0f,
-380.0f, -298.0f, 299.0f, 298.0f, -137.0f,
367.0f, 107.0f, 254.0f, 1.0f, 300.0f };


PuRe_Vector3F g_Origin = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
float g_OriginDistance = 750.0f;

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx, bool a_Network)
    {
        this->m_pApplication = a_pApplication;
        this->m_Pause = false;
        this->m_PauseEnd = false;
    }

    // **************************************************************************
    // **************************************************************************
    sba::SInputData CGameScene::HandleInput(int a_PlayerIdx, PuRe_IInput* a_pInput)
    {
        sba::SInputData input;
        memset(&input, 0, sizeof(sba::SInputData));

        input.Shoot = sba_Input->Axis(Input::EAxis::Type::GameShoot, a_PlayerIdx) > 0.0f || sba_Input->ButtonIsPressed(Input::EButton::Type::GameShoot,a_PlayerIdx);
        input.Weapon = -1;
        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameUseMG, a_PlayerIdx))
            input.Weapon = 0;
        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameUseLaser, a_PlayerIdx))
            input.Weapon = 1;
        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameUseRocket, a_PlayerIdx))
            input.Weapon = 2;
        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameUseMine, a_PlayerIdx))
            input.Weapon = 3;

        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootMG, a_PlayerIdx))
        {
            input.Weapon = 0;
            input.Shoot = true;
        }
        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootLaser, a_PlayerIdx))
        {
            input.Weapon = 1;
            input.Shoot = true;
        }
        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootRocket, a_PlayerIdx))
        {
            input.Weapon = 2;
            input.Shoot = true;
        }
        if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameShootMine, a_PlayerIdx))
        {
            input.Weapon = 3;
            input.Shoot = true;
        }

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
        if (Move.X > 1.0f) Move.X = 1.0f; else if (Move.X < -1.0f) Move.X = -1.0f;
        if (Move.Y > 1.0f) Move.Y = 1.0f; else if (Move.Y < -1.0f) Move.Y = -1.0f;

        input.MoveX = (char)(Move.X*100.0f);
        input.MoveY = -(char)(Move.Y*100.0f);



        float fThrust;
        if (a_PlayerIdx == 0)
        {
            if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameThrust, a_PlayerIdx))
                fThrust = 100;
        }

        if (sba_Controls[a_PlayerIdx].Thrust == 1)
        {
            fThrust = sba_Input->Axis(Input::EAxis::Type::GameThrust_1, a_PlayerIdx);
            if (sba_Input->ButtonIsPressed(Input::EButton::Type::GameThrustB_1, a_PlayerIdx))
                fThrust -= 1.0f;
        }
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

        PuRe_Vector2F Move = PuRe_Vector2F(0.0f, 0.0f);
        if (a_Input->MoveX != 0)
            Move.X = a_Input->MoveX / 100.0f;
        if (a_Input->MoveY != 0)
            Move.Y = a_Input->MoveY / 100.0f;
        ship->Move(Move);

        if (a_Input->Weapon != -1)
            a_pPlayer->Weapon = a_Input->Weapon;

        float* cd = &a_pPlayer->MGCD;
        if (a_Input->Shoot&&*cd == 0.0f&&a_pPlayer->Weapon == 0)
        {
            ship->Shoot(TheBrick::MG - 100, a_rBullets, a_pPlayer, Move, a_Time);
            *cd = std::stof(sba_Balancing->GetValue("MG_CD"));
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }
        cd = &a_pPlayer->LaserCD;
        if (a_Input->Shoot&&*cd == 0.0f&&a_pPlayer->Weapon == 1)
        {
            ship->Shoot(TheBrick::Laser - 100, a_rBullets, a_pPlayer, Move, a_Time);
            *cd = std::stof(sba_Balancing->GetValue("Laser_CD"));
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }
        cd = &a_pPlayer->RocketCD;
        if (a_Input->Shoot&&*cd == 0.0f&&a_pPlayer->Weapon == 2)
        {
            ship->Shoot(TheBrick::Rocket - 100, a_rBullets, a_pPlayer, Move, a_Time);
            *cd = std::stof(sba_Balancing->GetValue("Rocket_CD"));
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }
        cd = &a_pPlayer->MineCD;
        if (a_Input->Shoot&&*cd == 0.0f&&a_pPlayer->Weapon == 3)
        {
            ship->Shoot(TheBrick::Mine - 100, a_rBullets, a_pPlayer, Move, a_Time);
            *cd = std::stof(sba_Balancing->GetValue("Mine_CD"));
        }
        else if (*cd != 0.0f)
        {
            *cd -= a_DeltaTime;
            if (*cd < 0.0f)
                *cd = 0.0f;
        }

        if (a_Input->Thrust != 0)
            ship->Thrust(a_Input->Thrust / 100.0f);

        if (a_Input->Spin != 0)
            ship->Spin(a_Input->Spin / 100.0f);

        ship->Update(a_pPlayer->ID, a_DeltaTime, a_Time);
    }

    void CGameScene::UpdateGame(std::vector<SItemData>& a_rRespawnItems, std::vector<CAsteroid*>& a_rAsteroids, std::vector<ExplosionEmitter>& a_rExplosions, std::vector<CBullet*>& a_rBullets, std::vector<CItem*>& a_rItems, float a_Deltatime, int a_Time)
    {
        //Update Emitter
        for (unsigned int i = 0; i < a_rExplosions.size(); i++)
        {
            a_rExplosions[i].LifeTime -= a_Deltatime;
            if (a_rExplosions[i].LifeTime < 0.0f&&a_rExplosions[i].pEmitter->GetAmount() == 0)
            {
                SAFE_DELETE(a_rExplosions[i].pEmitter);
                if (a_rExplosions.begin() + i < a_rExplosions.end())
                    a_rExplosions.erase(a_rExplosions.begin() + i);
                i--;
            }
            else
            { 
                unsigned int amount = 0;
                float speed = 0.0f;

                switch (a_rExplosions[i].Bullet)
                {

                case TheBrick::Laser: //Laser
                    amount = 0;
                    speed = 0.0f;
                    break;

                case TheBrick::MG: //MG
                    amount = 50;
                    speed = 6000.0f;
                    break;

                case TheBrick::Mine: //Mine
                    amount = 80;
                    speed = 9000.0f;
                    break;

                case TheBrick::Rocket: //Rocket
                    amount = 70;
                    speed = 8000.0f;
                    break;
                }
                if (a_rExplosions[i].LifeTime > 0.0f)
                {
                    int time = a_Time % 100;
                    for (unsigned int j = 0; j<amount; j++)
                    {
                        int index1 = time + j;
                        int index2 = index1 + 1;
                        int index3 = index2 + 2;
                        if (index1 > 100) index1 = index1 - 100;
                        if (index2 > 100) index2 = index2 - 100;
                        if (index3 > 100) index3 = index3 - 100;
                        a_rExplosions[i].pEmitter->Spawn(0.5f, PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f), PuRe_Vector3F(g_Rand[index1] / speed, g_Rand[index2] / speed, g_Rand[index3] / speed), PuRe_QuaternionF(g_Rand[index1] / 400.0f, g_Rand[index2] / 400.0f, g_Rand[index3] / 400.0f), PuRe_Color(1.0f, 0.0f, 0.0f, 1.0f));
                    }
                }
                a_rExplosions[i].pEmitter->Update(a_Deltatime);
            }
        }
        //Update Bullets
        for (unsigned int i = 0; i < a_rBullets.size(); i++)
        {
            if (a_rBullets[i]->m_Collided||a_rBullets[i]->m_lifeTime == 4.9f)
            {
                ExplosionEmitter eEmitter;
                eEmitter.pEmitter = new PuRe_ParticleEmitter(TheBrick::OngToPuRe(a_rBullets[i]->m_pBody->getWorldCenter()), PuRe_QuaternionF());
                eEmitter.LifeTime = 0.25f;
                eEmitter.Bullet = a_rBullets[i]->m_ID;
                a_rExplosions.push_back(eEmitter);
            }
            a_rBullets[i]->Update(a_Deltatime,a_rBullets);

            float maxLifeTime = 0.0f;

            switch (a_rBullets[i]->m_ID)
            {

            case TheBrick::Laser: //Laser
                maxLifeTime = std::stof(sba_Balancing->GetValue("Laser_LifeTime"));
                break;

            case TheBrick::MG: //MG
                maxLifeTime = std::stof(sba_Balancing->GetValue("MG_LifeTime"));
                break;

            case TheBrick::Mine: //Mine
                maxLifeTime = std::stof(sba_Balancing->GetValue("Mine_LifeTime"));
                break;

            case TheBrick::Rocket: //Rocket
                maxLifeTime = std::stof(sba_Balancing->GetValue("Rocket_LifeTime"));
                break;
            }
            if (a_rBullets[i]->m_Collided || a_rBullets[i]->m_lifeTime > maxLifeTime)
            {
                SAFE_DELETE(a_rBullets[i]);
                if (a_rBullets.begin() + i < a_rBullets.end())
                    a_rBullets.erase(a_rBullets.begin() + i);
                i--;
            }
        }
        //update respawn items
        for (unsigned int i = 0; i < a_rRespawnItems.size(); i++)
        {
            a_rRespawnItems[i].Respawn -= a_Deltatime;
            if (a_rRespawnItems[i].Respawn < 0.0f)
            {
                ong::BodyDescription a_desc;
                a_desc.transform = ong::Transform(a_rRespawnItems[i].Pos, ong::Quaternion(ong::vec3(0, 0, 0), 1));
                a_desc.type = ong::BodyType::Static;
                a_desc.continuousPhysics = false;
                a_desc.angularMomentum = a_rRespawnItems[i].Rot;
                a_desc.linearMomentum = a_rRespawnItems[i].Vel;
                CItem* item = new CItem(*sba_World, a_rRespawnItems[i].Type, &a_desc);
                a_rItems.push_back(item);
                if (a_rRespawnItems.begin() + i < a_rRespawnItems.end())
                    a_rRespawnItems.erase(a_rRespawnItems.begin() + i);
                i--;
            }
        }
        //Update Items
        for (unsigned int i = 0; i < a_rItems.size(); i++)
        {
            if (a_rItems[i]->m_Collided)
            {
                SItemData item;
                item.Pos = a_rItems[i]->m_pBody->getWorldCenter();
                item.Vel = a_rItems[i]->m_pBody->getLinearMomentum();
                item.Rot = a_rItems[i]->m_pBody->getAngularMomentum();
                item.Type = a_rItems[i]->m_ItemType;
                item.Respawn = 60.0f;
                a_rRespawnItems.push_back(item);
                SAFE_DELETE(a_rItems[i]);
                if (a_rItems.begin() + i < a_rItems.end())
                    a_rItems.erase(a_rItems.begin() + i);
                i--;
            }
        }



        //Delete objects outside
        ong::vec3 origin = TheBrick::PuReToOng(g_Origin);
        sba_Space->DelMiscOutside(origin, g_OriginDistance);
        for (unsigned int i = 0; i < a_rAsteroids.size(); i++)
        {
            if (ong::length(a_rAsteroids[i]->m_pBody->getWorldCenter() - origin) > g_OriginDistance)
            {
                delete a_rAsteroids[i];
                a_rAsteroids.erase(a_rAsteroids.begin() + i);
                --i;
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
                sba_Players[i]->Update(a_Deltatime, g_Origin,g_OriginDistance);
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
        int tseconds = (int)aTimer->GetTotalElapsedMilliseconds();
        if (seconds < 1.0f)
        {
            //Lagg avoid
            if (this->m_StartTimer > 0.2f)
            {

                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    sba::SInputData input = this->HandleInput(sba_Players[i]->PadID, aInput);
                    this->ProcessInput(this->m_Bullets, sba_Players[i], &input, seconds, tseconds);
                }


                ong_START_PROFILE(UPDATE_GAME);
                this->UpdateGame(this->m_RespawnItems,this->m_Asteroids,this->m_ExplosionEmitter, this->m_Bullets, this->m_Items, seconds, tseconds);
                ong_END_PROFILE(UPDATE_GAME);

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
        if (!this->m_pNetwork->UpdateNetwork(this->m_RespawnItems, this->m_Asteroids, this->m_ExplosionEmitter, this->m_Bullets, this->m_Items, this->m_pApplication->GetInput(), this->m_pApplication->GetTimer()->GetElapsedSeconds(), this->m_EndTime, &CGameScene::HandleInput, &CGameScene::ProcessInput, &CGameScene::UpdateGame))
            this->m_EndTime = -100.0f;

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
            ong::vec3 pos;
            ong::Quaternion rot;
            if (sba_Players[i]->ID == 0)
            {
                pos = ong::vec3(0.0f, 0.0f, -300.0f);
                rot = ong::QuatFromEulerAngles(0.0f,0.0f,0.0f);
            }
            else if (sba_Players[i]->ID == 1)
            {
                pos = ong::vec3(0.0f, 0.0f, 300.0f);
                rot = ong::QuatFromEulerAngles(180.0f*PuRe_DegToRad,0.0f,0.0f);
            }
            else if (sba_Players[i]->ID == 2)
            {
                pos = ong::vec3(300.0f, 0.0f, 0.0f);
                rot = ong::QuatFromEulerAngles(270.0f*PuRe_DegToRad, 0.0f, 0.0f);
            }
            else if (sba_Players[i]->ID == 3)
            {
                pos = ong::vec3(-300.0f, 0.0f, 0.0f);
                rot = ong::QuatFromEulerAngles(90.0f*PuRe_DegToRad, 0.0f, 0.0f);
            }
            sba_Players[i]->Ship->m_pBody->setPosition(pos);
            sba_Players[i]->Ship->m_pBody->setOrientation(rot);
            //Set default Weapon
            std::vector<TheBrick::CBrickInstance**> weapons;
            sba_Players[i]->Ship->GetWeapons(weapons);
            //For a Priority
            bool hasMG = false;
            bool hasLaser = false;
            bool hasRocket = false;

            //Go through weapons
            for (std::vector<TheBrick::CBrickInstance**>::iterator it = weapons.begin(); it != weapons.end(); ++it)
            {
                TheBrick::CBrickInstance* weapon = *(*it);
                switch (weapon->m_pBrick->GetBrickId())
                {
                case TheBrick::MG - 100: //MG
                    hasMG = true;
                    break;

                case TheBrick::Laser - 100: //Laser
                    hasLaser = true;
                    break;

                case TheBrick::Rocket - 100: //Rocket
                    hasRocket = true;
                    break;
                }

            }
            //Priority Order
            if (hasMG)
                sba_Players[i]->Weapon = 0;
            else if (hasLaser)
                sba_Players[i]->Weapon = 1;
            else if (hasRocket)
                sba_Players[i]->Weapon = 2;
            else
                sba_Players[i]->Weapon = 3;

            if (sba_Players[i]->PadID != -1)
                listeners++;

        }

        sba_SoundPlayer->SetListeners(listeners);
        sba_SoundPlayer->SetListenPosition(0, PuRe_Vector3F(), PuRe_Vector3F(), PuRe_Vector3F(0.0f, 0.0f, 1.0f), PuRe_Vector3F(0.0f, 1.0f, 0.0f));

        if (!sba_Map->GetMapData(this->m_Statics,this->m_Asteroids, this->m_Items,this->m_Lights)) //Map doesn't exist!! we end here
            this->m_EndTime = -1000;

        this->m_TimeLimit = 5.0f;
        this->m_EndTime = 60.0f*this->m_TimeLimit; //seconds * Minutes
        this->m_StartTimer = 0.0f;

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

        bool otherSkyBox = false;

        if (sba_SkyBoxName != sba_Map->GetSkybox())
        {
            SAFE_DELETE(sba_SkyBox);
            sba_SkyBox = new PuRe_SkyBox(graphics, sba_Map->GetSkybox());
            sba_SkyBoxName = sba_Map->GetSkybox();
            otherSkyBox = true;
        }

        //Load balancing
        sba_Space->LoadCSV();

        this->m_pShieldMaterial = graphics->LoadMaterial("../data/effects/shield/default");
        this->m_pFontMaterial = graphics->LoadMaterial("../data/effects/font/default");
        this->m_pUIMaterial = graphics->LoadMaterial("../data/effects/UI/default");
        this->m_pPostMaterial = graphics->LoadMaterial("../data/effects/Post/default");
        this->m_pPointLightMaterial = graphics->LoadMaterial("../data/effects/PointLight/default");
        this->m_pDirectionalLightMaterial = graphics->LoadMaterial("../data/effects/DirectionalLight/default");
        this->m_pParticleMaterial = graphics->LoadMaterial("../data/effects/particles/default");
        this->m_pExplosionMaterial = graphics->LoadMaterial("../data/effects/explosion/default");
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

        if (sba_Network->IsConnected())
            this->m_pNetwork = new CGameNetwork();


        if (otherSkyBox)
            sba_SkyBox->JoinThread();

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




        if (sba_Network->IsConnected())
            this->HandleNetwork();
        else
            this->HandleLocal();
        //Only Update Graphics! For GamePlay, Use UpdateGame!! UpdateGame is Network-Safe
        int camID = 0;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
            {
                sba::CSpaceship* playerShip;
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
        //just update camera
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


        //set winner
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
        //pause/end
        bool Pause = sba_Input->ButtonPressed(Input::EButton::Type::GamePause, 0);
        bool Click = sba_Input->ButtonPressed(Input::EButton::Type::GameClick, 0);
        if (!this->m_Pause)
        {
            if (Pause)
            {
                this->m_Pause = true;
                this->m_PauseEnd = false;
            }
        }
        else
        {
            float axis = sba_Input->Axis(Input::EAxis::Type::GamePauseSwitch,0);
            if (axis > 0.0f)
                this->m_PauseEnd = false;
            else if (axis < 0.0f)
                this->m_PauseEnd = true;
            if (Click||Pause)
            {
                if (this->m_PauseEnd)
                {
                    if (sba_Network->IsConnected())
                    {
                        SAFE_DELETE(sba_Network);
                        sba_Network = new sba::CNetworkHandler(sba_Options);
                    }
                    return 5;
                }
                else
                    this->m_Pause = false;
            }
        }

        sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances

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
                    float dist = (TheBrick::OngToPuRe(sba_Players[j]->Ship->m_pBody->getWorldCenter()) - g_Origin).Length();
                    if (dist > g_OriginDistance)
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
                sba_Renderer->Draw(0, true, this->m_pPointLight, this->m_pPointLightMaterial, light->Position, light->Color,light->Radius,light->Intensity);
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
        //explosion emitter
        for (auto explosion : this->m_ExplosionEmitter)
            sba_Renderer->Draw(1, false, explosion.pEmitter, this->m_pExplosionMaterial, this->m_pParticle1Sprite,-1,0.25f);
        //player emitter
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (sba_Players[i]->Ship->m_Shield != 0)
            {
                auto b = sba_Players[i]->Ship->m_pBody;
                auto tree = b->getBVTree();
                float length = ong::length(tree->aabb.e)*1.2f;
                PuRe_Vector3F pos = TheBrick::OngToPuRe(sba_Players[i]->Ship->m_pBody->getWorldCenter());
                sba_Renderer->Set(1, 1.0f, "intensity");
                sba_Renderer->Draw(1, true, sphereBuffer, sphereBuffer->GetSize(), PuRe_Primitive::Triangles, this->m_pShieldMaterial, pos, PuRe_MatrixF(), PuRe_Vector3F(), PuRe_Vector3F(length, length, length));
            }
            sba_Players[i]->Ship->DrawEmitter(this->m_pParticle1Sprite, this->m_pEngineMaterial, this->m_pPointLight, this->m_pPointLightMaterial);
        }
        /////////////////////////////////////////////////

        ///////////  DRAW UI  ///////////////////////
        this->m_pUI->DisplayUI(this->m_pFont, this->m_pFontMaterial, this->m_EndTime, this->m_WonID, mapend,g_OriginDistance,&this->m_Cameras,this->m_Pause,this->m_PauseEnd);
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
                if (mapend[camID])
                    this->DisplayArrow(Left, Right, Top, Bottom, Near, forward, campos, ppos, PuRe_Vector3F(0.0f, 0.0f, 0.0f), rotation, camID);
                for (unsigned int j = 0; j < sba_Players.size(); j++)
                {
                    if (i != j)
                    {
                        PuRe_Vector3F pos = TheBrick::OngToPuRe(sba_Players[j]->Ship->m_pBody->getWorldCenter());
                        this->DisplayArrow(Left, Right, Top, Bottom, Near, forward, campos, ppos, pos, rotation, camID, j);
                    }
                }

                ///////////  DRAW SPHERE  ///////////////////////
                float intensity = (this->m_Cameras[camID]->GetPosition() - g_Origin).Length() / g_OriginDistance;
                if (intensity > 1.0f) intensity = 1.0f;
                intensity *= intensity;
                sba_Renderer->Set(1, intensity, "intensity", camID);
                if (intensity < 1.0f)
                    sba_Renderer->SetCulling(1, false, camID);
                sba_Renderer->Draw(1, true, sphereBuffer, sphereBuffer->GetSize(), PuRe_Primitive::Triangles, this->m_pShieldMaterial, g_Origin, PuRe_MatrixF(), PuRe_Vector3F(), PuRe_Vector3F(g_OriginDistance, g_OriginDistance, g_OriginDistance), PuRe_Color(), camID);
                sba_Renderer->SetCulling(1, true, camID);
                /////////////////////////////////////////////////
                camID++;
            }
        }
        //////////////////////////////////////////////////

        ////////////////// POST SCREEN ////////////////////////////////
		ong_START_PROFILE(POST_SCREEN)
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
    void CGameScene::DisplayArrow(PuReEngine::Core::Plane<float>& a_rLeft,
        PuReEngine::Core::Plane<float>& a_rRight,
        PuReEngine::Core::Plane<float>& a_rTop,
        PuReEngine::Core::Plane<float>& a_rBottom,
        PuReEngine::Core::Plane<float>& a_rNear,
    PuRe_Vector3F a_Forward, PuRe_Vector3F a_CamPos, PuRe_Vector3F a_PlayerPos, PuRe_Vector3F a_Position, PuRe_QuaternionF a_Rotation, int a_CamID,int a_Player)
    {
        if (a_Player != -1)
            a_Position += TheBrick::OngToPuRe(ong::rotate(ong::vec3(0.0f, 5.0f, 0.0f), sba_Players[a_Player]->Ship->m_pBody->getOrientation()));
        PuRe_Vector3F diffPos = (a_PlayerPos - a_Position);
        float diff = diffPos.Length() / 500.0f;

        PuRe_Vector3F rot = PuRe_Vector3F(0.0f, 0.0f, 180 * PuRe_DegToRad);

        if (diff > 1.0f)
            diff = 1.0f;
        if (diff < 0.1f)
            diff = 0.1f;
        float size = 0.1f*diff;


        PuRe_Vector3F CamToPos = PuRe_Vector3F::Normalize(a_CamPos - a_Position);
        PuRe_Vector3F ScreenPos;
        bool inside = true;
        float Val = -0.3f;

        if ((diff = PuRe_Vector3F::Dot(CamToPos, a_rTop.Normal)) > Val) //top
        {
            ScreenPos.Y = 0.4f;
            ScreenPos.X = -diff;
            inside = false;
            rot.Z = 0.0f;
        }
        else if ((diff = PuRe_Vector3F::Dot(CamToPos, a_rLeft.Normal)) > Val) //right
        {
            ScreenPos.X = 0.7f;
            ScreenPos.Y = diff;
            inside = false;
            rot.Z = 270 * PuRe_DegToRad;
        }
        else if ((diff = PuRe_Vector3F::Dot(CamToPos, a_rRight.Normal)) > Val) //left
        {
            ScreenPos.X = -0.7f;
            ScreenPos.Y = -diff;
            inside = false;
            rot.Z = 90 * PuRe_DegToRad;
        }
        else if ((diff = PuRe_Vector3F::Dot(CamToPos, a_rTop.Normal)) > Val) //top
        {
            ScreenPos.Y = 0.4f;
            ScreenPos.X = -diff;
            inside = false;
            rot.Z = 0.0f;
        }
        else if ((diff = PuRe_Vector3F::Dot(CamToPos, a_rNear.Normal)) > Val) //behind
        {
            ScreenPos.Y = -0.4f;
            ScreenPos.X = -diff;
            inside = false;
            rot.Z = 180 * PuRe_DegToRad;
        }
        else if ((diff = PuRe_Vector3F::Dot(CamToPos, a_rBottom.Normal)) > Val) //bottom
        {
            ScreenPos.Y = -0.4f;
            ScreenPos.X = diff;
            inside = false;
            rot.Z = 180 * PuRe_DegToRad;
        }

        if (!inside)
        {
            ScreenPos = (ScreenPos*a_Rotation);
            a_Position = a_CamPos;
            a_Position += a_Forward;
            a_Position += ScreenPos;
            size = 0.0005f;
        }

        PuRe_QuaternionF rotq = PuRe_QuaternionF(rot);
        a_Rotation *= rotq;

        sba_Renderer->Draw(1, false, this->m_pLockSprite, this->m_pSpriteMaterial, a_Position, a_Rotation.GetMatrix(), PuRe_Vector3F(), PuRe_Vector3F(size, size, size), PuRe_Color(), PuRe_Vector2F(), PuRe_Vector2F(), a_CamID);


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
        for (unsigned int i = 0; i < this->m_ExplosionEmitter.size(); i++)
            SAFE_DELETE(this->m_ExplosionEmitter[i].pEmitter);
        // DELETE MATERIALS
        SAFE_DELETE(this->m_pShieldMaterial);
        SAFE_DELETE(this->m_pSpriteMaterial);
        SAFE_DELETE(this->m_pParticleMaterial);
        SAFE_DELETE(this->m_pExplosionMaterial);
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
        for (unsigned int i = 0; i < this->m_Statics.size(); i++)
            SAFE_DELETE(this->m_Statics[i]);
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