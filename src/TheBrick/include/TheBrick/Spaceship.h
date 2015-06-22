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
    private:
        ong::vec3 m_TargetVec;
        ong::vec3 m_TargetAng;

        std::string m_Name;

        PuRe_Vector3F m_MaxRotationSpeed; //X = Roll, Y = Yaw, Z = Pitch
        PuRe_Vector3F m_RotationAcceleration; //X = Roll, Y = Yaw, Z = Pitch
        float m_MaxSpeed;
        float m_SpeedAcceleration;
    public:
        int m_Life;
        int m_MaxLife;
        float m_Respawn;

    public:
        const std::string& GetName() const
        {
            return this->m_Name;
        }

        void SetName(const std::string& a_rName)
        {
            this->m_Name = a_rName;
        }

        void SetNameFromFilename(const std::string& a_rFilename)
        {
            this->m_Name = a_rFilename.substr(0, a_rFilename.find("."));
        }

    public:
        CSpaceship(ong::World& a_rWorld,std::string a_Name);
        ~CSpaceship();

        void CalculateData();
        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera) override;
        void Update(float a_DeltaTime) override;
        void Thrust(float a_Thrust);
        void Spin(float a_Spin);
        void Move(PuRe_Vector2F a_Move);
        void Shoot(std::vector<CBullet*>& a_rBullets, CBrickManager* a_pManager);
        static void Collision(ong::Collider* thisCollider, ong::Contact* contact);

        void Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld) override;
        void Serialize(CSerializer& a_pSerializer);
    };
}

#endif /* _SPACESHIP_H_ */