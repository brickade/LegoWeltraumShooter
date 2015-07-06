#ifndef _PLAYER_H_
#define _PLAYER_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Spaceship.h"

namespace sba
{
    struct SPlayer
    {
        unsigned char ID;
        char PadID;
        unsigned int ShipID;
        SOCKET NetworkInformation;
        bool Timeout;
        int m_Points;
        float m_ShootCooldown;
        CSpaceship* Ship;
    };
}
#endif /* _PLAYER_H_ */