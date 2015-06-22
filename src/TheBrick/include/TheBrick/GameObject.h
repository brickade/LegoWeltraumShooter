#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Body.h>
#include <Onager/myMath.h>
#include "BrickData.h"

namespace TheBrick
{
    class CBrickInstance;
    class CSerializer;
    class CBrickManager;
    enum EGameObjectType {Ship, Bullet, Object};
    class CGameObject
    {
    public:
        PuRe_List<CBrickInstance*> m_pBricks;
        ong::Body* m_pBody;
        EGameObjectType m_Type;

    private:

    public:
        const ong::Transform GetTransform() const
        {
            return this->m_pBody->getTransform();
        }

    public:
        CGameObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc);
        virtual ~CGameObject();

        virtual void Update(float a_DeltaTime);
        virtual void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

        virtual void Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld);
        virtual void Serialize(CSerializer& a_pSerializer);

        SBrickData GetBrick(unsigned int a_Index);
        void AddBrick(SBrickData a_Brick, CBrickManager& a_pBrickManager, ong::World& a_pWorld);

        void AddBrickInstance(CBrickInstance* a_pBrickInstance, ong::World& a_rWorld);
        void RemoveBrickInstance(const CBrickInstance& a_BrickInstance);

        PuRe_Vector3F PosToObjectSpace(const PuRe_Vector3F& a_rWorldSpacePosition) const;
        PuRe_Vector3F DirToObjectSpace(const PuRe_Vector3F& a_rWorldSpaceDir) const;
        PuRe_Vector3F PosToWorldSpace(const PuRe_Vector3F& a_rObjectSpacePosition) const;
        PuRe_Vector3F DirToWorldSpace(const PuRe_Vector3F& a_rObjectSpaceDir) const;
    };
}

#endif /* _GAMEOBJECT_H_ */