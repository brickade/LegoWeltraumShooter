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
        int Points;
        float ShootCooldown;
        float Shake;
        float Marker;
        CSpaceship* Ship;

        void Update(float a_DeltaTime)
        {
            if (this->Shake != 0.0f)
            {
                this->Shake -= a_DeltaTime;
                if (this->Shake < 0.0f)
                    this->Shake = 0.0f;
            }
            if (this->Marker != 0.0f)
            {
                this->Marker -= a_DeltaTime;
                if (this->Marker < 0.0f)
                    this->Marker = 0.0f;
            }
        }
    };
}
#endif /* _PLAYER_H_ */