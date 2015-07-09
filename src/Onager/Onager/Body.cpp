#include "Body.h"
#include "World.h"
#include "Narrowphase.h"
#include "BVH.h"

namespace ong
{


	Body::Body(const BodyDescription& descr, World* pWorld, int idx)
		: m_pWorld(pWorld),
		m_index(idx),
		m_flags(0),
		m_pCollider(nullptr),
		m_cm(0.0f, 0.0f, 0.0f),
		m_numContacts(0),
		m_pContacts(nullptr),
		m_tree(nullptr),
		m_numCollider(0)
	{

		m_aabb = {vec3(0, 0, 0), vec3(0,0,0)};

		m_flags |= descr.type;

		m_pWorld->m_r[m_index].p = descr.transform.p;
		m_pWorld->m_r[m_index].q = descr.transform.q;
		m_pWorld->m_p[m_index].l = descr.linearMomentum;
		m_pWorld->m_p[m_index].a = descr.angularMomentum;
	}


	void Body::addCollider(Collider* pCollider)
	{

		assert(pCollider->getBody() == NULL);

		pCollider->setBody(this);

		m_numCollider++;

		pCollider->setNext(m_pCollider);
		pCollider->setPrev(nullptr);

		if (m_pCollider)
			m_pCollider->setPrev(pCollider);

		m_pCollider = pCollider;

		calculateMassData();

		if (m_numCollider >= 2)
		{
			calculateTree();
		}
		calculateAABB();
		m_pWorld->updateProxy(m_proxyID);
	}

	void Body::removeCollider(Collider* collider)
	{
		assert(collider->getBody() == this);

		m_numCollider--;

		if (collider->getNext())
			collider->getNext()->setPrev(collider->getPrev());

		if (collider->getPrev())
			collider->getPrev()->setNext(collider->getNext());

		if (m_pCollider == collider)
			m_pCollider = collider->getNext();
		
		// remove contacts
		ContactIter* iter = m_pContacts;
		while (iter)
		{
			if (iter->contact->colliderA == collider || iter->contact->colliderB == collider)
			{
				m_pWorld->removeContact(iter->contact);
			}
			iter = iter->next;
		}

		collider->setBody(nullptr);

		if (m_numCollider != 0)
			calculateMassData();

		if (m_numCollider >= 2)
		{
			calculateTree();
		}
		calculateAABB();
		m_pWorld->updateProxy(m_proxyID);

	}



	void Body::calculateMassData()
	{
		float m = 0.0f;
		mat3x3 I = mat3x3(
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f));

		vec3 cm = vec3(0.0f, 0.0f, 0.0f);


		//if (m_flags & STATIC)
		//{

		//	vec3 oldPos = getPosition();
		//	m_pWorld->m_r[m_index].p = oldPos + cm;
		//	m_pWorld->m_m[m_index].localInvI = I;
		//	m_pWorld->m_m[m_index].invM = m;

		//	m_cm = cm;

		//	return;
		//}

		Collider* c = m_pCollider;
		for (; c != nullptr; c = c->getNext())
		{
			//if (c->isSensor())
			//	continue;

			const MassData& data = c->getMassData();
			Transform& t = c->getTransform();

			vec3 _cm = transformVec3(data.cm, t);

			m += data.m;

			cm += data.m * _cm;

			mat3x3 rot = toRotMat(t.q);
			mat3x3 _I = rot * data.I * transpose(rot);

			// center inertia to bodies frame of  reference
			_I = _I + data.m * (dot(_cm, _cm) * identity() - outerproduct(_cm, _cm));

			I = I + _I;
		}

		if (m != 0.0f)
			cm = 1.0f / m* cm;

		// center inertia to center of mass
		I = I - m * (dot(cm, cm) * identity() - outerproduct(cm, cm));
	  



		vec3 oldPos = getPosition();
		m_pWorld->m_r[m_index].p = oldPos + rotate(cm, getOrientation());
		if (m_flags & STATIC)
		{
			memset(&m_pWorld->m_m[m_index], 0, sizeof(MassState));
		}
		else
		{
			m_pWorld->m_m[m_index].invM = 1.0f / m;
			m_pWorld->m_m[m_index].localInvI = inverse(I);
		}

