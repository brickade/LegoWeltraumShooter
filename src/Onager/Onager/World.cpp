#include "World.h"
#include "Narrowphase.h"
#include "ContactSolver.h"
#include "QuickHull.h"
#include "Profiler.h"
#include "TimeOfImpact.h"


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


	void advanceCpBody(World* world, Body* body, float t)
	{
		int idx = body->getIndex();
		int cpIdx = body->getCpIndex();
		// (p1-p0) / (t1-t0) * (t2-t)
		world->m_cp[cpIdx].p0 += (t - world->m_cp[cpIdx].t) / (1.0f - world->m_cp[cpIdx].t) * (world->m_cp[cpIdx].p1 - world->m_cp[cpIdx].p0);
		world->m_cp[cpIdx].t = t;
		world->m_r[idx].p = world->m_cp[cpIdx].p0;

		//DEBUG
		body->CP_POINTS.push_back(world->m_cp[cpIdx].p0);
	}


	void World::step(float dt)
	{

		assert(dt != 0.0f);


		//integrate 
		for (int i = 0; i < m_numBodies; ++i)
		{
			mat3x3 q = toRotMat(m_r[i].q);

			m_m[i].invI = q * m_m[i].localInvI * transpose(q);

			if (m_m[i].invM != 0.0f)
				m_p[i].l += dt * 1.0f / m_m[i].invM * m_gravity;

			m_v[i].v = m_m[i].invM * m_p[i].l;
			m_v[i].w = m_m[i].invI* m_p[i].a;
		}

		//initialize continous physics
		for (int i = 0; i < m_numCpBodies; ++i)
		{
			m_cp[i].t = 0.0f;
			m_cp[i].p0 = m_r[m_cp[i].bodyIdx].p;
			m_cp[i].p1 = m_cp[i].p0 + dt * m_v[m_cp[i].bodyIdx].v;
		}

		Body* b = m_pBody;
		while (b != nullptr)
		{
			b->calculateAABB();
			m_hGrid.updateBody(b->getProxyID());
			b = b->getNext();

			//DEBUG
			b->CP_POINTS.clear();

		}

	


		// broadphase
		Pair* pairs = new Pair[m_numBodies * m_numBodies];
		int numPairs = 0;
		{
			numPairs = m_hGrid.generatePairs(pairs);
		}

		

		assert(numPairs <= m_numBodies*m_numBodies);


		//---solve normal contacts---
		{
			//narrowphase
			m_contactManager.generateContacts(pairs, numPairs, 3 * numPairs);





			//resolution	  
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

				for (int i = 0; i < numContacts; ++i)
				{
					c[i]->colliderA->callbackPostSolve(c[i]);
					c[i]->colliderB->callbackPostSolve(c[i]);
				}

				delete[] contactConstraints;

			}

			for (int i = 0; i < m_numBodies; ++i)
			{
				m_r[i].p += dt * m_v[i].v;

				vec3 wAxis = dt * m_v[i].w;
				float wScalar = sqrt(lengthSq(wAxis));

				if (wScalar != 0.0f)
					m_r[i].q = QuatFromAxisAngle(1.0f / wScalar * wAxis, wScalar) * m_r[i].q;
			}
		}

		//---solve continuous contacts---
		{


			// find pairs with continous physics
			Pair* cPairs = new Pair[numPairs];
			int numCPairs = 0;
			for (int i = 0; i < numPairs; ++i)
			{
				if (pairs[i].A->getContinuousPhysics() || pairs[i].B->getContinuousPhysics())
				{
					if (pairs[i].A->getContinuousPhysics())
					{


						advanceCpBody(this, pairs[i].A, 0.0f);

					}
					if (pairs[i].B->getContinuousPhysics())
					{

						advanceCpBody(this, pairs[i].B, 0.0f);
					}

					cPairs[numCPairs++] = pairs[i];
				}
			}
			

			float t0 = 0.0f;
			for (;numCPairs > 0;)
			{
				Pair* minPair = nullptr;
				float minT = 1.0f;
				for (int i = 0; i < numCPairs; ++i)
				{
					float t = getTimeOfImpact(cPairs[i].A, cPairs[i].B, m_cp.data(), t0);
					
					if (t != t0 && t < minT && t < 0.99f)
					{
						minT = t;
						minPair = &cPairs[i];
					}
				}

				if (minPair == nullptr)
					break;



				//advance body to time of impact
				if (minPair->A->getContinuousPhysics() && m_cp[minPair->A->getCpIndex()].t != minT)
				{
					advanceCpBody(this, minPair->A, minT);
				}
				if (minPair->B->getContinuousPhysics() && m_cp[minPair->B->getCpIndex()].t != minT)
				{
					advanceCpBody(this, minPair->B, minT);
				}

				//update contacts
				for (int i = 0; i < 2; ++i)
				{
					ContactIter* c = ((Body**)minPair)[i]->getContacts();
					while (c)
					{
						// if cp body -> advance
						if (c->other->getContinuousPhysics() && m_cp[c->other->getCpIndex()].t != minT)
						{
							advanceCpBody(this, c->other, minT);
						}

						m_contactManager.updateContact(c->contact);
						c = c->next;
					}

				}
				m_contactManager.generateContact(minPair);

				// solve contacts
				//todo non vector
				std::vector<Contact*> cpContacts;
				int numContacts;
				Contact** contacts = m_contactManager.getContacts(&numContacts);
				// todo get contacts from bodies
				// find contacts
				for (int i = 0; i < numContacts; ++i)
				{
					if (contacts[i]->colliderA->getBody() == minPair->A || contacts[i]->colliderA->getBody() == minPair->B ||
						contacts[i]->colliderB->getBody() == minPair->A || contacts[i]->colliderB->getBody() == minPair->B)
					{
						cpContacts.push_back(contacts[i]);
					}
				}

				WorldContext context;
				context.r = m_r.data();
				context.v = m_v.data();
				context.p = m_p.data();
				context.m = m_m.data();

				ContactConstraint* contactConstraints = new ContactConstraint[cpContacts.size()];
				preSolveContacts(&context, cpContacts.data(), cpContacts.size(), 1.0f / (minT - t0), contactConstraints);

				for (int i = 0; i < 8; ++i)
				{
					solveContacts(&context, cpContacts.data(), cpContacts.size(), contactConstraints);
				}

				for (int i = 0; i < cpContacts.size(); ++i)
				{
					// collsion callbacks
					cpContacts[i]->colliderA->callbackPostSolve(cpContacts[i]);
					cpContacts[i]->colliderB->callbackPostSolve(cpContacts[i]);

					// todo check on bodies not on colliders
					// update spatial partitioning
					Body* ba = cpContacts[i]->colliderA->getBody();
					Body* bb = cpContacts[i]->colliderB->getBody();

					// update cp
					if (ba->getContinuousPhysics())
					{
						m_cp[ba->getCpIndex()].p1 = m_cp[ba->getCpIndex()].p0 + (1.0f - m_cp[ba->getCpIndex()].t)*dt * m_v[ba->getIndex()].v;
						ba->calculateAABB();
						m_hGrid.updateBody(cpContacts[i]->colliderA->getBody()->getProxyID());
					}
					if (bb->getContinuousPhysics())
					{
						m_cp[bb->getCpIndex()].p1 = m_cp[ba->getCpIndex()].p0 + (1.0f - m_cp[ba->getCpIndex()].t)*dt * m_v[bb->getIndex()].v;
						bb->calculateAABB();
						m_hGrid.updateBody(cpContacts[i]->colliderB->getBody()->getProxyID());
					}
						


				}

				delete[] contactConstraints;
				
				t0 = minT;
				//broadphase
				numPairs = m_hGrid.generatePairs(pairs);
				numCPairs = 0;
				for (int i = 0; i < numPairs; ++i)
				{	
					if (pairs[i].A->getContinuousPhysics() && m_cp[pairs[i].A->getCpIndex()].t != t0)
						advanceCpBody(this, pairs[i].A, t0);
					if (pairs[i].B->getContinuousPhysics() && m_cp[pairs[i].B->getCpIndex()].t != t0)
						advanceCpBody(this, pairs[i].B, t0);

					cPairs[numCPairs++] = pairs[i];
				}


			}

			if (t0 != 1.0f)
			{
				for (int i = 0; i < numCPairs; ++i)
				{
					for (int j = 0; j < 2; ++j)
					{
						Body* b = ((Body**)cPairs + i)[j];
						if (b->getContinuousPhysics() && m_cp[b->getCpIndex()].t != 1.0f)
							advanceCpBody(this, b, 1.0f);
					}
				}

			}

		}

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

		if (description.continuousPhysics)
		{
			m_cp.push_back(ContinuousState());
			m_cp[m_numCpBodies].bodyIdx = idx;
			body->setCpIndex(m_numCpBodies++);
		}

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

		if (pBody->getContinuousPhysics())
		{
			int idx = pBody->getCpIndex();
			m_cp[idx] = m_cp[--m_numCpBodies];
			m_b[m_cp[idx].bodyIdx]->setCpIndex(idx);
			m_cp.pop_back();
		}


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

}