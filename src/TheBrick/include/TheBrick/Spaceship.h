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

        std::string m_Name;

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