#ifndef _BRICKINSTANCE_H_
#define _BRICKINSTANCE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>
#include <Onager/myMath.h>
#include <Onager/World.h>

#include "Conversion.h"
#include "Brick.h"
#include "Nub.h"
#include "Conversion.h"


namespace TheBrick
{
    //Forward declaration
    class CBrick;

    class CBrickInstance
    {
    public:
        CBrick* m_pBrick;
        ong::Transform m_Transform;
        PuRe_List<ong::Collider*> m_pCollider;
        PuRe_Color m_Color;

    private:

    public:
        CBrickInstance(CBrick* a_pBrick, ong::World* a_pWorld, PuRe_Color a_pColor = PuRe_Color(0.7f, 0.7f, 0.7f, 1.0f));
        ~CBrickInstance();

        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

        PuRe_List<SNub>* GetWorldSpaceNubs();
    };
}

#endif /* _BRICKINSTANCE_H_ */