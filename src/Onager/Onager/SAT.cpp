#include "SAT.h"
#include "contact.h"
#include "Shapes.h"
#include <float.h>
#include <vector>


namespace ong
{



	void createFaceContact(FaceQuery* faceQuery, const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, float dir, ContactManifold* manifold)
	{
		// find incident face


		Plane referencePlane = transformPlane(hull1->pPlanes[faceQuery->index], *t1);

		//set manifold normal
		manifold->normal = dir * referencePlane.n;

		int minIndex = -1;
		float min = FLT_MAX;
		for (int i = 0; i < hull2->numFaces; ++i)
		{
			vec3 n = rotate(hull2->pPlanes[i].n, t2->q);

			float d = dot(referencePlane.n, n);
			if (d < min)
				minIndex = i, min = d;
		}

		Face* f1 = hull1->pFaces + faceQuery->index;
		Face* f2 = hull2->pFaces + minIndex;


		std::vector<ContactPoint> polA;
		std::vector<ContactPoint> polB;

		std::vector<ContactPoint>* in = &polA;
		std::vector<ContactPoint>* out = &polB;

		//init clipping polygon

		HalfEdge* e20 = hull2->pEdges + f2->edge;
		HalfEdge* e2 = e20;
		do
		{
			vec3 A = transformVec3(hull2->pVertices[e2->tail], *t2);

			ContactPoint P;
			P.position = A;
			P.penetration = 0.0f;

			in->push_back(P);

			e2 = hull2->pEdges + e2->next;
		} while (e2 != e20);





		// clip incident face agains side planes of reference face
		HalfEdge* e10 = hull1->pEdges + f1->edge;
		HalfEdge* e1 = e10;
		do
		{
			Plane sidePlane;

			// todo smarter transforms

			vec3 A = transformVec3(hull1->pVertices[e1->tail], *t1);
			vec3 B = transformVec3(hull1->pVertices[(hull1->pEdges + e1->twin)->tail], *t1);

			sidePlane.n = normalize(cross(B - A, referencePlane.n));
			sidePlane.d = dot(A, sidePlane.n);


			//todo assert(in->size() != 0)
			if (in->size() == 0)
				break;

			ContactPoint* C = &(*in).back();
			for (ContactPoint& D : *in)
			{
				float distC = distPointFatPlane(C->position, sidePlane, hull1->epsilon);
				float distD = distPointFatPlane(D.position, sidePlane, hull1->epsilon);


				if (distC * distD < 0.0f)
				{
					vec3 I;
					float t;
					intersectSegmentPlane(C->position, D.position, sidePlane, t, I);

					ContactPoint P;
					P.position = I;
					P.penetration = 0.0f;

					out->push_back(P);

				}

				if (distD <= 0.0f || distC == 0.0f)
				{
					out->push_back(D);
				}

				C = &D;
			}
			in->clear();
			std::swap(in, out);

			e1 = hull1->pEdges + e1->next;
		} while (e1 != e10);

		for (ContactPoint& A : *in)
		{
			float d = -distPointFatPlane(A.position, referencePlane, hull1->epsilon);
			if (d >= 0.0f)
			{
				vec3 B = closestPtPointPlane(A.position, referencePlane);
				ContactPoint P;
				P.position = B;
				P.penetration = -sqrt(lengthSq(B - A.position));

				out->push_back(P);
			}


		}


		if (out->size() > 4)
		{
			optimizeContactPoints(*out, manifold);
		}
		else
		{
			memcpy(manifold->points, out->data(), sizeof(ContactPoint)*out->size());
			manifold->numPoints = out->size();
		}


	}

	void createEdgeContact(EdgeQuery* edgeQuery, const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, ContactManifold* manifold)
	{
		HalfEdge* e1 = hull1->pEdges + edgeQuery->index1;
		HalfEdge* et1 = hull1->pEdges + e1->twin;

		HalfEdge* e2 = hull2->pEdges + edgeQuery->index2;
		HalfEdge* et2 = hull2->pEdges + e2->twin;

		vec3 A = transformVec3(hull1->pVertices[e1->tail], *t1);
		vec3 B = transformVec3(hull1->pVertices[et1->tail], *t1);

		vec3 C = transformVec3(hull2->pVertices[e2->tail], *t2);
		vec3 D = transformVec3(hull2->pVertices[et2->tail], *t2);


		vec3 P, Q;
		float s, t;
		closestPtSegmentSegment(A, B, C, D, s, t, P, Q);

		manifold->numPoints = 1;
		manifold->points[0].penetration = -sqrt(lengthSq(Q - P));
		manifold->points[0].position = 0.5f * (P + Q);
		manifold->normal = normalize(cross(B - A, D - C));

		if (dot(manifold->normal, A - transformVec3(hull1->centroid, *t1)) < 0.0f)
		{
			manifold->normal = -manifold->normal;
		}



	}



