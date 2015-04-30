#include "QuickHull.h"
#include "Shapes.h"
#include "Allocator.h"

#include <float.h>
#include <vector>
#include <stack>
#include <map>


namespace ong
{



	struct qhVertex;
	struct qhHalfEdge;
	struct qhFace;


	typedef std::pair<qhVertex*, qhVertex*> VertexPair;
	typedef std::map<VertexPair, qhHalfEdge*> EdgeMap;

	struct qhVertex
	{
		qhVertex* prev = nullptr;
		qhVertex* next = nullptr;

		qhHalfEdge* edge = nullptr;

		vec3 position;

		float dist;
	};


	struct qhHalfEdge
	{
		qhVertex* tail = nullptr;

		qhHalfEdge* prev = nullptr;
		qhHalfEdge* next = nullptr;
		qhHalfEdge* twin = nullptr;

		qhFace* face = nullptr;
	};

	struct qhFace
	{
		qhFace* prev = nullptr;
		qhFace* next = nullptr;

		qhHalfEdge* edge = nullptr;

		qhVertex* contactList = nullptr;

		Plane plane;
		vec3 center;
		bool visited;

	};

	typedef Allocator<qhVertex> VertexAllocator;
	typedef Allocator<qhHalfEdge> EdgeAllocator;
	typedef Allocator<qhFace> FaceAllocator;

	struct qhHull
	{
		qhVertex* vertices = nullptr;
		qhFace* faces = nullptr;

		EdgeMap edges;

		int numVertices = 0;
		int numHalfEdges = 0;
		int numFaces = 0;

		float epsilon;

		VertexAllocator* vAlloc;
		EdgeAllocator* eAlloc;
		FaceAllocator* fAlloc;
	};





	qhFace* addFace(qhHull* hull, qhVertex** v)
	{
		qhHalfEdge* h0 = hull->eAlloc->sNew(qhHalfEdge());
		qhHalfEdge* h1 = hull->eAlloc->sNew(qhHalfEdge());
		qhHalfEdge* h2 = hull->eAlloc->sNew(qhHalfEdge());


		qhFace* f = hull->fAlloc->sNew(qhFace());

		hull->numFaces++;

		f->edge = h0;
		f->visited = false;
		f->plane = planeFromABC(v[0]->position, v[1]->position, v[2]->position);
		f->center = 1.0f / 3.0f * (v[0]->position + v[1]->position + v[2]->position);

		f->next = hull->faces;

		if (hull->faces != nullptr)
			hull->faces->prev = f;

		hull->faces = f;


		f->prev = nullptr;


		qhHalfEdge* h[3] = { h0, h1, h2 };
		for (int i = 0; i < 3; ++i)
		{
			int next = ((i + 1) % 3);
			int prev = ((i - 1) % 3);
			prev = (prev < 0 ? 3 + prev : prev);


			h[i]->face = f;
			h[i]->tail = v[i];

			h[i]->twin = nullptr;
			h[i]->next = h[next];
			h[i]->prev = h[prev];
			h[i]->face = f;

			if (hull->edges.count(VertexPair(v[next], v[i])) > 0)
			{
				qhHalfEdge* ht = hull->edges[VertexPair(v[next], v[i])];
				h[i]->twin = ht;
				ht->twin = h[i];
			}

			hull->edges[VertexPair(v[i], v[next])] = h[i];


			v[i]->edge = h[i];
		}

		return f;
	}

	qhVertex* addVertex(const vec3& point, qhHull* hull)
	{
		qhVertex* v = (*hull->vAlloc)();

		v->position = point;

		v->next = hull->vertices;
		v->prev = nullptr;

		if (hull->vertices)
			hull->vertices->prev = v;

		hull->vertices = v;

		hull->numVertices++;

		return v;
	}


