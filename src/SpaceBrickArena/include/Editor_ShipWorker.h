#ifndef _EDITORSHIPWORKER_H_
#define _EDITORSHIPWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "TheBrick/Spaceship.h"
#include "Navigation.h"

#include "include\Space.h"

namespace TheBrick
{
    class CSpaceship;
    class CBrickInstance;
}

namespace Editor
{
    class CShipWorker
    {

    public:

    private:
        sba::CNavigation* m_pNavigation;

    public:
        TheBrick::CSpaceship* GetCurrentSpaceShip()
        {
            return (*sba_ShipManager)[this->m_pNavigation->GetFocusedElementId()];
        }

    public:
        CShipWorker();
        ~CShipWorker();

        void AddShip(const char* a_pName);
        void SaveCurrentShip();
        void ResetCurrentShip();
        void DeleteCurrentShip();

        TheBrick::CBrickInstance* AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate);
    };
}

#endif /* _EDITORSHIPWORKER_H_ */