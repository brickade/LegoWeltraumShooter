#include "include/Editor_Assistant.h"
#include <TheBrick/Nub.h>
#include <TheBrick/Conversion.h>
#include <TheBrick/BrickInstance.h>
#include <TheBrick/Brick.h>
#include <TheBrick/GameObject.h>

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    /*CAssistant::CAssistant()
    {

    }*/

    // **************************************************************************
    // **************************************************************************
    bool CAssistant::GetClosestHitFromBrickInstanceNubs(TheBrick::CBrickInstance& a_rBrickInstanceToCastFrom, TheBrick::CGameObject& a_rGameObjectToCastTo, bool a_NubToCastFromGenderIsMale, const ong::vec3& a_rNubToCastFromDirection, ong::RayQueryResult* a_pResult)
    {
        std::vector<TheBrick::SNub>& nubsToCastFrom = a_rBrickInstanceToCastFrom.m_pBrick->GetNubs(); //Get currentBrick Nubs
        ong::Body& bodyToCastTo = *a_rGameObjectToCastTo.m_pBody;
        a_pResult->t = FLT_MAX;
        bool hit = false;
        for (size_t i = 0; i < nubsToCastFrom.size(); i++) //Go through currentBrick Nubs
        {
            const ong::vec3 rayDir = TheBrick::PuReToOng(a_rBrickInstanceToCastFrom.DirToWorldSpace(nubsToCastFrom[i].Direction)); //Transform nubDirection to WorldSpace
            if (nubsToCastFrom[i].isMale != a_NubToCastFromGenderIsMale || dot(ong::normalize(rayDir), a_rNubToCastFromDirection) < 0.99f)
            { //Wrong Direction
                continue;
            }
            const ong::vec3 rayOrigin = TheBrick::PuReToOng(a_rBrickInstanceToCastFrom.PosToWorldSpace(nubsToCastFrom[i].Position)); //Transform nubPosition to WorldSpace

            ong::RayQueryResult hs = { 0 }; //Allocate
            hs.t = FLT_MAX;
            //--------------------------------------------------------------------------------
            if (bodyToCastTo.queryRay(rayOrigin, rayDir, &hs)) //Cast Ray
            { //Ray Hit
                hit = true;
                //printf("RayPointHeight %i: %i\n", i, (int)(hs.point.y / TheBrick::CBrick::SEGMENT_HEIGHT));
                if (hs.t < a_pResult->t)
                { //This Hit is nearer to the brick than the saved one
                    *a_pResult = hs;
                }
            }
        }
        return hit;
    }

    // **************************************************************************
    // **************************************************************************
    bool CAssistant::CanDockAtHit(ong::RayQueryResult& a_rHit, bool a_NubRequestedGenderIsMale, const ong::vec3& a_rNubRequestedDirection)
    {
        TheBrick::CBrickInstance* brickInstance = reinterpret_cast<TheBrick::CBrickInstance*>(a_rHit.collider->getUserData()); //Get hit BrickInstance
        //Transform hitResult direction to brickInstance BrickSpace
        ong::vec3 dockingdir = TheBrick::PuReToOng(brickInstance->DirToBrickSpace(TheBrick::OngToPuRe(a_rHit.normal)));
        //Get hitBrick Nub at that position
        TheBrick::SNub* nub = brickInstance->GetNubAtWorldPos(TheBrick::OngToPuRe(a_rHit.point), 0.01f);
        assert(nub != nullptr);
        if (nub->isMale != a_NubRequestedGenderIsMale || dot(ong::normalize(TheBrick::PuReToOng(nub->Direction)), a_rNubRequestedDirection) < 0.99f)
        { //Wrong Direction
            return false;
        }
        return true;
    }

    // **************************************************************************
    // **************************************************************************
    static ong::vec3 GetMaxCollisionFreeMovementDelta(TheBrick::CBrickInstance& a_rBrickInstanceToTest, TheBrick::CGameObject& a_rGameObjectToTestAgainst, const ong::vec3& a_rBrickInstanceMoveDirection, float a_MoveTestingSteps)
    {
        
    }
}