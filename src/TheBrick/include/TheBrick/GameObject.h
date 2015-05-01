#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Body.h>
#include <Onager/myMath.h>

#include "BrickInstance.h"
#include "Serializer.h"
#include "BrickManager.h"

namespace TheBrick
{
    class CGameObject
    {
    public:
        PuRe_List<CBrickInstance*> m_pBricks;
        ong::Transform m_Transform;
        ong::Body* m_pBody;

    private:

    public:
        CGameObject();
        virtual ~CGameObject();

        virtual void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

        static CGameObject* Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager);
        void Serialize(CSerializer* a_pSerializer);
    };
}

#endif /* _GAMEOBJECT_H_ */