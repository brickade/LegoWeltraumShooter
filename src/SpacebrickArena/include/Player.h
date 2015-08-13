#ifndef _PLAYER_H_
#define _PLAYER_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Spaceship.h"
#include "TheBrick/Conversion.h"

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
        char Weapon;
        float MGCD;
        float RocketCD;
        float LaserCD;
        float MineCD;
        float Marker;
        float OutTime;
        float KilledTimer;
        CSpaceship* Ship;

        void Update(float a_DeltaTime,PuRe_Vector3F& a_rOrigin,float& a_rDistance)
        {
            if (this->KilledTimer != 0.0f)
            {
                this->KilledTimer -= a_DeltaTime;
                if (this->KilledTimer < 0.0f)
                    this->KilledTimer = 0.0f;
            }
            if (this->Ship->m_Shake != 0.0f)
            {
                this->Ship->m_Shake -= a_DeltaTime;
                if (this->Ship->m_Shake < 0.0f)
                    this->Ship->m_Shake = 0.0f;
            }
            if (this->Marker != 0.0f)
            {
                this->Marker -= a_DeltaTime;
                if (this->Marker < 0.0f)
                    this->Marker = 0.0f;
            }
            float dist = (TheBrick::OngToPuRe(this->Ship->m_pBody->getWorldCenter()) - a_rOrigin).Length();
            if (dist > a_rDistance)
            {
                OutTime += a_DeltaTime;
                if (OutTime > 10.0f)
                {
                    OutTime = 0.0f;
                    this->Ship->m_Life = 0;
                }
            }
            else if (OutTime != 0.0f)
                OutTime = 0.0f;
        }
    };
}
#endif /* _PLAYER_H_ */