		m_cm = cm;
	}




	void Body::calculateAABB()
	{
		if (m_numCollider > 1)
			m_aabb = transformAABB(&m_tree->aabb, &getTransform());
		else if (m_numCollider == 1)
			m_aabb = transformAABB(&m_pCollider->getAABB(), &getTransform());
		else
			m_aabb = { getTransform().p, vec3(0, 0, 0) };
	}

	void Body::calculateTree()
	{	
		if (m_numCollider <= 1)
			return;

		if (m_tree)
			delete[] m_tree;
		m_tree = new BVTree[m_numCollider + m_numCollider - 1];

		constructBVTree(m_pCollider, m_numCollider, m_tree);
	}



	//void intersectTree(BVTree* tree, BVTree* n, const vec3& o, const vec3& d, float tmax, float tmin, Collider** c)
	//{
	//	float t;
	//	vec3 p;
	//	if (!intersectRayAABB(o, d, n->aabb, t, p) || t > tmax)
	//		return;
	//
	//	if (n->type == NodeType::LEAF)
	//	{
	//		Collider* _c = n->collider;
	//		switch (_c->getShapeType())
	//		{
	//		case ShapeType::HULL:
	//			const Transform& _t = _c->getTransform();
	//			vec3 _o = invTransformVec3(o, _t);
	//			vec3 _d = rotate(d, conjugate(_t.q));
	//			
	//			if (intersectRayHull(_o, _d, _c->getHull(), t, p) && t < tmax && t < tmin)
	//			{
	//				tmin = t;
	//				*c = _c;
	//			}
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		intersectTree(tree, tree + n->left, o, d, tmax, tmin, c);
	//		intersectTree(tree, tree + n->right, o, d, tmax, tmin, c);
	//	}
	//
	//}

	void intersectTree(BVTree* tree, BVTree* n, const vec3& o, const vec3& d, float tmax, float& tmin, RayQueryResult* hit)
	{

		std::stack<BVTree*> s;
		s.push(n);

		while (true)
		{
			float t;
			vec3 p;
			if (!intersectRayAABB(o, d, n->aabb, t, p) || t > tmax)
			{
				if (s.empty())
					break;
				n = s.top();
				s.pop();
			}


			if (n->type == NodeType::LEAF)
			{
				Collider* _c = n->collider;
				vec3 n;
				const Transform& _t = _c->getTransform();
				vec3 _o = invTransformVec3(o, _t);
				vec3 _d = rotate(d, conjugate(_t.q));

				switch (_c->getShape().getType())
				{
				case ShapeType::HULL:

					if (intersectRayHull(_o, _d, _c->getShape(), t, p, n) && t < tmax && t < tmin)
					{
						tmin = t;
						hit->t = tmin;
						hit->normal = rotate(n, _t.q);
						hit->collider = _c;
					}
					break;
				case ShapeType::SPHERE:
					if (intersectRaySphere(_o, _d, _c->getShape(), t, p, n) && t < tmax && t < tmin)
					{
						tmin = t;
						hit->t = tmin;
						hit->normal = rotate(n, _t.q);
						hit->collider = _c;
					}
					break;
				case ShapeType::CAPSULE:
					if (intersectRayCapsule(_o, _d, _c->getShape(), t, p, n) && t < tmax && t < tmin)
					{
						tmin = t;
						hit->t = tmin;
						hit->normal = rotate(n, _t.q);
						hit->collider = _c;
					}
					break;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}


			if (s.empty())
				break;

			n = s.top();
			s.pop();
		}

	}





	bool Body::queryRay(const vec3& origin, const vec3& dir, RayQueryResult* result,  float tmax)
	{
		Transform t = getTransform();

		vec3 o = invTransformVec3(origin, t);
		vec3 d = rotate(dir, conjugate(t.q));


		float tmin = FLT_MAX; 

		if (m_numCollider > 1)
		{
			intersectTree(m_tree, m_tree, o, d, tmax, tmin, result);
		}
		else if (m_numCollider == 1)
		{
			vec3 p;
			if (intersectRayAABB(o, d, m_pCollider->getAABB(), tmin, p) && tmin < tmax)
			{
				vec3 n;

				const Transform& _t = m_pCollider->getTransform();
				vec3 _o = invTransformVec3(o, _t);
				vec3 _d = rotate(d, conjugate(_t.q));
				switch (m_pCollider->getShape().getType())
				{
				case ShapeType::HULL:
					if (intersectRayHull(_o, _d, m_pCollider->getShape(), tmin, p, n) && tmin < tmax)
					{
						result->collider = m_pCollider;
						result->normal = rotate(n, _t.q);
					}
					break;
				case ShapeType::SPHERE:
					if (intersectRaySphere(_o, _d, m_pCollider->getShape(), tmin, p, n) && tmin < tmax)
					{
						result->collider = m_pCollider;
						result->normal = rotate(n, _t.q);
					}
					break;
				case ShapeType::CAPSULE:
					if (intersectRayCapsule(_o, _d, m_pCollider->getShape(), tmin, p, n) && tmin < tmax)
					{
						result->collider = m_pCollider;
						result->normal = rotate(n, _t.q);
					}
					break;

				}
			}
		}

		result->t = tmin;
		result->point = origin + tmin*dir;
		result->normal = normalize(rotate(result->normal, t.q));

		return (result->collider != nullptr);
	}

	bool overlap(const Collider* a, const Collider* b)
	{
		Transform t2;
		Transform t1;

		if (a->getBody())
			t1 = transformTransform(a->getTransform(), a->getBody()->getTransform());
		else
			t1 = a->getTransform();

		if (b->getBody())
			t2 = transformTransform(b->getTransform(), b->getBody()->getTransform());
		else
			t2 = b->getTransform();

		return overlap(a->getShape(), b->getShape(), t1, t2);
	}

	bool overlapTree(BVTree* tree, BVTree* n, const Collider* collider, const vec3& t, const mat3x3& rot)
	{
		std::stack<BVTree*> s;

		while (true)
		{
			if (!overlap(n->aabb, collider->getAABB(), t, rot))
			{
				if (s.empty())
					return false;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider, collider))
					return true;
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return false;
			n = s.top();
			s.pop();
		}
	}

	bool overlapTree(BVTree* tree, BVTree* n, Collider* collider, const vec3& t, const mat3x3& rot, ColliderQueryCallBack callback)
	{
		std::stack<BVTree*> s;
		while (true)
		{
			if (!overlap(n->aabb, collider->getAABB(), t, rot))
			{
				if (s.empty())
					return true;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider, collider))
				{
					bool stop = !callback(collider, n->collider);
					if (stop)
						return false;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return true;
			n = s.top();
			s.pop();
		}
	}

	bool overlapTree(BVTree* tree, BVTree* n, ShapePtr shape, const Transform& transform, const vec3& t, const mat3x3& rot)
	{
		std::stack<BVTree*> s;

		AABB shapeAABB = calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) });

		while (true)
		{
			if (!overlap(n->aabb, shapeAABB, t, rot))
			{
				if (s.empty())
					return false;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider->getShape(), shape, transformTransform(n->collider->getTransform(), n->collider->getBody()->getTransform()), transform))
				{
					return true;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return false;
			n = s.top();
			s.pop();
		}
	}

	bool overlapTree(BVTree* tree, BVTree* n, ShapePtr shape,const Transform& transform, const vec3& t, const mat3x3& rot, ShapeQueryCallBack callback, void* userData)
	{
		std::stack<BVTree*> s;
		
		AABB shapeAABB = calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) });

		while (true)
		{
			if (!overlap(n->aabb, shapeAABB, t, rot))
			{
				if (s.empty())
					return true;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider->getShape(), shape,transformTransform(n->collider->getTransform(), n->collider->getBody()->getTransform()), transform))
				{
					bool stop = !callback(n->collider, userData);
					if (stop)
						return false;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return true;
			n = s.top();
			s.pop();
		}
	}

	bool Body::queryCollider(const Collider* collider)
	{
		Transform tCollider = Transform(vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1));

		if (collider->getBody())
		{
			tCollider = collider->getBody()->getTransform();
		}

		Transform t = invTransformTransform(tCollider, getTransform());
		mat3x3 rot = toRotMat(t.q);

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, m_tree, collider, t.p, rot);
		}
		else if (m_numCollider == 1)
		{
			if (overlap(m_pCollider->getAABB(), collider->getAABB(), t.p, rot))
			{
				return overlap(m_pCollider, collider);
			}
		}

		return false;
	}

	bool Body::queryCollider(Collider* collider, ColliderQueryCallBack callback)
	{
		Transform tCollider = Transform(vec3(0,0,0), Quaternion(vec3(0,0,0), 1));

		if (collider->getBody())
		{
			tCollider = collider->getBody()->getTransform();
		}

		Transform t = invTransformTransform(tCollider, getTransform());
		mat3x3 rot = toRotMat(t.q);

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, m_tree, collider, t.p, rot);
		}
		else if (m_numCollider == 1)
		{
			if (overlap(m_pCollider->getAABB(), collider->getAABB(), t.p, rot))
			{
				if (overlap(m_pCollider, collider))
				{
					return callback(collider, m_pCollider);;
				}
			}
		}

		return true;
	}

	bool Body::queryShape(ShapePtr shape, const Transform& transform)
	{
		Transform t = invTransformTransform(transform, getTransform());
		mat3x3 rot = toRotMat(t.q);

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, m_tree, shape, transform, t.p, rot);
		}
		else if (m_numCollider == 1)
		{
			if (overlap(m_pCollider->getAABB(), ong::calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) }), t.p, rot))
			{
				return overlap(m_pCollider->getShape(), shape, transformTransform(m_pCollider->getTransform(), getTransform()), transform);
			}
		}

		return false;
	}


	bool Body::queryShape(ShapePtr shape, const Transform& transform, ShapeQueryCallBack callback, void* userData)
	{
		Transform t = invTransformTransform(transform, getTransform());
		mat3x3 rot = toRotMat(t.q);

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, m_tree, shape, transform, t.p, rot, callback, userData);
		}
		else if (m_numCollider == 1)
		{
			if (overlap(m_pCollider->getAABB(), ong::calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) }), t.p, rot))
			{
				if ((overlap(m_pCollider->getShape(), shape, transformTransform(m_pCollider->getTransform(), getTransform()), transform)))
				{
					return callback(m_pCollider, userData);
				}
			}
		}

		return true;
	}


	const AABB& Body::getAABB()
	{
		return m_aabb;
	}

	Transform Body::getTransform() const
	{
		return Transform(getPosition(), getOrientation());
	}

	const vec3& Body::getWorldCenter() const
	{
		return m_pWorld->m_r[m_index].p;
	}


	vec3 Body::getPosition() const
	{
		return m_pWorld->m_r[m_index].p - rotate(m_cm, getOrientation());
		//return m_pWorld->getPosition(m_index) - m_cm;
	}

	const Quaternion& Body::getOrientation() const
	{
		return m_pWorld->m_r[m_index].q;
		//return m_pWorld->getOrientation(m_index);
	}


	vec3 Body::getLinearMomentum()
	{
		return m_pWorld->m_p[m_index].l;
	}

	vec3 Body::getRelativeLinearMomentum()
	{
		return rotate(getLinearMomentum(), conjugate(m_pWorld->m_r[m_index].q));
	}

	vec3 Body::getAngularMomentum()
	{
		return m_pWorld->m_p[m_index].a;
	}


	vec3 Body::getRelativeAngularMomentum()
	{
		return rotate(getAngularMomentum(), conjugate(m_pWorld->m_r[m_index].q));
	}


	vec3 Body::getLinearVelocity()
	{
		return m_pWorld->m_v[m_index].v;
	}


	vec3 Body::getRelativeLinearVelocity()
	{
		return rotate(getLinearVelocity(), conjugate(m_pWorld->m_r[m_index].q));
	}


	vec3 Body::getAngularVelocity()
	{
		mat3x3 q = toRotMat(m_pWorld->m_r[m_index].q);
		mat3x3 invI = q * m_pWorld->m_m[m_index].localInvI * transpose(q);

		/*return m_pWorld->m_p[m_index].a * invI;*/
		return invI * m_pWorld->m_p[m_index].a;
	}

	vec3 Body::getRelativeAngularVelocity()
	{
		return rotate(getAngularVelocity(), conjugate(m_pWorld->m_r[m_index].q));
	}


	float Body::getInverseMass()
	{
		return m_pWorld->m_m[m_index].invM;
		//return m_pWorld->getInvMass(m_index);
	}

	const mat3x3& Body::getInverseInertia()
	{
		return m_pWorld->m_m[m_index].localInvI;
		//return m_pWorld->getLocalInvInertia(m_index);
	}



	void Body::addContact(ContactIter* contact)
	{
		contact->next = m_pContacts;
		contact->prev = nullptr;

		if (m_pContacts)
			m_pContacts->prev = contact;

		m_pContacts = contact;

		m_numContacts++;
	}


	ContactIter* Body::removeContact(Contact* contact)
	{
		for (ContactIter* c = m_pContacts; c != 0; c = c->next)
		{
			if (c->contact == contact)
			{
				if (c->prev)
					c->prev->next = c->next;
				if (c->next)
					c->next->prev = c->prev;

				if (m_pContacts == c)
					m_pContacts = c->next;
				m_numContacts--;

				return c;
			}
		}

		assert(true);
		return 0;
	}


	void Body::setPosition(const vec3& position)
	{
		m_pWorld->m_r[m_index].p = position + rotate(m_cm, getOrientation());
		//m_pWorld->setPosition(m_index, position + m_cm);

		m_pWorld->updateProxy(m_proxyID);
	}



	void Body::applyImpulse(const vec3& impulse, const vec3& point)
	{
		m_pWorld->m_p[m_index].l += impulse;
		m_pWorld->m_p[m_index].a += cross(point - m_pWorld->m_r[m_index].p, impulse);
	}

	void Body::applyImpulse(const vec3& impulse)
	{
		m_pWorld->m_p[m_index].l += impulse;

	}

	void Body::applyRelativeImpulse(const vec3& impulse)
	{
		applyImpulse(rotate(impulse, getOrientation()));
	}

	void Body::applyRelativeImpulse(const vec3& impulse, const vec3& point)
	{
		vec3 world = transformVec3(point, getTransform());
		applyImpulse(rotate(impulse, getOrientation()), world);
	}

	void Body::applyAngularImpulse(const vec3& impulse)
	{
		m_pWorld->m_p[m_index].a += impulse;
	}

	void Body::applyRelativeAngularImpulse(const vec3& impulse)
	{
		m_pWorld->m_p[m_index].a += rotate(impulse, getOrientation());
	}

	void Body::applyForce(const vec3& force, float t)
	{
		applyImpulse(t * force);
	}

	void Body::applyForce(const vec3& force, const vec3& point, float t)
	{
		applyImpulse(t * force, point);
	}


	void Body::applyRelativeForce(const vec3& force, float t)
	{
		applyRelativeImpulse(t * force);
	}

	void Body::applyRelativeForce(const vec3& force, const vec3& point, float t)
	{
		applyRelativeImpulse(t * force, point);
	}

	void Body::applyTorque(const vec3& torque, float t)
	{
		applyAngularImpulse(t * torque);
	}


	void Body::applyRelativeTorque(const vec3& torque, float t)
	{
		applyRelativeAngularImpulse(t * torque);
	}


	void Body::setLinearMomentum(const vec3& momentum)
	{
		m_pWorld->m_p[m_index].l = momentum;
	}

	void Body::setAngularMomentum(const vec3& momentum)
	{
		m_pWorld->m_p[m_index].a = momentum;
	}


}