	void buildInitialHull(vec3* points, int numPoints, qhHull* hull)
	{


		vec3* p0 = 0, *p1 = 0, *p2 = 0, *p3 = 0;
		qhFace* faces[4];

		// find longest Edge
		{
			static const vec3 dirs[6] =
			{
				vec3(1, 0, 0),
				vec3(0, 1, 0),
				vec3(0, 0, 1),
				vec3(-1, 0, 0),
				vec3(0, -1, 0),
				vec3(0, 0, -1)
			};


			// find extreme points
			int extremes[6] = { -1 };
			{
				float max[6] = { FLT_MIN };

				for (int i = 0; i < numPoints; ++i)
				{
					for (int j = 0; j < 6; ++j)
					{
						float dist = dot(points[i], dirs[j]);
						if (dist < max[j])
							extremes[j] = i, max[j] = dist;
					}
				}
			}

			// find two furthest apart
		{
			float max = 0.0f;
			for (int i = 0; i < 5; ++i)
			{
				for (int j = i + 1; j < 6; ++j)
				{
					float dist = lengthSq(points[j] - points[i]);
					if (dist > max)
					{
						p0 = points + extremes[i];
						p1 = points + extremes[j];
						max = dist;
					}
				}
			}
		}
		}

		Line e01 = lineFromAB(*p0, *p1);

		// complete first triangle
		{
			float max = 0.0f;
			for (int i = 0; i < numPoints; ++i)
			{
				float dist = sqDistPointLine(points[i], e01);
				if (dist > max)
				{
					p2 = points + i;
					max = dist;
				}

			}
		}


		qhVertex* v0 = addVertex(*p0, hull);
		qhVertex* v1 = addVertex(*p1, hull);
		qhVertex* v2 = addVertex(*p2, hull);

		qhVertex* vf0[3] = { v0, v1, v2 };
		faces[0] = addFace(hull, vf0);


		// find last Point
		{
			float max = 0.0f;
			for (int i = 0; i < numPoints; ++i)
			{
				float dist = -distPointPlane(points[i], faces[0]->plane);
				if (dist > max)
				{
					p3 = points + i;
					max = dist;
				}
			}
		}

		// build hull

		qhVertex* v3 = addVertex(*p3, hull);

		qhVertex* vf1[3] = { v0, v2, v3 };
		faces[1] = addFace(hull, vf1);
		qhVertex* vf2[3] = { v2, v1, v3 };
		faces[2] = addFace(hull, vf2);
		qhVertex* vf3[3] = { v1, v0, v3 };
		faces[3] = addFace(hull, vf3);

		// assign contact points
		for (int i = 0; i < numPoints; ++i)
		{
			if (points + i == p0 || points + i == p1 || points + i == p2 || points + i == p3)
				continue;

			float minDist = FLT_MAX;
			qhFace* f = nullptr;

			for (int j = 0; j < 4; ++j)
			{
				float dist = distPointPlane(points[i], faces[j]->plane);
				if (dist >  0 && dist < minDist)
				{
					minDist = dist;
					f = faces[j];
				}

			}

			if (f)
			{

				qhVertex* v = (*hull->vAlloc)();
				v->position = points[i];

				v->next = f->contactList;
				v->prev = nullptr;

				v->dist = minDist;

				if (f->contactList != nullptr)
					f->contactList->prev = v;

				f->contactList = v;

			}
		}


	}


	qhVertex* nextConflictVertex(qhHull* hull, qhFace** face)
	{

		float maxDist = 0.0f;

		qhFace* f = hull->faces;
		qhVertex* cV = nullptr;

		while (f != nullptr)
		{
			qhVertex* v = f->contactList;

			while (v != nullptr)
			{
				if (v->dist > maxDist)
					cV = v, maxDist = v->dist, *face = f;

				v = v->next;
			}
			f = f->next;
		}

		if (!cV)
			return nullptr;


		// remove from contact list
		if (cV->prev)
			cV->prev->next = cV->next;

		if (cV->next)
			cV->next->prev = cV->prev;

		if (cV == (*face)->contactList)
			(*face)->contactList = cV->next;

		// add to hull

		cV->next = hull->vertices;
		cV->prev = nullptr;

		if (hull->vertices)
			hull->vertices->prev = cV;

		hull->vertices = cV;

		hull->numVertices++;


		return cV;
	}



	// TODO non recursive
	void buildHorizon(qhVertex* v, qhFace* f, std::vector<qhHalfEdge*>& horizon)
	{

		f->visited = true;

		qhHalfEdge* h = f->edge;
		do
		{
			qhFace* nextFace = h->twin->face;

			if (nextFace->visited)
				continue;

			if (distPointPlane(v->position, nextFace->plane) > 0)
			{
				buildHorizon(v, nextFace, horizon);
			}
			else
			{
				horizon.push_back(h);
			}

			h = h->next;
		} while (h != f->edge);

	}

