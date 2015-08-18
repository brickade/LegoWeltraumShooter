#ifndef _SHIPMANAGER_H_
#define _SHIPMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Spaceship.h"

namespace Editor
{
    class CEditorScene;
    class CCamera;
}

namespace sba
{
    class CShipManager
    {
    private:
        std::vector<std::pair<std::string, PuRe_Sprite*>> m_Sprites;
        std::string m_FolderPath = "";

        PuRe_IMaterial* m_pPostMaterial;
        PuRe_PointLight* m_pPointLight;
        PuRe_IMaterial* m_pPointLightMaterial;
        PuRe_DirectionalLight* m_pDirectionalLight;
        PuRe_IMaterial* m_pDirectionalLightMaterial;
        Editor::CCamera* m_pCamera;

    public:
        PuRe_Sprite* operator[](size_t a_Index)
        {
            return this->m_Sprites[a_Index].second;
        };

        const PuRe_Sprite* operator[](size_t a_Index) const
        {
            return this->m_Sprites[a_Index].second;
        };

        size_t GetShipCount() const
        {
            return this->m_Sprites.size();
        }

    public:
        CShipManager(const char* a_pFolder);
        ~CShipManager();

        void Load(); //Load all sprites and paths from folder
        void AddShip(sba::CSpaceship& a_rShip); //Add ship sprite and path and save ship file
        void AddNewShip(const char* a_pName); //Add default ship with specified name
        void ResetShip(sba::CSpaceship& a_rShip); //Reset given ship to default and update sprite
        void SaveShipToFile(sba::CSpaceship& a_rShip); //Save given ship to file
#ifdef EDITOR_DEV
        void SaveShipToFileAsObject(sba::CSpaceship& a_rShip); //Save given ship to file
#endif
        void DeleteShip(sba::CSpaceship& a_rShip); //Delete given ship from disk and delete sprite and path
        sba::CSpaceship* GetShip(size_t a_Index); //Load actual ship from disk
        void UpdateSprite(sba::CSpaceship& a_rShip); //Updates the sprite, does not save it to file

        void UpdateShipName(sba::CSpaceship& a_rShip, std::string& a_rOldShipName); //Update Ship and sprite name on disk and path in m_Sprites
        
        void BatchRenderShip(const sba::CSpaceship& a_rShip, int a_Width = 1920, int a_Height = 1080) const;
    private:
        const char* PathFromShip(sba::CSpaceship& a_rShip) const
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

        PuRe_Sprite* GetSpriteFromShip(const sba::CSpaceship& a_rShip) const;
    };
}

#endif /* _SHIPMANAGER_H_ */