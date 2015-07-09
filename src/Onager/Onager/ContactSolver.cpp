#include "ContactSolver.h"

#include "Contact.h"
#include "Collider.h"
#include "Body.h"
#include "World.h"

namespace ong
{



	void preSolveContacts(WorldContext* w, Contact** contacts, int numContacts, float invDt, ContactConstraint* contactConstraints)
	{
		for (int i = 0; i < numContacts; ++i)
		{
			Contact* c = contacts[i];

			Body* a = c->colliderA->getBody();
			Body* b = c->colliderB->getBody();

			int idxA = a->getIndex();
			int idxB = b->getIndex();

			c->friction = sqrt(c->colliderA->getMaterial()->friction * c->colliderB->getMaterial()->friction);
			c->e = ong_MAX(c->colliderA->getMaterial()->restitution, c->colliderB->getMaterial()->restitution);

			ContactManifold* man = &c->manifold;

			// calculate tangent space
			{
				vec3 v;
				if (abs(man->normal.x) <= abs(man->normal.y) && abs(man->normal.x) <= abs(man->normal.z))
					v = vec3(man->normal.x > 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f);
				else if (abs(man->normal.y) <= abs(man->normal.z))
					v = vec3(0, man->normal.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				else
					v = vec3(0, 0, man->normal.z > 0 ? 1.0f : -1.0f);

				c->biTangent = normalize(cross(v, man->normal));
				c->tangent = normalize(cross(man->normal, c->biTangent));
			}


			for (int j = 0; j < c->manifold.numPoints; ++j)
			{

				c->rA[j] = man->points[j].position - w->r[idxA].p;
				c->rB[j] = man->points[j].position - w->r[idxB].p;

				c->massN[j] = 1.0f / (w->m[idxA].invM + w->m[idxB].invM +
					dot(man->normal, cross(w->m[idxA].invI* cross(c->rA[j], man->normal), c->rA[j]) + cross(w->m[idxB].invI*cross(c->rB[j], man->normal), c->rB[j])));

				c->massT[j] = 1.0f / (w->m[idxA].invM + w->m[idxB].invM +
					dot(c->tangent, cross(w->m[idxA].invI*cross(c->rA[j], c->tangent), c->rA[j]) + cross(w->m[idxB].invI*cross(c->rB[j], c->tangent), c->rB[j])));

				c->massBT[j] = 1.0f / (w->m[idxA].invM + w->m[idxB].invM +
					dot(c->biTangent, cross(w->m[idxA].invI*cross(c->rA[j], c->biTangent), c->rA[j]) + cross(w->m[idxB].invI*cross(c->rB[j], c->biTangent), c->rB[j])));



				vec3 vA = w->v[idxA].v + cross(w->v[idxA].w, c->rA[j]);
				vec3 vB = w->v[idxB].v + cross(w->v[idxB].w, c->rB[j]);

				vec3 dv = vB - vA;

				float vn = dot(dv, man->normal);


				float penetrationBias = -0.02f * invDt * ong_MIN(0.0f, (man->points[j].penetration + 0.02f));
				penetrationBias = ong_MIN(0.3f, penetrationBias);

				float restititutionBias = 0.0f;

				// only apply restitution to velocities beyond a threshold
				if (abs(vn) > 1.0f)
					restititutionBias = -c->e * vn;

				contactConstraints[i].veloctiyBias[j] = penetrationBias + restititutionBias;

				
				//warmstarting
				if (c->accImpulseN[j] != 0.0f || c->accImpulseT[j] != 0.0f || c->accImpulseBT[j] != 0.0f)
				{
					vec3 impulse = c->accImpulseN[j] * c->manifold.normal + c->accImpulseT[j] * c->tangent + c->accImpulseBT[j] * c->biTangent;

					w->v[idxA].v -= w->m[idxA].invM * impulse;
					w->v[idxB].v += w->m[idxB].invM * impulse;

					w->v[idxA].w -= w->m[idxA].invI * cross(c->rA[j], impulse);
					w->v[idxB].w += w->m[idxB].invI * cross(c->rA[j], impulse);

				}
			}
		}
	}


	void solveContacts(WorldContext* w, Contact** contacts, int numContacts, ContactConstraint* constraints)
	{
		for (int j = 0; j < numContacts; ++j)
		{
			Contact* c = contacts[j];

			Body* a = c->colliderA->getBody();
			Body* b = c->colliderB->getBody();

			int idxA = a->getIndex();
			int idxB = b->getIndex();


			ContactManifold* man = &c->manifold;
			for (int i = 0; i < man->numPoints; ++i)
			{

				vec3 vA = w->v[idxA].v + cross(w->v[idxA].w, c->rA[i]);
				vec3 vB = w->v[idxB].v + cross(w->v[idxB].w, c->rB[i]);
 
				vec3 dv = vB - vA;
				

				float vn = dot(dv, man->normal);

				float dImpulseN = (-vn + constraints[j].veloctiyBias[i]) * c->massN[i];

				// clamp accumulated impulse
				float impulseN0 = c->accImpulseN[i];
				c->accImpulseN[i] = ong_MAX(impulseN0 + dImpulseN, 0.0f);
				dImpulseN = c->accImpulseN[i] - impulseN0;


				vec3 impulse = dImpulseN * man->normal;

				w->v[idxA].v -= w->m[idxA].invM * (impulse);
				w->v[idxB].v += w->m[idxB].invM * (impulse);
								
				w->v[idxA].w -= w->m[idxA].invI * (cross(c->rA[i], impulse));
				w->v[idxB].w += w->m[idxB].invI * (cross(c->rB[i], impulse));

				// friction
				if (c->friction <= 0.0f)
					continue;

				vA = w->v[idxA].v + cross(w->v[idxA].w, c->rA[i]);
				vB = w->v[idxB].v + cross(w->v[idxB].w, c->rB[i]);

				dv = vB - vA;

				float vt = dot(dv, c->tangent);
				float vbt = dot(dv, c->biTangent);

				float dImpulseT = -vt * c->massT[i];
				float dImpulseBT = -vbt * c->massBT[i];

				float maxImpulse = c->friction * c->accImpulseN[i];

				float impulseT0 = c->accImpulseT[i];
				c->accImpulseT[i] = ong_clamp(impulseT0 + dImpulseT, -maxImpulse, maxImpulse);
				dImpulseT = c->accImpulseT[i] - impulseT0;

				float impulseBT0 = c->accImpulseBT[i];
				c->accImpulseBT[i] = ong_clamp(impulseBT0 + dImpulseBT, -maxImpulse, maxImpulse);
				dImpulseBT = c->accImpulseBT[i] - impulseBT0;

				
				impulse = dImpulseT * c->tangent + dImpulseBT * c->biTangent;
				

				w->v[idxA].v -= w->m[idxA].invM * (impulse);
				w->v[idxB].v += w->m[idxB].invM * (impulse);
										  
				w->v[idxA].w -= w->m[idxA].invI * (cross(c->rA[i], impulse));
				w->v[idxB].w += w->m[idxB].invI * (cross(c->rB[i], impulse));

			}
		}
	}


	void postSolveContacts(WorldContext* w, Contact** contacts, int numContacts, ContactConstraint* constraints)
	{
		for (int i = 0; i < numContacts; ++i)
		{
			Contact* c = contacts[i];

			Body* a = c->colliderA->getBody();
			Body* b = c->colliderB->getBody();

			int idxA = a->getIndex();
			int idxB = b->getIndex();

			c->colliderA->callbackPostSolve(c);
			c->colliderB->callbackPostSolve(c);

			for (int j = 0; j < contacts[i]->manifold.numPoints; ++j)
			{
			

				vec3 impulse = c->accImpulseN[j] * c->manifold.normal + c->accImpulseT[j] * c->tangent + c->accImpulseBT[j] * c->biTangent;

				w->p[idxA].l -= impulse;
				w->p[idxB].l += impulse;

				w->p[idxA].a -= cross(c->rA[j], impulse);
				w->p[idxB].a += cross(c->rB[j], impulse);
			}

			w->v[idxA].v = w->m[idxA].invM * w->p[idxA].l;
			w->v[idxB].v = w->m[idxB].invM * w->p[idxB].l;

			w->v[idxA].w = w->m[idxA].invI*w->p[idxA].a;
			w->v[idxB].w = w->m[idxB].invI*w->p[idxB].a;
		}
	}
}
