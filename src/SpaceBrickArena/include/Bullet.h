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
        PuRe_ParticleEmitter* m_pEmitter;

    public:
        CBullet(ong::BodyDescription* a_desc, ong::World& a_rWorld, SPlayer* a_pOwner, PuRe_Color a_Color);
        ~CBullet();
        void Update(float a_DeltaTime) override;
        static void Collision(ong::Collider* thisCollider, ong::Contact* contact);
        void DrawEmitter(PuRe_Sprite* a_pSprite, PuRe_IMaterial* a_pMaterial, PuRe_PointLight* a_pLight, PuRe_IMaterial* a_pLightMaterial);
    };
}

#endif /* _BULLET_H_ */