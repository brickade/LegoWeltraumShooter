#include "World.h"
#include "Narrowphase.h"
#include "ContactSolver.h"
#include "QuickHull.h"
#include "Profiler.h"


namespace ong
{



	World::World(const vec3& gravity)
		: m_gravity(gravity),
		m_bodyAllocator(BodyAllocator(32)),
		m_colliderAllocator(ColliderAllocator(32)),
		m_hullAllocator(HullAllocator(32)),
		m_sphereAllocator(SphereAllocator(32)),
		m_capsuleAllocator(CapsuleAllocator(32)),
		m_materialAllocator(MaterialAllocator(5)),
		m_pBody(nullptr),
		m_numBodies(0),
		m_numColliders(0)

	{
	}


	void World::step(float dt)
	{

		assert(dt != 0.0f);


		Body* b = m_pBody;
		while (b != nullptr)
		{
			b->calculateAABB();
			m_hGrid.updateBody(b->getProxyID());
			//b->clearContacts();
			b = b->getNext();
		}


		// broadphase
		ong_START_PROFILE(BROADPHASE);
		Pair* pairs = new Pair[m_numBodies * m_numBodies];
		int numPairs = 0;
		numPairs = m_hGrid.generatePairs(pairs);

		ong_END_PROFILE(BROADPHASE);
		assert(numPairs <= m_numBodies*m_numBodies);

		//narrowphase
		ong_START_PROFILE(NARROWPHASE);
		//todo ...
		//m_contactManager.generateContacts(pairs, numPairs, m_numColliders*m_numColliders);

		m_contactManager.generateContacts(pairs, numPairs, 3 * numPairs);
		
		ong_END_PROFILE(NARROWPHASE);

		//integrate
		ong_START_PROFILE(INTEGRATE);

		for (int i = 0; i < m_numBodies; ++i)
		{
			mat3x3 q = toRotMat(m_r[i].q);

			m_m[i].invI = q * m_m[i].localInvI * transpose(q);

			if (m_m[i].invM != 0.0f)
				m_p[i].l += dt * 1.0f / m_m[i].invM * m_gravity;

			m_v[i].v = m_m[i].invM * m_p[i].l;
			m_v[i].w = m_m[i].invI* m_p[i].a;
		}

		ong_END_PROFILE(INTEGRATE);

		//resolution	  
		ong_START_PROFILE(RESOLUTION);
		{
			WorldContext context;
			context.r = m_r.data();
			context.v = m_v.data();
			context.p = m_p.data();
			context.m = m_m.data();

			int numContacts = 0;
			Contact** c = m_contactManager.getContacts(&numContacts);

			ContactConstraint* contactConstraints = new ContactConstraint[numContacts];
			preSolveContacts(&context, c, numContacts, 1.0f / dt, contactConstraints);

			for (int i = 0; i < 16; ++i)
			{
				solveContacts(&context, c, numContacts, contactConstraints);
			}

			postSolveContacts(&context, c, numContacts, contactConstraints);

			//for (int i = 0; i < numContacts; ++i)
			//{
			//	c[i]->colliderA->callbackPostSolve(c[i]);
			//	c[i]->colliderB->callbackPostSolve(c[i]);
			//}

			delete[] contactConstraints;
		}
		ong_END_PROFILE(RESOLUTION);

		ong_START_PROFILE(INTEGRATE2);
		for (int i = 0; i < m_numBodies; ++i)
		{
			m_r[i].p += dt * m_v[i].v;

			vec3 wAxis = dt * m_v[i].w;
			float wScalar = sqrt(lengthSq(wAxis));
			
			wScalar = ong_MAX(-0.25f*ong_PI, ong_MIN(0.25f*ong_PI, wScalar));

			if (wScalar != 0.0f)
				m_r[i].q = QuatFromAxisAngle(1.0f / wScalar * wAxis, wScalar) * m_r[i].q;
		}
		ong_END_PROFILE(INTEGRATE2);

		delete[] pairs;
	}

	Body* World::createBody(const BodyDescription& description)
	{
		int idx = m_numBodies++;
		m_r.push_back(PositionState());
		m_v.push_back(VelocityState());
		m_p.push_back(MomentumState());
		m_m.push_back(MassState());
		m_b.push_back(nullptr);

		Body* body = m_bodyAllocator.sNew(Body(description, this, idx));

		m_b[idx] = body;

		body->setNext(m_pBody);
		body->setPrevious(nullptr);

		const ProxyID* proxyID = m_hGrid.addBody(body);
		
		body->setProxyID(proxyID);


		if (m_pBody != nullptr)
			m_pBody->setPrevious(body);

		m_pBody = body;


		return body;
	}



	void World::destroyBody(Body* pBody)
	{



		int idx = pBody->getIndex();

		m_numBodies--;

		m_r[idx] = m_r[m_numBodies];
		m_v[idx] = m_v[m_numBodies];
		m_p[idx] = m_p[m_numBodies];
		m_m[idx] = m_m[m_numBodies];
		m_b[idx] = m_b[m_numBodies];


		m_b[idx]->setIndex(idx);

		m_r.pop_back();
		m_v.pop_back();
		m_p.pop_back();
		m_m.pop_back();
		m_b.pop_back();




		m_contactManager.removeBody(pBody);


		Collider* c = pBody->getCollider();
		while (c)
		{
			destroyCollider(c);
			c = c->getNext();
		}

		if (pBody->getPrevious())
			pBody->getPrevious()->setNext(pBody->getNext());
		if (pBody->getNext())
			pBody->getNext()->setPrevious(pBody->getPrevious());

		if (m_pBody == pBody)
			m_pBody = pBody->getNext();

		m_hGrid.removeBody(pBody->getProxyID());


		m_bodyAllocator.sDelete(pBody);
	}

