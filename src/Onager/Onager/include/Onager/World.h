#pragma once


#include "MyMath.h"
#include <vector>
#include <stack>
#include "States.h"
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




	// TODO
	//	
	//	-sensors
	//	-continuos collision detection
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

		bool queryRay(const vec3& origin, const vec3& dir, RayQueryResult* hit, float tmax = FLT_MAX);
		bool queryCollider(const Collider* collider);
		bool queryCollider(Collider* collider, ColliderQueryCallBack callback);
		bool queryShape(ShapePtr shape, const Transform& transform);
		bool queryShape(ShapePtr shape, const Transform& transform, ShapeQueryCallBack callback, void* userData);

		inline void World::setGravity(const vec3& gravity);

		

	ong_internal:

		//	--MANIPULATORS--

		void updateProxy(const ProxyID* proxyID);
		void removeContact(Contact* pContact);

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