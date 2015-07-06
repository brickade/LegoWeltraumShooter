#pragma once


#include "defines.h"
#include "Shapes.h"
#include "MassProperties.h"

namespace ong
{

	struct WorldMemory;
	class World;
	class Body;
	class Collider;
	struct Contact;



	template<typename T>
	class Allocator;

	typedef Allocator<Hull> HullAllocator;


	struct Material
	{
		float density;
		float restitution;
		float friction;
	};


	typedef void(*CollisionCallback)(Collider* thisCollider, Contact* contact);
	struct ColliderCallbacks
	{
		CollisionCallback beginContact = 0;
		CollisionCallback endContact = 0;
		CollisionCallback preSolve = 0;
		CollisionCallback postSolve = 0;
	};
	

	struct ColliderDescription
	{
		Transform transform;
		Material* material;
		ShapePtr shape;
		bool isSensor;
	};

	struct ColliderData
	{
		Transform transform;
		Material* pMaterial;
		MassData massData;
		AABB aabb;
		ShapePtr shape;
		uint32 collisionGroup;
		uint32 collisionFilter;
		bool isSensor;
	};





	// todo set shape
	class Collider
	{
	public:

		//	--CREATORS--
		Collider(const ColliderDescription& description);
		// create collider without recalculating all data
		Collider(const ColliderData& data);

		//	--MANIPULATORS--

		void setUserData(void* pUserData);

		void setCollisionGroup(uint32 collisionGroup);
		void setCollisionFilter(uint32 collisionFilter);

		void setCallbacks(const ColliderCallbacks& callbacks);

		void calculateMassProperties();

		void setMaterial(Material* material);

		void calculateAABB();

		void setPosition(const vec3& p);
		void translate(const vec3& translation);

		void setOrientation(const Quaternion& q);
		void rotate(const Quaternion& rotation);

		void setTransform(const Transform& t);

		//	--ACCESSORS--

		const MassData& getMassData();
		Body* getBody();
		const Body* getBody() const;

		bool isSensor() const;

		void* getUserData() const;

		uint32 getCollisionGroup() const;
		uint32 getCollisionFilter() const;

		ColliderCallbacks getColliderCallbacks() const;

		Transform& getTransform();
		const Transform& getTransform() const;

		Material* getMaterial();

		Collider* getNext();
		Collider* getPrev();

		const AABB& getAABB() const;

		const ShapePtr getShape() const;

		ColliderData getData() const;

	ong_internal:
		void setBody(Body* pBody);
		void setNext(Collider* pNext);
		void setPrev(Collider* pPrev);
		
		void callbackBeginContact(Contact* contact);
		void callbackEndContact(Contact* contact);
		void callbackPreSolve(Contact* contact);
		void callbackPostSolve(Contact* contact);


	private:
		Body* m_pBody;

		Transform m_transform;

		Material* m_pMaterial;

		MassData m_massData;

		AABB m_aabb;
		ShapePtr m_shape;

		bool m_sensor;

		void* m_pUserData;
		ColliderCallbacks m_callbacks;

		uint32 m_collisionGroup;
		uint32 m_collisionFilter;

		Collider* m_prev;
		Collider* m_next;
	};




	inline void Collider::setUserData(void* pUserData)
	{
		m_pUserData = pUserData;
	}

	inline void Collider::setCollisionGroup(uint32 collisionGroup)
	{
		m_collisionGroup = collisionGroup;
	}

	inline void Collider::setCollisionFilter(uint32 collisionFilter)
	{
		m_collisionFilter = collisionFilter;
	}

	inline void Collider::setCallbacks(const ColliderCallbacks& callbacks)
	{
		m_callbacks = callbacks;
	}



	inline void Collider::callbackBeginContact(Contact* contact)
	{
		if (m_callbacks.beginContact)
			m_callbacks.beginContact(this, contact);
	}

	inline void Collider::callbackEndContact(Contact* contact)
	{
		if (m_callbacks.endContact)
			m_callbacks.endContact(this, contact);
	}

	inline void Collider::callbackPreSolve(Contact* contact)
	{
		if (m_callbacks.preSolve)
			m_callbacks.preSolve(this, contact);
	}
		
	inline void Collider::callbackPostSolve(Contact* contact)
	{
		if (m_callbacks.postSolve)
			m_callbacks.postSolve(this, contact);
	}


	inline const MassData& Collider::getMassData()
	{
		return m_massData;
	}

	inline Collider* Collider::getNext()
	{
		return m_next;
	}

	inline Collider* Collider::getPrev()
	{
		return m_prev;
	}


	inline Transform& Collider::getTransform()
	{
		return m_transform;
	}

	inline const Transform& Collider::getTransform() const
	{
		return m_transform;
	}

	inline const ShapePtr Collider::getShape() const
	{
		return m_shape;
	}


	inline Material* Collider::getMaterial()
	{
		return m_pMaterial;
	}

	inline bool Collider::isSensor() const
	{
		return m_sensor;
	}


	inline void* Collider::getUserData() const
	{
		return m_pUserData;
	}
	
	inline uint32 Collider::getCollisionGroup() const
	{
		return m_collisionGroup;
	}

	inline uint32 Collider::getCollisionFilter() const
	{
		return m_collisionFilter;
	}

	inline ColliderCallbacks Collider::getColliderCallbacks() const
	{
		return m_callbacks;
	}
}