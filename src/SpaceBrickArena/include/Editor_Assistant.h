#ifndef _EDITORASSISTANT_H_
#define _EDITORASSISTANT_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

namespace TheBrick{
    class CGameObject;
    class CBrickInstance;
}

namespace Editor
{
    class CAssistant
    {

    public:

    private:

    public:
        static bool GetClosestHitFromBrickInstanceNubs(TheBrick::CBrickInstance& a_rBrickInstanceToCastFrom, TheBrick::CGameObject& a_rGameObjectToCastTo, bool a_NubToCastFromGenderIsMale, const ong::vec3& a_rNubToCastFromDirection, ong::RayQueryResult* a_pResult, ong::vec3* a_pResultRayOrigin);
        static bool CanDockAtHit(ong::RayQueryResult& a_rHit, bool a_NubRequestedGenderIsMale, const ong::vec3& a_rNubRequestedDirection);
        static bool MovementDeltaIsCollisionFree(TheBrick::CBrickInstance& a_rBrickInstanceToTest, TheBrick::CGameObject& a_rGameObjectToTestAgainst, const ong::vec3& a_rBrickInstanceMoveDeltaToTest, float a_MoveTestingSteps, ong::vec3* a_pMaxCollisionFreeMovementDelta);
    };
}

#endif /* _EDITORASSISTANT_H_ */