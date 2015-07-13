#include "include/Space.h"

#include "Onager\Profiler.h"

namespace sba
{
    Space* Space::g_pInstance = 0;

    // **************************************************************************
    // **************************************************************************
    Space::Space()
    {
        this->World = new ong::World();
        this->InputManager = new sba::CInputManager();
        this->BrickManager = new sba::CBrickManager("../data/bricks/");
        this->m_pMap = new sba::CGameMap();

        std::string userpath = std::getenv("USERPROFILE");
        userpath += "\\Documents\\SpacebrickArena\\";
        std::string shipPath = userpath + "ships/";
        //Create Directory if it doesn't exist
#ifdef _WIN32
        CreateDirectory(userpath.c_str(), NULL);
        CreateDirectory(shipPath.c_str(), NULL);
#else
        mkdir(userpath, 777);
        mkdir(shipPath, 777);
#endif
        this->m_pIniReader = new sba::CIniReader((userpath + "Options.ini").c_str()); //Create IniFile
        this->ShipManager = new sba::CShipManager(shipPath.c_str()); 
        this->m_pNetworkhandler = new sba::CNetworkHandler(this->m_pIniReader);
		this->DestructionManager = new sba::CDestructionManager();

        this->m_pSoundPlayer = new PuRe_SoundPlayer();
    }

    // **************************************************************************
    // **************************************************************************
    Space::~Space()
    {
        for (unsigned int i = 0; i < this->m_Players.size(); i++)
        {
            SAFE_DELETE(this->m_Players[i]->Ship);
            SAFE_DELETE(this->m_Players[i]);
        }
        this->m_Players.clear();
        SAFE_DELETE(this->m_pSoundPlayer);
        SAFE_DELETE(this->m_pIniReader);
        SAFE_DELETE(this->m_pMap);
        SAFE_DELETE(this->Renderer);
        SAFE_DELETE(this->m_SSAOMaterial);
        SAFE_DELETE(this->ShipManager);
        SAFE_DELETE(this->BrickManager);
        SAFE_DELETE(this->InputManager);
        SAFE_DELETE(this->World);
        SAFE_DELETE(this->m_pNetworkhandler);
        SAFE_DELETE(this->m_pFinalMaterial);
    }

