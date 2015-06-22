#pragma once

#include "defines.h"
#include "geomMath.h"
#include <float.h>

namespace ong
{



	struct Sphere
	{
		vec3 c;
		float r;
	};

	struct AABB
	{
		vec3 c;
		vec3 e;
	};


	struct Capsule
	{
		vec3 c1;
		vec3 c2;
		float r;
	};

	struct Face;

	struct HalfEdge
	{
		uint8 tail;

		uint8 twin;
		uint8 next;

		uint8 face;
	};

	struct Face
	{
		uint8 edge;
	};


	struct Hull
	{
		vec3 centroid; // not cm

		int32 numVertices;
		vec3* pVertices;

		int32 numEdges;
		HalfEdge* pEdges;

		int32 numFaces;
		Face* pFaces;
		Plane* pPlanes;

		//todo not sure about that
		float epsilon;
	};

	struct ShapeType
	{
		enum Type
		{
			SPHERE,
			CAPSULE,
			HULL,
			COUNT,
		};
	};


	struct ShapeConstruction
	{
		enum Type
		{
			HULL_FROM_POINTS = ShapeType::COUNT,
			HULL_FROM_BOX,
		};
	};

	struct ShapeDescription
	{
		union
		{
			// create Shape from existing shape
			ShapeType::Type shapeType;
			// construct new Shape
			ShapeConstruction::Type constructionType;
			// can be either ShapeType or ShapeConstruction
			int type;
		};
		union
		{
			Sphere sphere;
			Capsule capsule;
			Hull hull;
			struct
			{
				vec3* points;
				int numPoints;
			} hullFromPoints;
			struct
			{
				vec3 c;
				vec3 e;
			} hullFromBox;
		};
	};

	class ShapePtr
	{
	public:
		ShapePtr();
		ShapePtr(Sphere* pSphere);
		ShapePtr(Capsule* pCapsule);
		ShapePtr(Hull* pHull);

		ShapeType::Type getType() const;

		Sphere* toSphere();
		Capsule* toCapsule();
		Hull* toHull();

		const Sphere* toSphere() const;
		const Capsule* toCapsule() const;
		const Hull* toHull() const;

		operator Sphere*();
		operator Capsule*();
		operator Hull*();

		operator const Sphere*() const;
		operator const Capsule*() const;
		operator const Hull*() const;

		bool operator!() const;

	private:
		ShapeType::Type m_type;
		union
		{
			void* m_pShape;
			Sphere* m_pSphere;
			Capsule* m_pCapsule;
			Hull* m_pHull;
		};
	};



	// aabb
	float sqDistPointAABB(const vec3& p, const AABB& aabb);
	AABB calculateAABB(ShapePtr shape, const Transform& transform);
	AABB calculateAABB(const Hull* hull, const Transform& transform);
	AABB calculateAABB(const Sphere* sphere, const Transform& transform);
	AABB calculateAABB(const Capsule* capsule, const Transform& transform);

	// hull
	vec3 getHullSupport(const vec3& dir, const Hull* hull, int* idx = 0);
	vec3 getSegmentSupport(const vec3& dir, const vec3& a, const vec3& b, int* idx = 0);
	vec3 closestPointOnHull(const vec3& p, const Hull* hull, float epsilon = FLT_EPSILON);

	// return the squared distance between points
	float closestPtSegmentHull(const vec3& a, const vec3& b, const Hull* hull, vec3& cSegment, vec3& cHull, float epsilon = FLT_EPSILON);

	

	// broad
	bool overlap(Sphere* sphere, AABB* aabb);
	bool overlap(Sphere* sphereA, Sphere* sphereB);
	bool overlap(const AABB* aabbA, const AABB* aabbB);
	// is not 100% accurate! can false hit!
	bool overlap(const AABB& a, const AABB& b, const vec3& t, const mat3x3& rot);

