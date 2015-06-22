#ifndef _ASTEROID_H_
#define _ASTEROID_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "TheBrick/GameObject.h"

#include "TheBrick/DebugDraw.h"
#include "TheBrick/CSVParser.h"

namespace Game
{
    class CAsteroid : public TheBrick::CGameObject
    {
    public:
        CAsteroid(/*CBrickManager* a_pBrickManager, */ong::World& a_rWorld, ong::vec3 a_Position);
        ~CAsteroid();

        void Update(float a_DeltaTime) override;

        void Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld) override;
        void Serialize(TheBrick::CSerializer& a_pSerializer) override;
    };
}

#endif /* _ASTEROID_H_ */