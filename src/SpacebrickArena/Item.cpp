#include "include/Item.h"

#include "TheBrick/Conversion.h"
#include "TheBrick/BrickInstance.h"

#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CItem::CItem(ong::World& a_rWorld, EItemType a_Type, ong::vec3 a_Position, ong::vec3 a_Velocity, ong::vec3 a_Rotation) : CGameObject(a_rWorld, nullptr), m_ItemType(a_Type)
    {
        this->m_Type = TheBrick::EGameObjectType::Item;
        PuRe_Color color;
        unsigned int brickID;
        switch (a_Type)
        {
        case EItemType::Repair:
            color = PuRe_Color(0.0f, 1.0f, 0.0f);
            brickID = 905;
            break;
        case EItemType::Shield:
            color = PuRe_Color(0.0f, 0.0f, 1.0f);
            brickID = 906;
            break;
        case EItemType::Speed:
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
            brickID = 907;
            break;
        }
        TheBrick::CBrickInstance* brick = new TheBrick::CBrickInstance(*sba_BrickManager->GetBrickArray()[brickID], *this, *sba_World, color, true);

        this->m_pBody->setPosition(a_Position);
        this->m_pBody->applyImpulse(a_Velocity);
        this->m_pBody->applyAngularImpulse(a_Rotation);
        this->m_Collided = false;
    }

    // **************************************************************************
    // **************************************************************************
    CItem::~CItem()
    {
    }

    // **************************************************************************
    // **************************************************************************
    EItemType CItem::GetType()
    {
        return this->m_ItemType;
    }

    // **************************************************************************
    // **************************************************************************
    void CItem::Update(float a_DeltaTime)
    {
        //int p = -1;
        //float distance = 20.0f;
        //ong::vec3 itemPos = this->m_pBody->getWorldCenter();
        //for (unsigned int i = 0; i<sba_Players.size();i++)
        //{
        //    float diff = TheBrick::OngToPuRe(sba_Players[i]->Ship->m_pBody->getWorldCenter() - itemPos).Length();
        //    if (diff < distance)
        //    {
        //        p = i;
        //        distance = diff;
        //    }
        //}

        //if (p != -1)
        //{
        //    ong::vec3 playerPos = sba_Players[p]->Ship->m_pBody->getWorldCenter();
        //    ong::vec3 diff = playerPos - itemPos;
        //    PuRe_Vector3F pdiff = TheBrick::OngToPuRe(playerPos - itemPos);
        //    float Speed = 30.0f*a_DeltaTime;
        //    if (Speed < 0.5f)
        //        Speed = 0.5f;
        //    if (Speed > 1.0f)
        //        Speed = 1.0f;
        //    ong::vec3 newPosition = itemPos + TheBrick::PuReToOng(pdiff*Speed);
        //    this->m_pBody->setPosition(newPosition);
        //}
    }

}