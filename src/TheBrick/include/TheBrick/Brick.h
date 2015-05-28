#ifndef _BRICK_H_
#define _BRICK_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>
#include <Onager/Shapes.h>
#include <Onager/World.h>

#include <cstring>

#include "BrickInstance.h"
#include "GameObject.h"
#include "Nub.h"
#include "Serializer.h"

namespace TheBrick
{
    class CBrick
    {
    // ------------------------------------------------------------------------
    // Member
    // ------------------------------------------------------------------------
    public:
        static const float SEGMENT_WIDTH;
        static const float SEGMENT_HEIGHT;
        static const int MAX_MODEL_PATH_LENGTH;

    private:
        unsigned int m_BrickId;
        char* m_ModelPath;
        PuRe_Model* m_pModel;
        PuRe_IMaterial* m_pMaterial;
        bool m_IsTransparent;
        PuRe_Vector3F m_Pivotoffset;
        PuRe_List<SNub> m_pNubs;
        PuRe_List<ong::ColliderData> m_pColliderData;
        PuRe_List<CBrickInstance*> m_pBrickInstances;
        PuRe_RenderInstance* m_pRenderInstances;
        int m_RenderInstancesCount;

    // ------------------------------------------------------------------------
    // Getter & Setter
    // ------------------------------------------------------------------------
    public:
        PuRe_List<CBrickInstance*> GetBrickInstances()
        {
            return this->m_pBrickInstances;
        }

        PuRe_RenderInstance* GetRenderInstances()
        {
            return this->m_pRenderInstances;
        }

        PuRe_Model* GetModel()
        {
            return this->m_pModel;
        }

        void SetModel(PuRe_Model* a_pModel)
        {
            this->m_pModel = a_pModel;
        }

        PuRe_IMaterial* GetMaterial()
        {
            return this->m_pMaterial;
        }

        void SetMaterial(PuRe_IMaterial* a_pMaterial)
        {
            this->m_pMaterial = a_pMaterial;
        }

        std::vector<SNub>& GetNubs()
        {
            return this->m_pNubs;
        }

        void SetNubs(const std::vector<SNub>& a_rNubs)
        {
            this->m_pNubs = a_rNubs;
        }

        std::vector<ong::ColliderData>& GetColliderData()
        {
            return this->m_pColliderData;
        }

        void SetColliderData(const std::vector<ong::ColliderData>& a_rColliderData)
        {
            this->m_pColliderData = a_rColliderData;
        }

        unsigned int GetBrickId() const
        {
            return this->m_BrickId;
        }

        void SetBrickId(unsigned int a_BrickId)
        {
            this->m_BrickId = a_BrickId;
        }

        const char* GetModelPath() const
        {
            return this->m_ModelPath;
        }

        void SetModelPath(char* a_ModelPath)
        {
            strcpy(this->m_ModelPath, a_ModelPath);
        }

        bool& GetIsTransparent()
        {
            return this->m_IsTransparent;
        }

        void SetIsTransparent(bool a_IsTransparent)
        {
            this->m_IsTransparent = a_IsTransparent;
        }

        PuRe_Vector3F& GetPivotOffset()
        {
            return this->m_Pivotoffset;
        }

        void SetPivotOffset(const PuRe_Vector3F a_PivotOffset)
        {
            this->m_Pivotoffset = a_PivotOffset;
        }

    // ------------------------------------------------------------------------
    // METHODS
    // ------------------------------------------------------------------------
    public:
        CBrick(PuRe_IMaterial& a_pMaterial);
        CBrick(PuRe_Model& a_pModel, PuRe_IMaterial& a_pMaterial);
        ~CBrick();

        CBrickInstance* CreateInstance(CGameObject& a_rGameObject, ong::World& a_pWorld);
        void AddInstance(CBrickInstance& a_rBrickInstance);
        void DeleteInstance(CBrickInstance& a_rBrickInstance);
        void RebuildRenderInstances();

        void Draw(PuRe_Renderer& a_rRenderer);

        void Deserialize(CSerializer& a_pSerializer, PuRe_IGraphics& a_pGraphics, ong::World& a_pWorld);
        void Serialize(CSerializer& a_pSerializer);

        int GetCategoryId();
    };
}

#endif /* _BRICK_H_ */