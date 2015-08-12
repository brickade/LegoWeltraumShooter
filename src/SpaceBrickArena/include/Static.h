#ifndef _STATIC_H_
#define _STATIC_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "TheBrick/GameObject.h"

#include "TheBrick/DebugDraw.h"
#include "TheBrick/CSVParser.h"

namespace sba
{
    class CStatic : public TheBrick::CGameObject
    {
    public:
        CStatic(ong::World& a_rWorld, ong::BodyDescription* a_desc);
        ~CStatic();

        void Update(float a_DeltaTime) override;
        void Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld) override;
        void Serialize(TheBrick::CSerializer& a_pSerializer) override;
    };
}

#endif /* _STATIC_H_ */