	void SAT(const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, ContactManifold* manifold, Feature* feature)
	{
		manifold->numPoints = 0;

		FaceQuery faceQueryA;
		queryFaceDirections(hull1, t1, hull2, t2, &faceQueryA);

		if (faceQueryA.separation > 0.0f)
			return;

		FaceQuery faceQueryB;
		queryFaceDirections(hull2, t2, hull1, t1, &faceQueryB);

		if (faceQueryB.separation > 0.0f)
			return;

		EdgeQuery edgeQuery;
		queryEdgeDirections(hull1, t1, hull2, t2, &edgeQuery);

		if (edgeQuery.separation > 0.0f)
			return;

		//dunno about this
		edgeQuery.separation -= 0.00001f;

		if (faceQueryA.separation >= faceQueryB.separation && faceQueryA.separation > edgeQuery.separation)
		{
			createFaceContact(&faceQueryA, hull1, t1, hull2, t2, 1.0f, manifold);

			if (feature)
			{
				feature->type = Feature::HULL_FACE;
				feature->hullFace.face1 = faceQueryA.index;
				feature->hullFace.face2 = -1;
			}

		}
		else if (faceQueryB.separation >= edgeQuery.separation)
		{
			createFaceContact(&faceQueryB, hull2, t2, hull1, t1, -1.0f, manifold);

			if (feature)
			{
				feature->type = Feature::HULL_FACE;
				feature->hullFace.face2 = faceQueryB.index;
				feature->hullFace.face1 = -1;
			}
		}
		else
		{
			createEdgeContact(&edgeQuery, hull1, t1, hull2, t2, manifold);

			if (feature)
			{
				feature->type = Feature::HULL_EDGE;
				feature->hullEdge.edge1 = edgeQuery.index1;
				feature->hullEdge.edge1 = edgeQuery.index2;
			}
		}
	}

	float project(const Plane& p, const Hull* hull)
	{
		vec3 support = getHullSupport(-p.n, hull);
		return distPointPlane(support, p);
	}

	void queryFaceDirections(const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, FaceQuery* out)
	{
		Transform t = invTransformTransform(*t1, *t2);

		int maxIndex = -1;
		float maxSeparation = -FLT_MAX;


		for (int i = 0; i < hull1->numFaces; ++i)
		{
			Plane plane = transformPlane(hull1->pPlanes[i], t);

			float separation = project(plane, hull2);

			if (separation > maxSeparation)
				maxIndex = i, maxSeparation = separation;
		}

		out->index = maxIndex;
		out->separation = maxSeparation;

	}


	bool isMinkowskiSum(const vec3& A, const vec3& B, const vec3& BxA, const vec3& C, const vec3& D, const vec3& DxC)
	{
		//test if arcs AB and CD intersect on unit sphere

		float CBA = dot(C, BxA);
		float DBA = dot(D, BxA);
		float ADC = dot(A, DxC);
		float BDC = dot(B, DxC);

		return CBA*DBA < 0.0f && ADC*BDC<0.0f && CBA*BDC>0.0f;

	}

	float project(const vec3& P1, const vec3& E1, const vec3& P2, const vec3& E2, const vec3& C1)
	{
		// build plane through edge1 and check distance to edge2

		vec3 E1xE2 = cross(E1, E2);

		// skip near parallel edges
		const float kTolerance = 0.005f;
		float L = sqrt(dot(E1xE2, E1xE2));
		if (L < kTolerance * sqrt(dot(E1, E1) * dot(E2, E2)))
			return -FLT_MAX;

		vec3 n = 1.0f / L * E1xE2;

		// assure consistent normal orientation
		if (dot(n, P1 - C1) < 0.0f)
		{
			n = -n;
		}

		// s = Dot(n, p2) - d = Dot(n, p2) - Dot(n, p1) = Dot(n, p2 - p1) 
		return dot(n, P2 - P1);
	}

	void queryEdgeDirections(const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, EdgeQuery* out)
	{
		Transform t = invTransformTransform(*t1, *t2);

		vec3 C1 = transformVec3(hull1->centroid, t);

		int maxIndex1 = -1, maxIndex2 = -1;
		float maxSeparation = -FLT_MAX;

		for (int i1 = 0; i1 < hull1->numEdges; i1 += 2)
		{
			HalfEdge* e1 = hull1->pEdges + i1;
			HalfEdge* t1 = hull1->pEdges + e1->twin;

			vec3 P1 = transformVec3(hull1->pVertices[e1->tail], t);
			vec3 Q1 = transformVec3(hull1->pVertices[t1->tail], t);
			vec3 E1 = Q1 - P1;

			vec3 U1 = rotate(hull1->pPlanes[e1->face].n, t.q);
			vec3 V1 = rotate(hull1->pPlanes[t1->face].n, t.q);

			for (int i2 = 0; i2 < hull2->numEdges; i2 += 2)
			{
				HalfEdge* e2 = hull2->pEdges + i2;
				HalfEdge* t2 = hull2->pEdges + e2->twin;

				vec3 P2 = hull2->pVertices[e2->tail];
				vec3 Q2 = hull2->pVertices[t2->tail];
				vec3 E2 = Q2 - P2;

				vec3 U2 = hull2->pPlanes[e2->face].n;
				vec3 V2 = hull2->pPlanes[t2->face].n;

				// negate h2 to account for minkowski difference
				if (isMinkowskiSum(U1, V1, -E1, -U2, -V2, -E2))
				{
					float separation = project(P1, E1, P2, E2, C1);
					if (separation > maxSeparation)
						maxIndex1 = i1, maxIndex2 = i2, maxSeparation = separation;

				}

			}
		}

		out->index1 = maxIndex1;
		out->index2 = maxIndex2;
		out->separation = maxSeparation;

	}

}