    // **************************************************************************
    // **************************************************************************
    void Space::Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer, PuRe_Application& a_rpApplication)
    {
        this->Application = &a_rpApplication;
        this->Renderer = new PuRe_Renderer(&a_pGraphics);
        this->m_SSAOMaterial = a_pGraphics.LoadMaterial("../data/effects/SSAO/default");
        this->m_pNoiseTexture = new PuRe_Sprite(&a_pGraphics, "../data/textures/ssao.jpg");
        for (int i = 0; i < 3; i++)
        {
            this->Renderer->AddTarget(PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth, a_pGraphics.GetDescription().ResolutionHeight));
        }
        if (this->m_pIniReader->GetValue("SSAO") == "On")
        {
            this->Renderer->SetSSAO(0, this->m_SSAOMaterial, this->m_pNoiseTexture);
            this->Renderer->SetSSAO(1, this->m_SSAOMaterial, this->m_pNoiseTexture);
        }
        this->BrickManager->Initialize();
        this->InputManager->Initialize();
        this->Font = new PuRe_Font(&a_pGraphics, "../data/textures/font.png");
        this->FontMaterial = a_pGraphics.LoadMaterial("../data/effects/font/default");
        this->SpriteMaterial = a_pGraphics.LoadMaterial("../data/effects/sprite/default");
        this->m_pFinalMaterial = a_pGraphics.LoadMaterial("../data/effects/Final/default");

        //load sounds
        int i = 0;
        PuRe_IWindow* window = sba_Application->GetWindow();
        std::string folderPath = "../data/audio/";
        std::string file = window->GetFileAtIndex(i, folderPath.c_str());
        while (file != "")
        {
            i++;
            if (file.substr(file.find_last_of(".") + 1) != "mp3")
            {
                file = window->GetFileAtIndex(i, folderPath.c_str());
                continue;
            }
            std::string name = file.substr(0, file.find_last_of("."));
            std::string path = folderPath + "/" + file.c_str();
            this->m_pSoundPlayer->LoadSound(path.c_str(), name.c_str());
            file = window->GetFileAtIndex(i, folderPath.c_str());

        }
    }

    // **************************************************************************
    // **************************************************************************
    void Space::UpdatePhysics(PuRe_Timer* a_pTimer)
    {
        if (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate)
        {
			ong_START_PROFILE(STEP);
            do
            {
                this->m_LastPhysicsUpdate += 1 / this->m_PhysicsFramerate;
                Space::Instance()->World->step(1 / this->m_PhysicsFramerate);
            } while (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate);
			ong_END_PROFILE(STEP);

			ong_START_PROFILE(REBUILD_RENDER_INSTANCE);
            Space::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances
			ong_END_PROFILE(REBUILD_RENDER_INSTANCE);

			//update destruction
			ong_START_PROFILE(DESTRUCTION);
			DestructionManager->Update();
			ong_END_PROFILE(DESTRUCTION);
        }

	}

    // **************************************************************************
    // **************************************************************************
    void Space::DeletePlayer(unsigned int a_Index)
    {
        if (this->m_Players.size() > a_Index)
        {
            SAFE_DELETE(this->m_Players[a_Index]->Ship);
            SAFE_DELETE(this->m_Players[a_Index]);
            this->m_Players.erase(this->m_Players.begin() + a_Index);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void Space::CreatePlayer(int a_Pad, PuRe_IWindow* a_pWindow)
    {
        sba::SPlayer* p = new sba::SPlayer();
        int ID = 0; // 0 is Host
        for (unsigned int j = 0; j < sba_Players.size(); j++)
        {
            if (ID == sba_Players[j]->ID)
            {
                ID++;
                j = 0;
            }
        }
        p->ID = ID;
        p->PadID = a_Pad;
        p->ShipID = 0;
        p->NetworkInformation = 0;
        std::vector<TheBrick::CBrickInstance**> engines;
        std::vector<TheBrick::CBrickInstance**> cockpits;
        std::vector<TheBrick::CBrickInstance**> weapons;
        for (unsigned int i = 0; i<sba_ShipManager->GetShipCount(); i++)
        {
            p->ShipID = i;
            p->Ship = sba_ShipManager->GetShip(p->ShipID);
            engines.clear();
            cockpits.clear();
            weapons.clear();
            p->Ship->GetEngines(engines);
            p->Ship->GetCockpits(cockpits);
            p->Ship->GetWeapons(weapons);
            if (engines.size() == 0||cockpits.size() == 0||weapons.size() == 0)
            {
                SAFE_DELETE(p->Ship);
                p->Ship = NULL;
            }
            else
                break;
        }
        if (p->Ship == NULL)
            printf("NO VALID SHIP!!\n");
        sba_Players.push_back(p);
    }

    // **************************************************************************
    // **************************************************************************
    void Space::RenderFont(std::string a_Text, PuRe_Vector2F a_Position, float a_Size, float a_Width, unsigned int a_RendertargetIndex)
    {
        this->Renderer->Draw(a_RendertargetIndex, false, this->Font, this->FontMaterial, a_Text, PuRe_Vector3F(a_Position, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F(a_Size, a_Size, a_Size), a_Size * a_Width);
    }

	void Space::AddMiscObject(TheBrick::CGameObject* a_pObject)
	{
		m_MiscObjects.push_back(a_pObject);
	}

	void Space::ClearMiscObjects()
	{
		for (auto pObject : m_MiscObjects)
		{
			delete pObject;
		}
		m_MiscObjects.clear();
	}
		
		

}