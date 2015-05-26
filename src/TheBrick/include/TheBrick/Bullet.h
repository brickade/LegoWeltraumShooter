#ifndef _BULLET_H_
#define _BULLET_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "GameObject.h"


namespace TheBrick
{
    class CBullet : public CGameObject
    {
    public:
        float m_lifeTime;
    private:


    public:
        CBullet(CBrickManager* a_pBrickManager,PuRe_Vector3F a_Position, PuRe_Vector3F a_Speed, ong::World& a_rWorld);
        ~CBullet();
        void Update(float a_DeltaTime) override;
    };
}

#endif /* _BULLET_H_ */