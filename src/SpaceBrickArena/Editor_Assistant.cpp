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
    bool CAssistant::GetClosestHitsFromBrickInstanceNubs(TheBrick::CBrickInstance& a_rBrickInstanceToCastFrom, TheBrick::CGameObject& a_rGameObjectToCastTo, bool a_NubToCastFromGenderIsMale, const ong::vec3& a_rNubToCastFromDirection, std::vector<ong::RayQueryResult>* a_pResults, std::vector<ong::vec3>* a_pResultRayOrigins)
    {
        std::vector<TheBrick::SNub>& nubsToCastFrom = a_rBrickInstanceToCastFrom.m_pBrick->GetNubs(); //Get currentBrick Nubs
        ong::Body& bodyToCastTo = *a_rGameObjectToCastTo.m_pBody;
        float currentMinDistance = FLT_MAX;
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
                if (fabs(hs.t - currentMinDistance) < FLT_EPSILON)
                { //This Hit is as near to the brick as the others
                    a_pResults->push_back(hs);
                    a_pResultRayOrigins->push_back(rayOrigin);
                    continue;
                }
                if (hs.t < currentMinDistance)
                { //This Hit is nearer to the brick than the others
                    a_pResults->clear();
                    a_pResultRayOrigins->clear();
                    currentMinDistance = hs.t;
                    a_pResults->push_back(hs);
                    a_pResultRayOrigins->push_back(rayOrigin);
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
        if (nub == nullptr || nub->isMale != a_NubRequestedGenderIsMale || dot(ong::normalize(TheBrick::PuReToOng(nub->Direction)), a_rNubRequestedDirection) < 0.99f)
        { //No nub or wrong Direction
            return false;
        }
        return true;
    }

    // **************************************************************************
    // **************************************************************************
    bool CAssistant::MovementDeltaIsCollisionFree(TheBrick::CBrickInstance& a_rBrickInstanceToTest, TheBrick::CGameObject& a_rGameObjectToTestAgainst, const ong::vec3& a_rBrickInstanceMoveDeltaToTest, float a_MoveTestingSteps, ong::vec3* a_pMaxCollisionFreeMovementDelta)
    {
        ong::vec3 start = TheBrick::PuReToOng(a_rBrickInstanceToTest.GetGameObject()->PosToWorldSpace(TheBrick::OngToPuRe(a_rBrickInstanceToTest.GetTransform().p)));
        ong::vec3 direction = ong::normalize(a_rBrickInstanceMoveDeltaToTest);
        int maxSteps = (int)round((ong::length(a_rBrickInstanceMoveDeltaToTest) / a_MoveTestingSteps));
        for (int i = 0; i <= maxSteps; i++)
        {
            *a_pMaxCollisionFreeMovementDelta = (i * a_MoveTestingSteps) * direction;
            ong::Transform transform = a_rBrickInstanceToTest.GetTransform();
            transform.p = TheBrick::PuReToOng(a_rBrickInstanceToTest.GetGameObject()->PosToObjectSpace(TheBrick::OngToPuRe(start + *a_pMaxCollisionFreeMovementDelta)));
            a_rBrickInstanceToTest.SetTransform(transform);
            bool quit = false;
            for (size_t t = 0; t < a_rBrickInstanceToTest.m_pCollider.size(); t++)
            {
                if (a_rGameObjectToTestAgainst.m_pBody->queryCollider(a_rBrickInstanceToTest.m_pCollider[t]))
                {
                    return false;
                }
            }
        }
        return true;
    }
}