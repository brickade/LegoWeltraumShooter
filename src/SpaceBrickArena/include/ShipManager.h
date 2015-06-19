#ifndef _SHIPMANAGER_H_
#define _SHIPMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <TheBrick/Spaceship.h>

namespace sba
{
    class CShipManager
    {
    private:
        std::vector<TheBrick::CSpaceship*> m_Spaceships;
        std::string m_FolderPath = "../data/ships/";
        int m_LastPlayedId = -1;
        int m_LastEditedId = -1;

    public:
        TheBrick::CSpaceship* operator[](std::size_t a_Index)
        {
            return this->m_Spaceships[a_Index];
        };

        const TheBrick::CSpaceship* operator[](std::size_t a_Index) const
        {
            return this->m_Spaceships[a_Index];
        };

    public:
        CShipManager();
        ~CShipManager();

        void Load();
        void AddShip(TheBrick::CSpaceship* a_pShip);
        void AddNewShip(const char* a_pName);
        void SaveShip(int a_Index);
        void ResetShip(int a_Index);
        void DeleteShip(int a_Index);

    private:
        const char* PathFromShip(TheBrick::CSpaceship* a_pShip) const
        {
            std::string* tmp = new std::string(a_pShip->GetName());
            tmp->insert(0, this->m_FolderPath);
            return tmp->c_str();
        }

        const char* PathFromName(const char* a_pName) const
        {
            std::string* tmp = new std::string(a_pName);
            tmp->insert(0, this->m_FolderPath);
            return tmp->c_str();
        }
    };
}

#endif /* _SHIPMANAGER_H_ */