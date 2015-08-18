#include "BrickLoader.h"
#include <stdio.h>
#include "World.h"

using namespace ong;

struct SNub
{
	vec3 a;
	vec3 b;
	bool c;
};

ong::Body* loadBrick(char* pFile, ong::World* pWorld)
{
	FILE* file = fopen(pFile, "rb");

	//m_pModel
	char * m_ModelPath = (char*)malloc(200);
	fread(m_ModelPath, 200, 1, file);

	//m_BrickId
	unsigned int brickId;
	fread(&brickId, sizeof(brickId), 1, file);

	//m_IsTransparent
	bool isTransparent;
	fread(&isTransparent, sizeof(isTransparent), 1, file);

	//m_PivotOffset
	vec3 pivotoffset;
	fread(&pivotoffset, sizeof(pivotoffset), 1, file);

	//m_pNubs
	unsigned int numNubs;
	fread(&numNubs, sizeof(numNubs), 1, file);
	for (unsigned int i = 0; i < numNubs; i++)
	{
		
		SNub nub;
		fread(&nub, sizeof(nub), 1, file);
	}

	BodyDescription descr;
	descr.transform.p = vec3(0, 0, 0);
	descr.transform.q = Quaternion(vec3(0, 0, 0), 1);
	descr.continuousPhysics = false;
	descr.linearMomentum = vec3(0, 0, 0);
	descr.type = BodyType::Dynamic;
	
	Body* body = pWorld->createBody(descr);
	
	unsigned int colliderDataSize;
	fread(&colliderDataSize, sizeof(unsigned int), 1, file);
	for (unsigned int i = 0; i < colliderDataSize; i++)
	{
		ong::ColliderData colliderData;
		//transform
		fread(&colliderData.transform, sizeof(colliderData.transform), 1, file);
		//pMaterial

		//massData
		fread(&colliderData.massData, sizeof(colliderData.massData),1, file);
		//aabb
		fread(&colliderData.aabb, sizeof(colliderData.aabb),1, file);
		//shape
		ong::ShapeDescription shapeDesc;
		fread(&shapeDesc.type, sizeof(int), 1, file); //m_type
		switch (shapeDesc.shapeType) //m_shape
		{
		case ong::ShapeType::SPHERE:
			fread(&shapeDesc.sphere, sizeof(shapeDesc.sphere), 1, file);
			break;
		case ong::ShapeType::CAPSULE:
			fread(&shapeDesc.capsule, sizeof(shapeDesc.capsule), 1, file);
			break;
		case ong::ShapeType::HULL:
			ong::Hull* hull = &shapeDesc.hull;
			fread(&hull->centroid, sizeof(hull->centroid), 1, file);
			//Vertices
			fread(&hull->numVertices, sizeof(hull->numVertices), 1, file);
			hull->pVertices = new vec3[hull->numVertices];
			fread(hull->pVertices, sizeof(vec3), hull->numVertices, file);
			//Edges
			fread(&hull->numEdges, sizeof(hull->numEdges), 1, file);
			hull->pEdges = new HalfEdge[hull->numEdges];
			fread(hull->pEdges, sizeof(HalfEdge), hull->numEdges, file);
			//Faces
			fread(&hull->numFaces, sizeof(hull->numFaces), 1, file);
			hull->pFaces = new Face[hull->numFaces];
			fread(hull->pFaces, sizeof(Face), hull->numFaces, file);

			hull->pPlanes = new Plane[hull->numFaces];
			fread(hull->pPlanes, sizeof(Plane), hull->numFaces, file);
			//epsilon
			fread(&hull->epsilon, sizeof(hull->epsilon), 1, file);
			break;
		}
		colliderData.shape = pWorld->createShape(shapeDesc);

		ong::Material material;
		material.density = 1.0f;
		material.restitution = 0.6f;
		material.friction = 1.0f;
		ong::Material* mat = pWorld->createMaterial(material);

		colliderData.pMaterial = mat;

		colliderData.isSensor = false;
		colliderData.collisionFilter = 0;
		colliderData.collisionGroup = 0;

		Collider* collider = pWorld->createCollider(colliderData);
		body->addCollider(collider);
		
	}

	return body;
}