	Collider* World::createCollider(const ColliderDescription& description)
	{

		Collider* collider = nullptr;

		collider = m_colliderAllocator.sNew(Collider(description));
		m_numColliders++;

		return collider;
	}

	Collider* World::createCollider(const ColliderData& data)
	{

		Collider* collider = nullptr;

		collider = m_colliderAllocator.sNew(Collider(data));
		m_numColliders++;

		return collider;
	}

	void World::destroyCollider(Collider* pCollider)
	{
		if (pCollider->getBody())
			pCollider->getBody()->removeCollider(pCollider);

		m_colliderAllocator.sDelete(pCollider);
		m_numColliders--;
	}

	Material* World::createMaterial(const Material& material)
	{
		return m_materialAllocator(material);
	}


	void World::destroyMaterial(Material* pMaterial)
	{
		m_materialAllocator.sDelete(pMaterial);
	}

	ShapePtr World::createShape(const ShapeDescription& descr)
	{
		switch (descr.type)
		{
		case ShapeType::SPHERE:
			return ShapePtr(m_sphereAllocator(descr.sphere));
		case ShapeType::CAPSULE:
			return ShapePtr(m_capsuleAllocator(descr.capsule));
		case ShapeType::HULL:
		{
			Hull* h = m_hullAllocator(descr.hull);

			h->pEdges = new HalfEdge[h->numEdges];
			h->pVertices = new vec3[h->numVertices];
			h->pFaces = new Face[h->numFaces];
			h->pPlanes = new Plane[h->numFaces];

			memcpy(h->pEdges, descr.hull.pEdges, sizeof(HalfEdge) * h->numEdges);
			memcpy(h->pVertices, descr.hull.pVertices, sizeof(vec3) * h->numVertices);
			memcpy(h->pFaces, descr.hull.pFaces, sizeof(Face) * h->numFaces);
			memcpy(h->pPlanes, descr.hull.pPlanes, sizeof(Plane) * h->numFaces);

			return ShapePtr(h);
		}
		case ShapeConstruction::HULL_FROM_POINTS:
		{
			Hull* h = m_hullAllocator();
			quickHull(descr.hullFromPoints.points, descr.hullFromPoints.numPoints, h);
			return ShapePtr(h);
		}
		case ShapeConstruction::HULL_FROM_BOX:
		{

			// todo without quickhull
			Hull* h = m_hullAllocator();

			vec3 c = descr.hullFromBox.c;
			vec3 e = descr.hullFromBox.e;

			vec3 box[8] =
			{
				vec3(-e.x, -e.y, e.z) + c,
				vec3(-e.x, -e.y, -e.z) + c,
				vec3(e.x, -e.y, -e.z) + c,
				vec3(e.x, -e.y, e.z) + c,

				vec3(-e.x, e.y, e.z) + c,
				vec3(-e.x, e.y, -e.z) + c,
				vec3(e.x, e.y, -e.z) + c,
				vec3(e.x, e.y, e.z) + c
			};

			quickHull(box, 8, h);
			return ShapePtr(h);
		}
		default:
			return ShapePtr();
		}
	}

	void World::destroyShape(ShapePtr shape)
	{
		switch (shape.getType())
		{
		case ShapeType::SPHERE:
			m_sphereAllocator.sDelete(shape);
			return;
		case ShapeType::CAPSULE:
			m_capsuleAllocator.sDelete(shape);
			return;
		case ShapeType::HULL:
		{
			delete[] shape.toHull()->pEdges;
			delete[] shape.toHull()->pVertices;
			delete[] shape.toHull()->pFaces;
			delete[] shape.toHull()->pPlanes;

			m_hullAllocator.sDelete(shape);
			return;
		}
		}
	}

	bool World::queryRay(const vec3& origin, const vec3& dir, RayQueryResult* hit, float tmax)
	{
		//Profiler profile("Query Ray");

		return m_hGrid.queryRay(origin, dir, hit, tmax);
	}

	bool World::queryCollider(const Collider* collider)
	{
		return m_hGrid.queryCollider(collider);
	}

	bool World::queryCollider(Collider* collider, ColliderQueryCallBack callback)
	{
		return m_hGrid.queryCollider(collider, callback);
	}

	bool World::queryShape(ShapePtr shape, const Transform& transform)
	{
		return m_hGrid.queryShape(shape, transform);
	}

	bool World::queryShape(ShapePtr shape, const Transform& transform, ShapeQueryCallBack callback, void* userData)
	{
		return m_hGrid.queryShape(shape, transform, callback, userData);
	}

	void World::updateProxy(const ProxyID* proxyID)
	{
		m_hGrid.updateBody(proxyID);
	}

	void World::removeContact(Contact* pContact)
	{
		m_contactManager.removeContact(pContact);
	}
}