#ifndef _MENU_SERVER_H_
#define _MENU_SERVER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include "Space.h"

namespace Menu
{
    struct SServer
    {
        std::string Name;
        std::string Port;
        std::string IP;
        unsigned int Players;
        float LastUpdate; 
    };

    inline bool operator==(const SServer& lhs, const SServer& rhs)
    {
        return rhs.Name == lhs.Name&&rhs.IP == lhs.IP&&rhs.Port == lhs.Port;
    }
}

#endif /* _MENU_SERVER_H_ */