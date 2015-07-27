#ifndef _ITEM_H_
#define _ITEM_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Onager/World.h"

#include "TheBrick/GameObject.h"


namespace sba
{
    enum EItemType { Shield, Repair, Speed };
    class CItem : public TheBrick::CGameObject
    {
    private:
        EItemType m_ItemType;
    public:
        bool m_Collided;
    public:
        CItem(ong::World& a_rWorld, EItemType a_Type, ong::vec3 a_Position, ong::vec3 a_Velocity, ong::vec3 a_Rotation);
        ~CItem();
    public:
        EItemType GetType();
        void Update(float a_DeltaTime) override;
    };
}

#endif /* _ITEM_H_ */