	void buildNewFaces(std::vector<qhFace*>& newFaces, qhVertex* vertex, std::vector<qhHalfEdge*>& horizon, qhHull* hull)
	{
		// todo remove unnecessary vertices
		// see if (?) vertices are behind all new faces??

		std::vector<qhFace*> oldFaces;
		// delete old ones
		{
			qhFace* f = hull->faces;

			while (f != nullptr)
			{
				if (f->visited)
				{
					oldFaces.push_back(f);

					if (f->prev)
						f->prev->next = f->next;
					if (f->next)
						f->next->prev = f->prev;

					if (f == hull->faces)
						hull->faces = f->next;

					hull->numFaces--;
				}

				f = f->next;
			}
		}

		// add new ones
		for (qhHalfEdge* h : horizon)
		{
			qhVertex* vs[3] = { h->tail, h->twin->tail, vertex };
			qhFace* f = addFace(hull, vs);
			newFaces.push_back(f);
		}

		// update contact lists

		for (qhFace* f : oldFaces)
		{
			qhVertex* cv = f->contactList;

			while (cv != nullptr)
			{
				qhVertex* cvNext = cv->next;

				float minDist = FLT_MAX;
				qhFace* fnew = nullptr;
				for (qhFace* f2 : newFaces)
				{
					float dist = distPointPlane(cv->position, f2->plane);
					if (dist > 0 && dist < minDist)
					{
						minDist = dist;
						fnew = f2;
					}

				}

				if (fnew)
				{
					cv->next = fnew->contactList;
					cv->prev = nullptr;

					if (fnew->contactList != nullptr)
						fnew->contactList->prev = cv;

					fnew->contactList = cv;

					cv->dist = minDist;
				}
				else
				{
					//remove
					if (cv->next)
						cv->next->prev = cv->prev;

					if (cv->prev)
						cv->prev->next = cv->next;

					qhVertex* _cv = cv;

					hull->vAlloc->sDelete(_cv);

				}

				cv = cvNext;
			}

		}



	}

	void mergeFaces(std::vector<qhFace*>& newFaces, qhHull* hull)
	{
		// todo remove unnecessary vertices/faces
		// see valve presentation


		for (qhFace* f : newFaces)
		{

			//check if face still valid
			if (f->edge->face != f)
			{
				continue;
			}

			qhHalfEdge* h = f->edge;

			do
			{
				qhFace* f2 = h->twin->face;

				float f1Dist = distPointFatPlane(f2->center, f->plane, hull->epsilon);
				float f2Dist = distPointFatPlane(f->center, f2->plane, hull->epsilon);

				if (f1Dist >= 0.0f || f2Dist >= 0.0f) // not convex
				{
					qhHalfEdge* ht = h->twin;

					f->edge = h->prev;

					// set face
					{
						qhHalfEdge* _h = ht;
						do
						{
							_h->face = f;
							_h = _h->next;
						} while (_h != ht);
					}

					// link edges
					h->prev->next = ht->next;
					h->next->prev = ht->prev;
					ht->prev->next = h->next;
					ht->next->prev = h->prev;




					// calc new center and plane
					{
						int numVertices = 0;
						vec3 verticesSum = vec3(0.0f, 0.0f, 0.0f);
						float plane1Dist = 0.0f;
						float plane2Dist = 0.0f;
						qhHalfEdge* h = f->edge;
						do
						{
							verticesSum += h->tail->position;
							numVertices++;

							plane1Dist += abs(distPointPlane(h->tail->position, f->plane));
							plane2Dist += abs(distPointPlane(h->tail->position, f2->plane));

							h = h->next;
						} while (h != f->edge);

						f->center = 1.0f / float(numVertices) * verticesSum;
						f->plane = plane1Dist < plane2Dist ? f->plane : f2->plane;
					}

					// remove f2
					if (f2->prev)
						f2->prev->next = f2->next;
					if (f2->next)
						f2->next->prev = f2->prev;

					if (hull->faces == f2)
					{
						hull->faces = f2->next;
					}


					hull->numFaces--;
				}


				h = h->next;

			} while (h != f->edge);


		}

	}


	void DEBUG_checkEdges(qhHull* hull, qhFace* f)
	{

		assert(f->edge->face == f);

		qhHalfEdge* h = f->edge;
		do
		{
			assert(h->prev->next == h);
			assert(h->next->prev == h);
			assert(h->twin != nullptr);
			h = h->next;
		} while (h != f->edge);
	}

