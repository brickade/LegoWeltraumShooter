#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Onager/World.h"

#include "GameObject.h"
#include "DebugDraw.h"
#include "CSVParser.h"
#include "Bullet.h"
#include "BrickManager.h"

namespace TheBrick
{
    class CSpaceship : public CGameObject
    {
    public:
        CCSVParser* m_pCSVFile;

    private:
        ong::vec3 m_TargetVec;
        ong::vec3 m_TargetAng;

    public:
        CSpaceship(ong::World& a_rWorld,ong::vec3& a_rPosition);
        ~CSpaceship();

        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera) override;
        void Update(float a_DeltaTime) override;

        void HandleInput(int a_CID,PuRe_IInput* a_pInput, float a_DeltaTime, std::vector<CBullet*>& a_rBullets, CBrickManager* a_pManager);

        void Thrust(float a_Thrust);
        void Spin(float a_Spin);
        void Move(PuRe_Vector2F a_Move);
        void Shoot(std::vector<CBullet*>& a_rBullets, CBrickManager* a_pManager);

        void Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld) override;
        void Serialize(CSerializer& a_pSerializer);
    };
}

#endif /* _SPACESHIP_H_ */