#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Body.h>
#include <Onager/myMath.h>

#include "BrickInstance.h"
#include "Serializer.h"
#include "BrickManager.h"

#include "DebugDraw.h"

namespace TheBrick
{
    class CGameObject
    {
    public:
        PuRe_List<CBrickInstance*> m_pBricks;
        ong::Body* m_pBody;

    private:

    public:
        CGameObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc);
        virtual ~CGameObject();

        virtual void Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld);
        void Serialize(CSerializer& a_pSerializer);

        void AddBrickInstance(CBrickInstance* a_pBrickInstance, ong::World& a_rWorld);
        void RemoveBrickInstance(const CBrickInstance& a_BrickInstance);
    };
}

#endif /* _GAMEOBJECT_H_ */