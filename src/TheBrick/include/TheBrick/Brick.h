#ifndef _BRICK_H_
#define _BRICK_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>

#include "BrickInstance.h"
#include "Nub.h"
#include "Serializer.h"

namespace TheBrick
{
    //Forward declaration
    class CBrickInstance;

    class CBrick
    {
    public:
        unsigned int m_BrickId;
        static const float SEGMENT_WIDTH;
        static const float SEGMENT_HEIGHT;

        PuRe_Model* m_pModel;
        PuRe_List<SNub> m_pNubs;
        PuRe_List<ong::ColliderData> m_pColliderData;
        
    public:
        CBrick(PuRe_Model* a_pModel);
        ~CBrick();

        static CBrick* FromFile(PuRe_IGraphics* a_pGraphics, const char* a_pPath);
        CBrickInstance* CreateInstance();

        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, PuRe_Vector3F a_position, PuRe_Vector3F a_Scale, PuRe_Vector3F a_rotation);

        PuRe_List<SNub>* GetNubs();

        static CBrick* Deserialize(CSerializer* a_pSerializer, PuRe_IGraphics* a_pGraphics, PuRe_IMaterial* a_pMaterial);
        void Serialize(CSerializer* a_pSerializer);
    };
}

#endif /* _BRICK_H_ */