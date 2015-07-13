#ifndef _ASTEROID_H_
#define _ASTEROID_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "TheBrick/GameObject.h"
#include "DestructibleObject.h"

#include "TheBrick/DebugDraw.h"
#include "TheBrick/CSVParser.h"

namespace sba
{
	class CAsteroid : public CDestructibleObject
    {
    public:
        CAsteroid(ong::World& a_rWorld, ong::vec3 a_Position, ong::vec3 a_Velocity, ong::vec3 a_Rotation);
        ~CAsteroid();

        void Update(float a_DeltaTime) override;
        void Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld) override;
        void Serialize(TheBrick::CSerializer& a_pSerializer) override;
    };
}

#endif /* _ASTEROID_H_ */