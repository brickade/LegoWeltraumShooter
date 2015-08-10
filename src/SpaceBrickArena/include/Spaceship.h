#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Onager/World.h"

#include "TheBrick/GameObject.h"
#include "DestructibleObject.h"
#include "TheBrick/DebugDraw.h"
#include "TheBrick/CSVParser.h"
#include "Bullet.h"
#include "Item.h"

namespace sba
{
    class CBrick;

    class CSpaceship : public CDestructibleObject
    {
    public:
        static const int MAX_BRICK_COUNT;
        static const int MAX_BRICK_WIDTH;
        static const int MAX_BRICK_HEIGHT;
        static const int MAX_COCKPITS;
        static const int MAX_ENGINES;
        static const int MAX_WEAPONS;

    private:
        TheBrick::SBrickData* m_pBrickArray;
        unsigned int m_BrickCount;

        void CalculateProperties();
        void CalculateReset();

        ong::vec3 m_TargetVec;
        ong::vec3 m_TargetAng;

        std::string m_Name;

        PuRe_Vector3F m_MaxRotationSpeed; //X = Roll, Y = Yaw, Z = Pitch
        PuRe_Vector3F m_RotationAcceleration; //X = Roll, Y = Yaw, Z = Pitch
        float m_MaxSpeed;
        float m_SpeedAcceleration;
        float m_SpeedFactor;
        float m_OldMass;

    public:
        int m_Shield;
        int m_Life;
        int m_MaxLife;
        float m_Respawn;
        float m_Shake;
        std::vector<PuRe_ParticleEmitter*> m_EngineEmitter;

    public:
        float GetMaxSpeed() const
        {
            return this->m_MaxSpeed;
        }
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
		void ReCalculateData();

        void Respawn(ong::vec3 a_Position);

        void Update(int a_OID, float a_DeltaTime,int a_Time);
        void Thrust(float a_Thrust);
        void Spin(float a_Spin);
        void Move(PuRe_Vector2F a_Move);
        void Shoot(int a_Weapon, std::vector<CBullet*>& a_rBullets, SPlayer* a_pOwner, PuRe_Vector2F a_Direction, int a_Time);
        static void Collision(ong::Collider* thisCollider, ong::Contact* contact);

        void Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld) override;
        void Serialize(TheBrick::CSerializer& a_pSerializer);

        void Reset(TheBrick::CBrick& a_rStartBrick, ong::World& a_rWorld);
        void Reset();
        void DrawEmitter(PuRe_Sprite* a_pSprite, PuRe_IMaterial* a_pMaterial,PuRe_PointLight* a_pLight,PuRe_IMaterial* a_pLightMaterial);

        //Dont use raw pointer to avoid destructor call with std::vector clear/erase
        void GetWeapons(std::vector<TheBrick::CBrickInstance**>& a_rOutVector);
        void GetEngines(std::vector<TheBrick::CBrickInstance**>& a_rOutVector);
        void GetCockpits(std::vector<TheBrick::CBrickInstance**>& a_rOutVector);
    };
}

#endif /* _SPACESHIP_H_ */