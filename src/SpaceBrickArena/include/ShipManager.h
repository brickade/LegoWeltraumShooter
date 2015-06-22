#ifndef _SHIPMANAGER_H_
#define _SHIPMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "TheBrick/Spaceship.h"

namespace sba
{
    class CShipManager
    {
    private:
        std::vector<std::pair<std::string, PuRe_Sprite*>> m_Sprites;
        std::string m_FolderPath = "";

    public:
        PuRe_Sprite* operator[](size_t a_Index)
        {
            return this->m_Sprites[a_Index].second;
        };

        const PuRe_Sprite* operator[](size_t a_Index) const
        {
            return this->m_Sprites[a_Index].second;
        };

    public:
        CShipManager(const char* a_pFolder);
        ~CShipManager();

        void Load(); //Load all sprites and paths from folder
        void AddShip(TheBrick::CSpaceship& a_rShip); //Add ship sprite and path and save ship file
        void AddNewShip(const char* a_pName); //Add default ship with specified name
        void ResetShip(TheBrick::CSpaceship& a_rShip); //Reset given ship to default and update sprite
        void SaveShipToFile(TheBrick::CSpaceship& a_rShip); //Save given ship to file
        void DeleteShip(TheBrick::CSpaceship& a_rShip); //Delete given ship from disk and delete sprite and path
        TheBrick::CSpaceship* GetShip(size_t a_Index); //Load actual ship from disk
        void UpdateSprite(TheBrick::CSpaceship& a_rShip); //Updates the sprite, does not save it to file

    private:
        const char* PathFromShip(TheBrick::CSpaceship& a_rShip) const
        {
            std::string* tmp = new std::string(a_rShip.GetName());
            tmp->insert(0, this->m_FolderPath);
            return tmp->c_str();
        }

        const char* PathFromName(const char* a_pName) const
        {
            std::string* tmp = new std::string(a_pName);
            tmp->insert(0, this->m_FolderPath);
            return tmp->c_str();
        }

        PuRe_Sprite* GetSpriteFromShip(TheBrick::CSpaceship& a_rShip) const;
    };
}

#endif /* _SHIPMANAGER_H_ */