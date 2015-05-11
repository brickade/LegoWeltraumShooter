#include "Collider.h"
#include "World.h"

#include "Allocator.h"
#include "QuickHull.h"

namespace ong
{

	Collider::Collider(const ColliderDescription& descr)
		: m_pBody(nullptr),
		m_transform(descr.transform),
		m_pMaterial(descr.material),
		m_shape(descr.shape),
		m_pUserData(nullptr),
		m_next(nullptr)


	{
		calculateMassProperties();
		calculateAABB();
	}

	Collider::Collider(const ColliderData& data)
		: m_pBody(nullptr),
		m_transform(data.transform),
		m_pMaterial(data.pMaterial),
		m_massData(data.massData),
		m_aabb(data.aabb),
		m_shape(data.shape),
		m_pUserData(nullptr),
		m_next(nullptr)
	{

	}

	void Collider::calculateMassProperties()
	{
		calculateMassData(m_shape, m_pMaterial->density, &m_massData);
	}

	void Collider::setMaterial(Material* material)
	{
		m_pMaterial = material;

		calculateMassProperties();

		if (m_pBody)
		{
			m_pBody->calculateMassData();
		}
	}


	void Collider::calculateAABB()
	{
		switch (m_shape.getType())
		{
		case ShapeType::HULL:
		{
			mat3x3 rot = toRotMat(m_transform.q);
			Hull* hull = (Hull*)m_shape;

			vec3 min = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
			vec3 max = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for (int i = 0; i < hull->numVertices; ++i)
			{
				vec3 v = rot * hull->pVertices[i];

				for (int i = 0; i < 3; ++i)
				{
					if (v[i] < min[i]) min[i] = v[i];
					if (v[i] > max[i]) max[i] = v[i];
				}
			}

			m_aabb.e = 0.5f * (max - min);
			m_aabb.c = m_transform.p + min + m_aabb.e;
			break;
		}
		case ShapeType::SPHERE:
		{
			Sphere* sphere = (Sphere*)m_shape;
			m_aabb.c = transformVec3(sphere->c, m_transform);
			m_aabb.e = sphere->r * vec3(1, 1, 1);

			break;
		}
		case ShapeType::CAPSULE:
		{
			Capsule* c = (Capsule*)m_shape;
			m_aabb.c = 0.5f * (c->c1 + c->c2) + m_transform.p;

			for (int i = 0; i < 3; ++i)
			{
				vec3 c1 = transformVec3(c->c1, m_transform);
				vec3 c2 = transformVec3(c->c2, m_transform);

				m_aabb.e[i] = ong_MAX(ong_MAX(ong_MAX(abs(c1[i] + c->r), abs(c1[i] - c->r)),
					abs(c2[i] + c->r)),
					abs(c2[i] - c->r)) - m_aabb.c[i];
			}



			break;
		}
		default:
			m_aabb = { vec3(0, 0, 0), vec3(0, 0, 0) };

		}



	}

	void Collider::setPosition(const vec3& p)
	{
		m_aabb.c += (p - m_transform.p);


		m_transform.p = p;
	}
	void Collider::translate(const vec3& translation)
	{
		m_transform.p += translation;
		m_aabb.c += translation;
	}

	void Collider::setOrientation(const Quaternion& q)
	{
		m_transform.q = q;

		calculateAABB();
	}
	void Collider::rotate(const Quaternion& rotation)
	{
		m_transform.q = rotation * m_transform.q;

		calculateAABB();
	}


	void Collider::setTransform(const Transform& t)
	{
		m_transform = t;
		calculateAABB();
	}

	void Collider::setBody(Body* pBody)
	{
		m_pBody = pBody;
	}

	Body* Collider::getBody()
	{
		return m_pBody;
	}

	const Body* Collider::getBody() const
	{
		return m_pBody;
	}




	void Collider::setNext(Collider* pNext)
	{
		m_next = pNext;
	}


	void Collider::setPrev(Collider* pPrev)
	{
		m_prev = pPrev;
	}




	const AABB& Collider::getAABB() const
	{
		return m_aabb;
	}


	ColliderData Collider::getData() const
	{
		ColliderData data;
		data.transform = m_transform;
		data.pMaterial = m_pMaterial;
		data.massData = m_massData;
		data.aabb = m_aabb;
		data.shape = m_shape;

		return data;
	}
}