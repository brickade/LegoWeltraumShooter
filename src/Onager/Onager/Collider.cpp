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
		m_sensor(descr.isSensor),
		m_pUserData(nullptr),
		m_next(nullptr),
		m_collisionGroup(0),
		m_collisionFilter(0)
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
		m_sensor(data.isSensor),
		m_pUserData(nullptr),
		m_next(nullptr),
		m_collisionGroup(data.collisionGroup),
		m_collisionFilter(data.collisionFilter)
	{

	}

	void Collider::calculateMassProperties()
	{
		//if (!m_sensor)
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
		m_aabb = ong::calculateAABB(m_shape, m_transform);
	}

	void Collider::setPosition(const vec3& p)
	{
		m_aabb.c += (p - m_transform.p);
		m_transform.p = p;

		if (m_pBody)
			m_pBody->calculateTree();
	}
	void Collider::translate(const vec3& translation)
	{
		m_transform.p += translation;
		m_aabb.c += translation;

		if (m_pBody)
			m_pBody->calculateTree();
	}

	void Collider::setOrientation(const Quaternion& q)
	{
		m_transform.q = q;
		calculateAABB();

		if (m_pBody)
			m_pBody->calculateTree();
	}
	void Collider::rotate(const Quaternion& rotation)
	{
		m_transform.q = rotation * m_transform.q;
		calculateAABB();

		if (m_pBody)
			m_pBody->calculateTree();
	}


	void Collider::setTransform(const Transform& t)
	{
		m_transform = t;
		calculateAABB();

		if (m_pBody)
			m_pBody->calculateTree();
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
		data.collisionGroup = m_collisionGroup;
		data.collisionFilter = m_collisionFilter;
		data.isSensor = m_sensor;

		return data;
	}
}