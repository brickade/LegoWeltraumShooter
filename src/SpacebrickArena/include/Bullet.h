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
        TheBrick::CBrickInstance* m_pWeapon;
        float m_lifeTime;
        int m_Damage;
        bool m_Collided;
        float m_Speed;
        SPlayer* m_pOwner;
        unsigned int m_ID;
        PuRe_ParticleEmitter* m_pEmitter;
        PuRe_Color m_Color;
        int m_SoundChannel;

    public:
        CBullet(ong::BodyDescription* a_desc, ong::World& a_rWorld, SPlayer* a_pOwner, PuRe_Color a_Color, unsigned int a_ID,TheBrick::CBrickInstance* a_pWeapon);
        ~CBullet();
        void Update(float a_DeltaTime, std::vector<CBullet*>& a_rBullets);
        static void Collision(ong::Collider* thisCollider, ong::Contact* contact);
        void DrawEmitter(PuRe_Sprite* a_pSprite, PuRe_IMaterial* a_pMaterial, PuRe_PointLight* a_pLight, PuRe_IMaterial* a_pLightMaterial);
    };
}

#endif /* _BULLET_H_ */