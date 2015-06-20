#include "include/Space.h"
#include "include/INIReader.h"

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
        this->ShipManager = new sba::CShipManager("../data/ships/");
        this->m_pNetworkhandler = new sba::CNetworkHandler();
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
        SAFE_DELETE(this->Renderer);
        SAFE_DELETE(this->m_SSAOMaterial);
        SAFE_DELETE(this->ShipManager);
        SAFE_DELETE(this->BrickManager);
        SAFE_DELETE(this->InputManager);
        SAFE_DELETE(this->World);
        SAFE_DELETE(this->m_pNetworkhandler);
    }

    // **************************************************************************
    // **************************************************************************
    void Space::Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer, PuRe_Application& a_rpApplication)
    {
        this->Renderer = new PuRe_Renderer(&a_pGraphics);
        this->m_SSAOMaterial = a_pGraphics.LoadMaterial("../data/effects/SSAO/default");
        this->m_pNoiseTexture = new PuRe_Sprite(&a_pGraphics, "../data/textures/ssao.jpg");
        for (int i = 0; i < 3; i++)
        {
            this->Renderer->AddTarget(PuRe_Vector2I(a_pGraphics.GetDescription().ResolutionWidth, a_pGraphics.GetDescription().ResolutionHeight));
        }
        if (CIniReader::Instance()->GetValue("SSAO") == "On")
        {
            this->Renderer->SetSSAO(0, this->m_SSAOMaterial, this->m_pNoiseTexture);
            this->Renderer->SetSSAO(1, this->m_SSAOMaterial, this->m_pNoiseTexture);
        }
        this->BrickManager->Initialize();
        this->InputManager->Initialize();
        this->Font = new PuRe_Font(&a_pGraphics, "../data/textures/font.png");
        this->FontMaterial = a_pGraphics.LoadMaterial("../data/effects/font/default");

        this->Application = &a_rpApplication;
    }

    // **************************************************************************
    // **************************************************************************
    void Space::UpdatePhysics(PuRe_Timer* a_pTimer)
    {
        if (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate)
        {
            do
            {
                this->m_LastPhysicsUpdate += 1 / this->m_PhysicsFramerate;
                Space::Instance()->World->step(1 / this->m_PhysicsFramerate);
            } while (a_pTimer->GetTotalElapsedSeconds() - this->m_LastPhysicsUpdate >= 1 / this->m_PhysicsFramerate);
            Space::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances
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
            this->m_Players.erase(this->m_Players.begin()+a_Index);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool Space::CheckShip(PuRe_IWindow* a_pWindow)
    {
        int i = 0;
        const char* path = "../data/ships/";
        bool right = false;

        std::string file = a_pWindow->GetFileAtIndex(i, path);
        std::string lastFile = file;
        
        while (!right)
        {
            if (file.substr(file.find_last_of(".") + 1) == "ship")
                return true;
            else
            {
                i++;
                file = a_pWindow->GetFileAtIndex(i, path);
                if (lastFile == file)
                    return false;
                else
                    lastFile = file;
            }
        }
        return true;
    }

    // **************************************************************************
    // **************************************************************************
    void Space::CreatePlayer(int a_Pad,PuRe_IWindow* a_pWindow)
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
        p->NetworkInformation = 0;
        int i = 0;
        const char* path = "../data/ships/";
        bool right = false;

        std::string file = a_pWindow->GetFileAtIndex(i, path);
        std::string lastFile = file;

        while (!right)
        {
            if (file.substr(file.find_last_of(".") + 1) == "ship")
                right = true;
            else
            {
                i++;
                file = a_pWindow->GetFileAtIndex(i, path);
                if (lastFile == file)
                    right = true;
                else
                    lastFile = file;
            }
        }

        std::string name = file.substr(0, file.find_last_of("."));

        file = path + file;
        TheBrick::CSerializer serializer;
        serializer.OpenRead(file.c_str());
        ong::vec3 pos = ong::vec3(0.0f, 0.0f, 0.0f);
        p->Ship = new TheBrick::CSpaceship(*sba_World, name);
        p->Ship->Deserialize(serializer, *sba_BrickManager, *sba_World);
        serializer.Close();
        sba_Players.push_back(p);
    }

    // **************************************************************************
    // **************************************************************************
    void Space::RenderFont(std::string a_Text, PuRe_Vector2F a_Position, float a_Size, float a_Width, unsigned int a_RendertargetIndex)
    {
        this->Renderer->Draw(a_RendertargetIndex, false, this->Font, this->FontMaterial, a_Text, PuRe_Vector3F(a_Position, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F(a_Size, a_Size, a_Size), a_Size * a_Width);
    }
}