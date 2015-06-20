#ifndef _EDITORSHIPHANDLER_H_
#define _EDITORSHIPHANDLER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "TheBrick/Spaceship.h"
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
        TheBrick::CSpaceship* m_pCurrentSpaceship;

    public:
        TheBrick::CSpaceship* GetCurrentSpaceShip()
        {
            return this->m_pCurrentSpaceship;
        }

    public:
        CShipHandler(int a_playerIdx);
        ~CShipHandler();

        void AddShip(const char* a_pName);
        void SaveCurrentShip();
        void ResetCurrentShip();
        void DeleteCurrentShip();

        TheBrick::CBrickInstance* AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate);

        void UpdateCurrentShipPreview();
    };
}

#endif /* _EDITORSHIPHANDLER_H_ */