	void DEBUG_checkEdges(qhHull* hull)
	{
		qhFace* f = hull->faces;
		while (f != nullptr)
		{

			DEBUG_checkEdges(hull, f);

			f = f->next;
		}
	}


	void addVertexToHull(qhVertex* vertex, qhFace* face, qhHull* hull)
	{
		std::vector<qhHalfEdge*> horizon;
		buildHorizon(vertex, face, horizon);

		std::vector<qhFace*> newFaces;
		buildNewFaces(newFaces, vertex, horizon, hull);

		mergeFaces(newFaces, hull);

	}


	void buildHullFromQuickhull(Hull* hull, qhHull* quickHull)
	{

		hull->epsilon = quickHull->epsilon;

		hull->numVertices = quickHull->numVertices;
		hull->numFaces = quickHull->numFaces;
		hull->numEdges = (hull->numVertices + hull->numFaces - 2) * 2;

		hull->pVertices = new vec3[hull->numVertices];
		hull->pEdges = new HalfEdge[hull->numEdges];
		hull->pFaces = new Face[hull->numFaces];
		hull->pPlanes = new Plane[hull->numFaces];

		std::map <qhVertex*, int> vertMap;
		std::map < std::pair<int, int>, int> edgeMap;

		// set vertices and centroid
		qhVertex* v = quickHull->vertices;
		hull->centroid = vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; v != nullptr; ++i)
		{
			hull->pVertices[i] = v->position;
			hull->centroid += v->position;
			vertMap[v] = i;
			v = v->next;
		}
		hull->centroid = 1.0f / hull->numVertices * hull->centroid;



		// set faces and edges
		int edgeHead = 0;
		qhFace* f = quickHull->faces;
		for (int i = 0; f != nullptr; ++i)
		{
			hull->pPlanes[i] = f->plane;

			int last = -1;
			int first = -1;

			qhHalfEdge* h = f->edge;
			do
			{
				// halfedges and their twins are always adjacent
				// the halfedge with the lower vertex index comes first


				int A = vertMap[h->tail];
				int B = vertMap[h->twin->tail];

				std::pair<int, int> edgeKey = A < B ? std::make_pair(A, B) : std::make_pair(B, A);




				int edgePos;
				int twinPos;

				if (edgeMap.count(edgeKey) == 0)
				{
					// twin was not yet built
					edgePos = edgeHead;
					edgeMap[edgeKey] = edgeHead;
					edgeHead += 2;
				}
				else
				{
					// get twin
					edgePos = edgeMap[edgeKey];
				}

				twinPos = edgePos + 1;

				if (A > B)
				{
					// halfedge is the second halfedge
					std::swap(edgePos, twinPos);
				}

				HalfEdge* edge;

				edge = hull->pEdges + edgePos;
				edge->twin = twinPos;
				edge->face = i;
				edge->tail = A;

				// connect the previous edge with this one
				if (last != -1)
					hull->pEdges[last].next = edgePos;
				else
					first = edgePos;

				last = edgePos;

				h = h->next;

			} while (h != f->edge);

			// connect the last edge with the first
			hull->pEdges[last].next = first;
			// set the face edge
			hull->pFaces[i].edge = first;

			f = f->next;
		}
	}


	void quickHull(vec3* points, int numPoints, Hull* hull)
	{




		VertexAllocator vertices(numPoints);
		EdgeAllocator edges((3 * numPoints - 6) * 2);
		FaceAllocator faces(2 * numPoints - 4);

		qhHull newHull;

		newHull.vAlloc = &vertices;
		newHull.eAlloc = &edges;
		newHull.fAlloc = &faces;

		vec3 max = vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < numPoints; ++i)
		{
			max.x = ong_MAX(abs(points[i].x), max.x);
			max.y = ong_MAX(abs(points[i].y), max.y);
			max.z = ong_MAX(abs(points[i].z), max.z);
		}

		newHull.epsilon = (max.x + max.y + max.z) * FLT_EPSILON;

		//
		buildInitialHull(points, numPoints, &newHull);


		qhFace* currFace = nullptr;
		qhVertex* currVertex = nextConflictVertex(&newHull, &currFace);
		while (currVertex != nullptr)
		{
			addVertexToHull(currVertex, currFace, &newHull);
			currVertex = nextConflictVertex(&newHull, &currFace);
		}


		//build hull

		buildHullFromQuickhull(hull, &newHull);

	}

}