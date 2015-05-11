#pragma once


#include "MyMath.h"
#include <vector>
#include <stack>
#include "Body.h"
#include "Collider.h"
#include "Shapes.h"
#include "Allocator.h"
#include "Broadphase.h"
#include "Narrowphase.h"


namespace ong
{

	typedef Allocator<Body> BodyAllocator;
	typedef Allocator<Collider> ColliderAllocator;
	typedef Allocator<Hull> HullAllocator;
	typedef Allocator<Sphere> SphereAllocator;
	typedef Allocator<Capsule> CapsuleAllocator;
	typedef Allocator<Material> MaterialAllocator;


	struct PositionState
	{
		vec3 p;
		Quaternion q;
	};

	struct VelocityState
	{
		vec3 v;
		vec3 w;
	};

	struct MomentumState
	{
		vec3 l;
		vec3 a;
	};


	struct MassState
	{
		mat3x3 localInvI;
		mat3x3 invI;
		float invM;
	};

	struct WorldContext
	{
		PositionState* r;

		VelocityState* v;

		MomentumState* p;

		MassState* m;

	};


	// TODO
	//	
	//	-spatial partitioning
	//	-world queries
	//	-collision masks
	//	-sensors??
	//	-sleeping
	//
	//	-hull memory leaking

	class World
	{
	public:
		World(const vec3& gravity = vec3(0.0f, 0.0f, 0.0f));

		//simulation step, dt should be constant
		void step(float dt);

		Body* createBody(const BodyDescription& description);
		void destroyBody(Body* pBody);

		//create new Collider
		Collider* createCollider(const ColliderDescription& description);
		//create collider from existing collider data
		Collider* createCollider(const ColliderData& description);
		void destroyCollider(Collider* pCollider);

		Material* createMaterial(const Material& material);
		void destroyMaterial(Material* pMaterial);

		ShapePtr createShape(const ShapeDescription& descr);
		void destroyShape(ShapePtr shape);

		inline void World::setGravity(const vec3& gravity);


	ong_internal:

		//	--MANIPULATORS--

		void setProxy(int proxyID, const Proxy& proxy);

		//	--ACCESSORS--


		const Proxy& getProxy(int proxyID);


		std::vector<PositionState> m_r;
		std::vector<VelocityState> m_v;
		std::vector<MomentumState> m_p;
		std::vector<MassState> m_m;
		std::vector<Body*> m_b;


	private:

		static const int NUN_VELOCITY_ITERATIONS = 8;

		Body* m_pBody;
		int m_numBodies;
		int m_numColliders;
		vec3 m_gravity;

		HGrid m_hGrid;
		ContactManager m_contactManager;

		BodyAllocator m_bodyAllocator;
		ColliderAllocator m_colliderAllocator;
		HullAllocator m_hullAllocator;
		SphereAllocator m_sphereAllocator;
		CapsuleAllocator m_capsuleAllocator;
		MaterialAllocator m_materialAllocator;
	};



	inline void World::setGravity(const vec3& gravity)
	{
		m_gravity = gravity;
	}
	

}