#ifndef _GAMEMAP_H_
#define _GAMEMAP_H_

#include <string>
#include "Asteroid.h"
#include "Item.h"

#include "TheBrick/Conversion.h"

namespace sba
{
    class CGameMap
    {
    private:
        std::string m_Name;
    public:
        void SetMap(const char* a_pFile);
    public:
        CGameMap();
        ~CGameMap();
    public:
        bool GetMapData(std::vector<CAsteroid*>& a_rObjects, std::vector<CItem*>& a_rItems);
        std::string GetSkybox();
        std::string GetName();
    };
}

#endif /* _GAMEMAP_H_ */