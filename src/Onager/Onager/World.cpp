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
		m_numCpBodies(0),
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
		//printf("advance body %d to %f\n", body->getIndex(), t);
	}


	void World::step(float dt)
	{

		assert(dt != 0.0f);


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
			//DEBUG
			b->CP_POINTS.clear();


			b->calculateAABB();
			m_hGrid.updateBody(b->getProxyID());
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
		assert(numPairs <= m_numBodies*m_numBodies);


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


		//---solve continuous contacts---
		ong_START_PROFILE(CONTINUOS);
		{
			// find pairs with continous physics
			//Pair* cPairs = new Pair[numPairs];
			

			int numCPairs = 0;
			Pair* cPairs = (Pair*)malloc(sizeof(Pair) * numPairs);
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
			while(numCPairs > 0)
			{
				Pair** minPairs = new Pair*[numCPairs];
				int numMinPairs = 0;
				float minT = 1.0f;

				static float TIME_OF_IMPACT_EPSILON = 0.01f;

				// find minimum next impacts
				for (int i = 0; i < numCPairs; ++i)
				{
					float t = getTimeOfImpact(cPairs[i].A, cPairs[i].B, m_cp.data(), t0);

					if (t <= t0 || t >= 1.0f)
						continue;


					if (t < minT)
					{
						//check if any of the minpairs intersects also on new time
						if (t < minT - TIME_OF_IMPACT_EPSILON)
						{
							for (int i = 0; i < numMinPairs; ++i)
							{
								vec3 v = vec3(0, 0, 0);
								Body* a = minPairs[i]->A, *b = minPairs[i]->B;
								if (a->getContinuousPhysics())
									v += m_cp[a->getCpIndex()].p1 - m_cp[a->getCpIndex()].p0;
								if (b->getContinuousPhysics())
									v -= m_cp[b->getCpIndex()].p1 - m_cp[b->getCpIndex()].p0;

								if (!overlap(a, b, Transform(t*v, Quaternion(vec3(0, 0, 0), 1))))
								{
									minPairs[i] = minPairs[--numMinPairs];
								}
							}
						}
						minT = t;
						minPairs[numMinPairs++] = &cPairs[i];

					}
					else if (t - TIME_OF_IMPACT_EPSILON < minT)
					{
						//check if smaller t is also valid time of impact
						vec3 v = vec3(0, 0, 0);
						Body* a = cPairs[i].A, *b = cPairs[i].B;
						if (a->getContinuousPhysics())
							v += m_cp[a->getCpIndex()].p1 - m_cp[a->getCpIndex()].p0;
						if (b->getContinuousPhysics())
							v -= m_cp[b->getCpIndex()].p1 - m_cp[b->getCpIndex()].p0;

						if (overlap(a, b, Transform(t*v, Quaternion(vec3(0, 0, 0), 1))))
						{
							minPairs[numMinPairs++] = cPairs+i;
						}
					}
					else if (t == minT)
					{
						minPairs[numMinPairs++] = &cPairs[i];
					}
				}

				if (numMinPairs == 0)
					break;

				for (int i = 0; i < numMinPairs; ++i)
				{	
					//advance body to time of impact
					if (minPairs[i]->A->getContinuousPhysics() && m_cp[minPairs[i]->A->getCpIndex()].t != minT)
					{
						advanceCpBody(this, minPairs[i]->A, minT);
					}
					if (minPairs[i]->B->getContinuousPhysics() && m_cp[minPairs[i]->B->getCpIndex()].t != minT)
					{
						advanceCpBody(this, minPairs[i]->B, minT);
					}

					//update contacts
					for (int j = 0; j < 2; ++j)
					{
						ContactIter* c = ((Body**)minPairs[i])[j]->getContacts();
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
					m_contactManager.generateContact(minPairs[i]);
				}

				// solve contacts
				//todo non vector
				std::vector<Contact*> cpContacts;
				int numContacts;
				Contact** contacts = m_contactManager.getContacts(&numContacts);
				// todo get contacts from bodies
				// find contacts
				for (int i = 0; i < numContacts; ++i)
				{
					for (int j = 0; j < numMinPairs; ++j)
					{
						if (contacts[i]->colliderA->getBody() == minPairs[j]->A || contacts[i]->colliderA->getBody() == minPairs[j]->B ||
							contacts[i]->colliderB->getBody() == minPairs[j]->A || contacts[i]->colliderB->getBody() == minPairs[j]->B)
						{
							cpContacts.push_back(contacts[i]);
						}
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

				postSolveContacts(&context, cpContacts.data(), cpContacts.size(), contactConstraints);

				for (size_t i = 0; i < cpContacts.size(); ++i)
				{
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
						m_cp[bb->getCpIndex()].p1 = m_cp[bb->getCpIndex()].p0 + (1.0f - m_cp[bb->getCpIndex()].t)*dt * m_v[bb->getIndex()].v;
						bb->calculateAABB();
						m_hGrid.updateBody(cpContacts[i]->colliderB->getBody()->getProxyID());
					}
						


				}
				delete[] contactConstraints;

				
				t0 = minT;
				//broadphase
				numPairs = m_hGrid.generatePairs(pairs);
				numCPairs = 0;

				cPairs = (Pair*)realloc(cPairs, numPairs * sizeof(Pair));

				for (int i = 0; i < numPairs; ++i)
				{	
					if (pairs[i].A->getContinuousPhysics() || pairs[i].B->getContinuousPhysics())
					{
						if (pairs[i].A->getContinuousPhysics() && m_cp[pairs[i].A->getCpIndex()].t != t0)
							advanceCpBody(this, pairs[i].A, t0);
						if (pairs[i].B->getContinuousPhysics() && m_cp[pairs[i].B->getCpIndex()].t != t0)
							advanceCpBody(this, pairs[i].B, t0);
						cPairs[numCPairs++] = pairs[i];
					}
				}

				
				delete[] minPairs;
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

			free(cPairs);
		}
		ong_END_PROFILE(CONTINUOS);

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
		//DEBUG
		for (int i = 0; i < m_numCpBodies; ++i)
		{
			assert(m_b[m_cp[i].bodyIdx]->getCpIndex() == i);
		}

		if (pBody->getContinuousPhysics())
		{
			int cpIdx = pBody->getCpIndex();
			m_cp[cpIdx] = m_cp[--m_numCpBodies];
			int bodyIdx = m_cp[cpIdx].bodyIdx;
			m_b[bodyIdx]->setCpIndex(cpIdx);
			pBody->setCpIndex(m_numCpBodies);
		}

		int idx = pBody->getIndex();


		Collider* c = pBody->getCollider();
		while (c)
		{
			destroyCollider(c);
			c = c->getNext();
		}


		m_numBodies--;

		m_r[idx] = m_r[m_numBodies];
		m_v[idx] = m_v[m_numBodies];
		m_p[idx] = m_p[m_numBodies];
		m_m[idx] = m_m[m_numBodies];
		m_b[idx] = m_b[m_numBodies];


		m_b[idx]->setIndex(idx);

		if (m_b[idx]->getContinuousPhysics())
		{
			m_cp[m_b[idx]->getCpIndex()].bodyIdx = idx;
		}
		

		for (int i = 0; i < m_numCpBodies; ++i)
		{
			assert(m_cp[i].bodyIdx < m_numBodies);
		}

		m_r.pop_back();
		m_v.pop_back();
		m_p.pop_back();
		m_m.pop_back();
		m_b.pop_back();

		if (pBody->getContinuousPhysics())
			m_cp.pop_back();


		m_contactManager.removeBody(pBody);



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