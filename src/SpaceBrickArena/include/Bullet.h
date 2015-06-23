#ifndef _BULLET_H_
#define _BULLET_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Onager/World.h"

#include "TheBrick/GameObject.h"

namespace sba
{
    struct SPlayer;

    class CBullet : public TheBrick::CGameObject
    {
    public:
        float m_lifeTime;
        int m_Damage;
        bool m_Collided;
        SPlayer* m_pOwner;

    public:
        CBullet(PuRe_Vector3F a_Position, PuRe_Vector3F a_Speed, ong::World& a_rWorld,SPlayer* a_pOwner);
        ~CBullet();
        void Update(float a_DeltaTime) override;
    };
}

#endif /* _BULLET_H_ */