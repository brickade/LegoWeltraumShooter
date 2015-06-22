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
        int i = 0;
        PuRe_IWindow* window = sba_Application->GetWindow();
        std::string file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        while (file != "")
        {
            i++;
            if (file.substr(file.find_last_of(".")+1) != "ship")
            {
                file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
                continue;
            }
            std::string namePath = file.substr(0, file.find_last_of(".")).insert(0, this->m_FolderPath);
            std::string previewPath = std::string(namePath).append(".dds");

            std::pair<std::string, PuRe_Sprite*> pair;
            pair.first = namePath; //Set Path

            //Check if preview exists
            std::ifstream preview(previewPath.c_str());
            if (!preview.good())
            { //Create new Sprite
                //Load Ship
                Game::CSpaceship ship(*sba_World, "");
                TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
                serializer->OpenRead(std::string(namePath).append(".ship").c_str());
                ship.Deserialize(*serializer, sba_BrickManager->GetBrickArray(), *sba_World);
                serializer->Close();
                delete serializer;
                //Create Sprite
                pair.second = this->GetSpriteFromShip(ship);
                //Save Sprite
                pair.second->GetTexture()->SaveTextureToFile(std::string(namePath).append(".dds").c_str());
            }
            else
            { //Load Sprite
                //Load Texture
                pair.second = new PuRe_Sprite(sba_Application->GetGraphics(), std::string(pair.first).append(".dds"));
            }
            this->m_Sprites.push_back(pair);
            file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        }
        if (this->m_Sprites.size() == 0)
        {
            this->AddNewShip("Banana");
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddShip(Game::CSpaceship& a_rShip)
    { //Add ship sprite and path and save ship file
        this->m_Sprites.push_back(std::make_pair(std::string(a_rShip.GetName()).insert(0, this->m_FolderPath), this->GetSpriteFromShip(a_rShip)));
        this->SaveShipToFile(a_rShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddNewShip(const char* a_pName)
    { //Add default ship with specified name
        Game::CSpaceship ship = Game::CSpaceship(*sba_World, a_pName);
        //Register
        this->m_Sprites.push_back(std::make_pair(std::string(ship.GetName()).insert(0, this->m_FolderPath), this->GetSpriteFromShip(ship)));
        //Set to default
        this->ResetShip(ship);
        //Save to file
        this->SaveShipToFile(ship);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::ResetShip(Game::CSpaceship& a_rShip)
    { //Reset given ship to default and update sprite
        a_rShip.Reset(sba_BrickManager->GetBrick(1), *sba_World);
        this->UpdateSprite(a_rShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::SaveShipToFile(Game::CSpaceship& a_rShip)
    { //Save given ship to file
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        std::string tmp = std::string(this->PathFromShip(a_rShip)).append(".ship");
        if (serializer->OpenWrite(std::string(this->PathFromShip(a_rShip)).append(".ship").c_str()))
        {
            a_rShip.Serialize(*serializer);
            serializer->Close();
        }
        delete serializer;
        std::string spritePath = this->PathFromShip(a_rShip);
        spritePath.append(".dds");
        this->GetSpriteFromShip(a_rShip)->GetTexture()->SaveTextureToFile(spritePath.c_str());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::DeleteShip(Game::CSpaceship& a_rShip)
    { //Delete given ship from disk and delete sprite and path
        if (remove(std::string(this->PathFromShip(a_rShip)).append(".ship").c_str()) != 0)
        {
            printf("cant delete ship bro");
        }
        std::string spritePath = this->PathFromShip(a_rShip);
        spritePath.append(".dds");
        if (remove(spritePath.c_str()) != 0)
        {
            printf("cant delete dds bro");
        }
        std::string path = this->PathFromShip(a_rShip);
        for (std::vector<std::pair<std::string, PuRe_Sprite*>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
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
    Game::CSpaceship* CShipManager::GetShip(size_t a_Index)
    { //Load actual ship from disk
        Game::CSpaceship* ship = new Game::CSpaceship(*sba_World, this->m_Sprites[a_Index].first.substr(this->m_Sprites[a_Index].first.find_last_of("/") + 1));
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if (serializer->OpenRead(std::string(this->m_Sprites[a_Index].first).append(".ship").c_str()))
        {
            ship->Deserialize(*serializer, sba_BrickManager->GetBrickArray(), *sba_World);
            serializer->Close();
        }
        delete serializer;
        return ship;
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::UpdateSprite(Game::CSpaceship& a_rShip)
    {
        std::string path = this->PathFromShip(a_rShip);
        for (std::vector<std::pair<std::string, PuRe_Sprite*>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
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
    PuRe_Sprite* CShipManager::GetSpriteFromShip(Game::CSpaceship& a_rShip) const
    {
        PuRe_Sprite* sprite = new PuRe_Sprite(sba_Application->GetGraphics(), "");
        //TODO: implement
        
        return sprite;
    }
}