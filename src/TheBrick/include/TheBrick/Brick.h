
#ifndef _BRICK_H_
#define _BRICK_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>
#include <Onager/Shapes.h>
#include <Onager/World.h>

#include "BrickInstance.h"
#include "Nub.h"
#include "Serializer.h"

namespace TheBrick
{
	//Forward declaration
	class CBrickInstance;

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

		// ------------------------------------------------------------------------
		// Getter & Setter
		// ------------------------------------------------------------------------
	public:
		PuRe_Model* GetModel()
		{
			return m_pModel;
		}

		void SetModel(PuRe_Model* m_p_model)
		{
			m_pModel = m_p_model;
		}

		PuRe_IMaterial* GetMaterial()
		{
			return m_pMaterial;
		}

		void SetMaterial(PuRe_IMaterial* m_p_material)
		{
			m_pMaterial = m_p_material;
		}

		std::vector<SNub>& GetNubs()
		{
			return m_pNubs;
		}

		void SetNubs(const std::vector<SNub>& m_p_nubs)
		{
			m_pNubs = m_p_nubs;
		}

		std::vector<ong::ColliderData>& GetColliderData()
		{
			return m_pColliderData;
		}

		void SetColliderData(const std::vector<ong::ColliderData>& m_p_collider_data)
		{
			m_pColliderData = m_p_collider_data;
		}

		unsigned int GetBrickId() const
		{
			return m_BrickId;
		}

		void SetBrickId(unsigned m_brick_id)
		{
			m_BrickId = m_brick_id;
		}

		const char* GetModelPath() const
		{
			return m_ModelPath;
		}

		void SetModelPath(char* m_model_path)
		{
			strcpy(this->m_ModelPath, m_model_path);
		}

		bool& GetIsTransparent()
		{
			return m_IsTransparent;
		}

		void SetIsTransparent(bool m_is_transparent)
		{
			m_IsTransparent = m_is_transparent;
		}

		PuRe_Vector3F& GetPivotOffset()
		{
			return m_Pivotoffset;
		}

		void SetPivotOffset(const PuRe_Vector3F m_pivotoffset)
		{
			m_Pivotoffset = m_pivotoffset;
		}

		// ------------------------------------------------------------------------
		// METHODS
		// ------------------------------------------------------------------------
	public:
		CBrick();
		CBrick(PuRe_Model* a_pModel);
		~CBrick();

		CBrickInstance* CreateInstance(ong::World* a_pWorld);

		void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, PuRe_Vector3F a_Position, PuRe_Vector3F a_Rotation, PuRe_Color a_Color = PuRe_Color(1, 1, 1, 1), PuRe_Vector3F a_Scale = PuRe_Vector3F(1, 1, 1));

		void Deserialize(CSerializer* a_pSerializer, PuRe_IGraphics* a_pGraphics, PuRe_IMaterial* a_pMaterial, ong::World* a_pWorld);
		void Serialize(CSerializer* a_pSerializer);

		int GetCategoryId();
	};
}

#endif /* _BRICK_H_ */