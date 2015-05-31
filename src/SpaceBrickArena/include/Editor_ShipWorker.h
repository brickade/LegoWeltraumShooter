#ifndef _EDITORSHIPWORKER_H_
#define _EDITORSHIPWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "TheBrick/Spaceship.h"

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

        TheBrick::CSpaceship* m_pCurrentSpaceship;

    public:
        TheBrick::CSpaceship* GetCurrentSpaceShip()
        {
            return this->m_pCurrentSpaceship;
        }

    public:
        CShipWorker();
        ~CShipWorker();

        void LoadShipFromFile(const char* a_pFile);
        void SaveShipToFile(const char* a_pFile);

        void ResetShip();
        TheBrick::CBrickInstance* AddBrickInstanceToShip(const TheBrick::CBrickInstance& a_pTemplate);
    };
}

#endif /* _EDITORSHIPWORKER_H_ */