	// narrow
	bool overlap(const ShapePtr shapeA, const ShapePtr shapeB, const Transform& ta, const Transform& tb);
	bool overlap(const Hull* hullA, const Hull* hullB, const Transform& t1, const Transform& t2);
	bool overlap(const Sphere* sphereA, const Sphere* sphereB, const Transform& t1, const Transform& t2);
	bool overlap(const Capsule* capsuleA, const Capsule* capsuleB, const Transform& ta, const Transform& tb);
	bool overlap(const Sphere* sphereA, const Capsule* capsuleB, const Transform& t1, const Transform& t2);
	bool overlap(const Sphere* sphereA, const Hull* hullB, const Transform& t1, const Transform& t2);
	bool overlap(const Capsule* capsuleA, const Hull* hullB, const Transform& t1, const Transform& t2);

	// rays
	bool intersectRayAABB(const vec3& origin, const vec3& dir, const AABB& aabb, float& tmin, vec3& p);
	bool intersectRayHull(const vec3& origin, const vec3& dir, const Hull* hull, float& tmin, vec3& p, vec3& n);
	bool intersectRaySphere(const vec3& origin, const vec3& dir, const Sphere* sphere, float& tmin, vec3& p, vec3& n);
	bool intersectRayCapsule(const vec3& origin, const vec3& dir, const Capsule* capsule, float& tmin, vec3& p, vec3& n);

	//aaabb

	void mergeAABBAABB(AABB* a, AABB* b);
	AABB transformAABB(const AABB* a, const Transform* t);



	// ShapePtr
	inline ShapePtr::ShapePtr()
		:m_type((ShapeType::Type) - 1),
		m_pShape(0) {}


	inline ShapePtr::ShapePtr(Sphere* pSphere)
		: m_type(ShapeType::SPHERE),
		m_pSphere(pSphere) {}



	inline ShapePtr::ShapePtr(Capsule* pCapsule)
		: m_type(ShapeType::CAPSULE),
		m_pCapsule(pCapsule){}


	inline ShapePtr::ShapePtr(Hull* pHull)
		: m_type(ShapeType::HULL),
		m_pHull(pHull) {}


	inline ShapeType::Type ShapePtr::getType() const
	{
		return m_type;
	}

	inline Sphere* ShapePtr::toSphere()
	{
		assert(m_type == ShapeType::SPHERE);
		return m_pSphere;
	}

	inline Capsule* ShapePtr::toCapsule()
	{
		assert(m_type == ShapeType::CAPSULE);
		return m_pCapsule;
	}


	inline Hull* ShapePtr::toHull()
	{
		assert(m_type == ShapeType::HULL);
		return m_pHull;
	}

	inline const Sphere* ShapePtr::toSphere() const
	{
		assert(m_type == ShapeType::SPHERE);
		return m_pSphere;
	}

	inline const Capsule* ShapePtr::toCapsule() const
	{
		assert(m_type == ShapeType::CAPSULE);
		return m_pCapsule;
	}

	inline const Hull* ShapePtr::toHull() const
	{
		assert(m_type == ShapeType::HULL);
		return m_pHull;
	}


	inline ShapePtr::operator Sphere*()
	{
		assert(m_type == ShapeType::SPHERE);
		return m_pSphere;
	}

	inline ShapePtr::operator Capsule*()
	{
		assert(m_type == ShapeType::CAPSULE);
		return m_pCapsule;
	}

	inline ShapePtr::operator Hull*()
	{
		assert(m_type == ShapeType::HULL);
		return m_pHull;
	}


	inline ShapePtr::operator const Sphere*() const
	{
		assert(m_type == ShapeType::SPHERE);
		return m_pSphere;
	}

	inline ShapePtr::operator const Capsule*() const
	{
		assert(m_type == ShapeType::CAPSULE);
		return m_pCapsule;
	}

	inline ShapePtr::operator const Hull*() const
	{
		assert(m_type == ShapeType::HULL);
		return m_pHull;
	}

	inline bool ShapePtr::operator!() const
	{
		return !m_pShape;
	}

}