#include "include/ShipManager.h"

#include "TheBrick/Serializer.h"
#include "TheBrick/Brick.h"
#include "include/BrickManager.h"

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
    { //Load all sprites and paths from folder
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
    { //Add ship sprite and path and save ship file
        this->m_Sprites.push_back(std::make_pair(std::string(a_rShip.GetName()).insert(0, this->m_FolderPath), this->GetSpriteFromShip(a_rShip)));
        this->SaveShipToFile(a_rShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddNewShip(const char* a_pName)
    { //Add default ship with specified name
        TheBrick::CSpaceship ship = TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
        ship.SetNameFromFilename(a_pName);
        //Register
        this->m_Sprites.push_back(std::make_pair(std::string(ship.GetName()).insert(0, this->m_FolderPath), this->GetSpriteFromShip(ship)));
        //Set to default
        this->ResetShip(ship);
        //Save to file
        this->SaveShipToFile(ship);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::ResetShip(TheBrick::CSpaceship& a_rShip)
    { //Reset given ship to default and update sprite
        a_rShip.Reset(sba_BrickManager->GetBrick(1), *sba_World);
        this->UpdateSprite(a_rShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::SaveShipToFile(TheBrick::CSpaceship& a_rShip)
    { //Save given ship to file
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if (serializer->OpenWrite(this->PathFromShip(a_rShip)))
        {
            a_rShip.Serialize(*serializer);
        }
        serializer->Close();
        delete serializer;
        std::string spritePath = this->PathFromShip(a_rShip);
        spritePath = spritePath.substr(0, spritePath.find_last_of(".")).append(".dds");
        this->GetSpriteFromShip(a_rShip).GetTexture()->SaveTextureToFile(spritePath.c_str());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::DeleteShip(TheBrick::CSpaceship& a_rShip)
    { //Delete given ship from disk and delete sprite and path
        if (remove(this->PathFromShip(a_rShip)) != 0)
        {
            printf("cant delete ship bro");
        }
        std::string spritePath = this->PathFromShip(a_rShip);
        spritePath = spritePath.substr(0, spritePath.find_last_of(".")).append(".dds");
        if (remove(spritePath.c_str()) != 0)
        {
            printf("cant delete dds bro");
        }
        std::string path = this->PathFromShip(a_rShip);
        for (std::vector<std::pair<std::string, PuRe_Sprite>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
        {
            if ((*it).first.compare(path) == 0)
            {
                this->m_Sprites.erase(it);
                break;
            }
        }

    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CSpaceship* CShipManager::GetShip(size_t a_Index)
    { //Load actual ship from disk
        TheBrick::CSpaceship ship = TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if (serializer->OpenRead(this->PathFromName(this->m_Sprites[a_Index].first.c_str())))
        {
            ship.Deserialize(*serializer, sba_BrickManager->GetBrickArray(), *sba_World);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::UpdateSprite(TheBrick::CSpaceship& a_rShip)
    {
        std::string path = this->PathFromShip(a_rShip);
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