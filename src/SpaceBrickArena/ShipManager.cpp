#include "include/ShipManager.h"

#include "TheBrick/Serializer.h"
#include "TheBrick/Brick.h"

#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CShipManager::CShipManager(const char* a_pFolder)
    {
        this->m_FolderPath = a_pFolder;
    }

    // **************************************************************************
    // **************************************************************************
    CShipManager::~CShipManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::Load()
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        int i = 0;
        PuRe_IWindow* window = sba_Application->GetWindow();
        std::string file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        while (file != "")
        {
            if (file.substr(file.size() - 4) != ".dds")
            {
                continue;
            }
            std::pair<std::string, PuRe_Sprite> pair;
            pair.first = std::string(file).insert(0, this->m_FolderPath); //Set Path
            pair.second = PuRe_Sprite(sba_Application->GetGraphics(), pair.first); //Load Texture
            //Get Ship
            /*TheBrick::CSpaceship* ship = new TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
            serializer->OpenRead(pair.first.c_str());
            ship->Deserialize(*serializer, *sba_BrickManager, *sba_World);
            serializer->Close();*/
            //Set Name
            //ship->SetNameFromFilename(file);
            //Register Ship
            //this->AddShip(ship);
            //Next
            file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        }
        delete serializer;
        if (this->m_Sprites.size() == 0)
        {
            this->AddNewShip("Banana.ship");
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddShip(TheBrick::CSpaceship& a_rShip)
    {
        this->m_Sprites.push_back(std::make_pair(std::string(a_rShip.GetName()).insert(0, this->m_FolderPath), this->GetSpriteFromShip(a_rShip)));
        
        //Serialize ship
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddNewShip(const char* a_pName)
    {
        TheBrick::CSpaceship ship = TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
        ship.SetNameFromFilename(a_pName);
        //Register
        this->AddShip(ship);
        //Set to default
        this->ResetShip(ship);
        //Save to file
        this->SaveShipToFile(this->m_Spaceships.size() - 1);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::ResetShip(TheBrick::CSpaceship& a_rShip)
    {
        
        &a_rShip = new TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));

        a_rShip.SetNameFromFilename("Banana.ship");
        TheBrick::CBrickInstance* brickInstance = sba_BrickManager->GetBrick(1).CreateInstance(a_rShip, *sba_World);
        brickInstance->SetTransform(ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
        brickInstance->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, 0.0f, 0.0f));
        brickInstance->m_Color = PuRe_Color(0, 0, 1);
        this->UpdateSprite(a_rShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::SaveShipToFile(TheBrick::CSpaceship& a_rShip)
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if (serializer->OpenWrite(this->PathFromShip(this->m_Spaceships[a_Index])))
        {
            this->m_Spaceships[a_Index]->Serialize(*serializer);
        }
        serializer->Close();
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::DeleteShip(TheBrick::CSpaceship& a_rShip)
    {
        delete this->m_Spaceships[a_Index];
        this->m_Spaceships.erase(this->m_Spaceships.begin() + a_Index);
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CSpaceship* CShipManager::GetShip(size_t a_Index)
    {
        
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::UpdateSprite(TheBrick::CSpaceship& a_rShip)
    {
        std::string path = std::string(a_rShip.GetName()).insert(0, this->m_FolderPath);
        for (std::vector<std::pair<std::string, PuRe_Sprite>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
        {
            if ((*it).first.compare(path) == 0)
            {
                (*it).second = this->GetSpriteFromShip(a_rShip);
                return;
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Sprite CShipManager::GetSpriteFromShip(TheBrick::CSpaceship& a_rShip) const
    {
        PuRe_Sprite sprite = PuRe_Sprite(sba_Application->GetGraphics(), "");

        return sprite;
    }
}