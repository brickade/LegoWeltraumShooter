#ifndef _EDITORSHIPHANDLER_H_
#define _EDITORSHIPHANDLER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "Spaceship.h"
#include "Navigation.h"

#include "include/Space.h"

namespace TheBrick
{
    class CSpaceship;
    class CBrickInstance;
}

namespace Editor
{
    class CShipHandler
    {

    public:

    private:
        int m_playerIdx;

        sba::CNavigation* m_pNavigation;
        Game::CSpaceship* m_pCurrentSpaceship;

    public:
        Game::CSpaceship* GetCurrentSpaceShip()
        {
            return this->m_pCurrentSpaceship;
        }

    public:
        CShipHandler(int a_playerIdx);
        ~CShipHandler();

        void Update();
        void Render();

        void AddShip(const char* a_pName);
        void SaveCurrentShip();
        void ResetCurrentShip();
        void DeleteCurrentShip();

        TheBrick::CBrickInstance* AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate);

        void UpdateCurrentShipPreview();
    };
}

#endif /* _EDITORSHIPHANDLER_H_ */