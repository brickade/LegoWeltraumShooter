#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "GameObject.h"

namespace TheBrick
{
    class CSpaceship : public CGameObject
    {
    public:

    private:

    public:
        CSpaceship(ong::World& a_rWorld);
        ~CSpaceship();

        void Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld) override;
        void Serialize(CSerializer& a_pSerializer);
    };
}

#endif /* _SPACESHIP_H_ */