#ifndef _ITEM_H_
#define _ITEM_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Onager/World.h"

#include "TheBrick/GameObject.h"


namespace sba
{
    enum EItemType { Shield, Repair, Speed };

    struct SItemData
    {
        EItemType Type;
        ong::vec3 Pos;
        ong::vec3 Vel;
        ong::vec3 Rot;
        float Respawn;
    };

    class CItem : public TheBrick::CGameObject
    {
    public:
        bool m_Collided;
        EItemType m_ItemType;
    public:
        CItem(ong::World& a_rWorld, EItemType a_Type, ong::BodyDescription* a_pdesc);
        ~CItem();
    public:
        EItemType GetType();
        void Update(float a_DeltaTime) override;
    };
}

#endif /* _ITEM_H_ */