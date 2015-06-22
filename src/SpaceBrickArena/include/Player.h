#ifndef _PLAYER_H_
#define _PLAYER_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "SpaceShip.h"

namespace sba
{
    struct SPlayer
    {
        unsigned char ID;
        char PadID;
        SOCKET NetworkInformation;
        bool Timeout;
        float m_Points;
        sba::CSpaceship* Ship;
    };
}
#endif /* _PLAYER_H_ */