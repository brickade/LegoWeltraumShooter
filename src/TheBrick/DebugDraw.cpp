#include "include\TheBrick\DebugDraw.h"

#include <PuReEngine\Core.h>
#include <Onager\Body.h>
#include <Onager\Collider.h>

namespace TheBrick
{
	static PuRe_IMaterial* g_pMaterial = nullptr;



	void DrawBody(ong::Body* a_pBody, PuRe_Camera* a_pCam, PuRe_IGraphics* a_pGraphics)
	{
		if (!g_pMaterial)
			g_pMaterial = a_pGraphics->LoadMaterial("..\\Data\\effects\\default\\line");

		g_pMaterial->Apply();

		a_pCam->Apply(g_pMaterial);


		ong::Transform bodyTransform = a_pBody->getTransform();
		for (ong::Collider* pC = a_pBody->getCollider(); pC != nullptr; pC = pC->getNext())
		{
			ong::Transform t = ong::transformTransform(pC->getTransform(), bodyTransform);
			ong::mat3x3 rot = ong::toRotMat(t.q);

			if (pC->getShape().getType() == ong::ShapeType::CAPSULE)
			{
				const ong::Capsule* c = pC->getShape();

				static const int POINTCOUNT = 32;
				ong::vec3 up = normalize(c->c2 - c->c1);
				ong::vec3 l;
				if (abs(up.x) < abs(up.y) < abs(up.z))
					l = normalize(cross(ong::vec3(1, 0, 0), up));
				else if (abs(up.y) < abs(up.z))
					l = normalize(cross(ong::vec3(0, 1, 0), up));
				else
					l = normalize(cross(ong::vec3(0, 0, 1), up));


				ong::vec3 s = normalize(cross(up, l));

				ong::mat3x3 cRot(
					ong::vec3(s.x, up.x, l.x),
					ong::vec3(s.y, up.y, l.y),
					ong::vec3(s.z, up.z, l.z));

				rot = cRot*rot;
			}

			PuRe_MatrixF rotation(
				rot[0][0], rot[0][1], rot[0][2], 0,
				rot[1][0], rot[1][1], rot[1][2], 0,
				rot[2][0], rot[2][1], rot[2][2], 0,
				0, 0, 0, 1);
			
			g_pMaterial->SetMatrix(PuRe_MatrixF::Translation(PuRe_Vector3F(t.p.x, t.p.y, t.p.z)), "Translation");
			g_pMaterial->SetMatrix(rotation, "Rotation");
			g_pMaterial->SetMatrix(PuRe_MatrixF::Scale(PuRe_Vector3F(1, 1, 1)), "Scale");
			
			ong::ShapePtr shape = pC->getShape();

			switch (shape.getType())
			{
			case ong::ShapeType::HULL:
			{

				ong::Hull* hull = shape;

				PuRe_IVertexBuffer* pVertexBuffer = a_pGraphics->CreateVertexBuffer(hull->numEdges * sizeof(PuReEngine::Core::SVertex), sizeof(PuReEngine::Core::SVertex));
				PuReEngine::Core::SVertex* pVertexData = (PuReEngine::Core::SVertex*)pVertexBuffer->Lock();

				for (int i = 0; i < hull->numEdges; i += 2)
				{
					ong::vec3 p1 = hull->pVertices[hull->pEdges[i].tail];
					ong::vec3 p2 = hull->pVertices[hull->pEdges[i + 1].tail];

					pVertexData[i].Position = PuRe_Vector3F(p1.x, p1.y, p1.z);
					pVertexData[i].Color = PuRe_Vector3F(1, 0, 0);

					pVertexData[i+1].Position = PuRe_Vector3F(p2.x, p2.y, p2.z);
					pVertexData[i+1].Color = PuRe_Vector3F(1, 0, 0);
				}
				
				pVertexBuffer->Unlock();

				a_pGraphics->SetVertexBuffer(pVertexBuffer);
				a_pGraphics->Draw(PuRe_Primitive::Lines, hull->numEdges);
				delete pVertexBuffer;

				break;
			}
			case ong::ShapeType::SPHERE:
			{

				const ong::Sphere* sphere = pC->getShape();

				static const int POINTCOUNT = 16;

				int numVerts = 2*POINTCOUNT * sphere->r;

				PuRe_IVertexBuffer* pVertexBuffer = a_pGraphics->CreateVertexBuffer(numVerts * 3 * sizeof(PuReEngine::Core::SVertex), sizeof(PuReEngine::Core::SVertex));
				PuReEngine::Core::SVertex* pVertexData = (PuReEngine::Core::SVertex*)pVertexBuffer->Lock();
				
				for (int i = 0; i < 3; ++i)
				{
					for (int j = 0; j < numVerts/2; j++)
					{
						int k = (j+1) % (numVerts/2);

						ong::vec3 p1 = sphere->c;
						ong::vec3 p2 = sphere->c;

						p1[(i + 1) % 3] += sin(2 * ong_PI * j / (POINTCOUNT * sphere->r)) * sphere->r;
						p1[(i + 2) % 3] += cos(2 * ong_PI * j / (POINTCOUNT * sphere->r)) * sphere->r;

						p2[(i + 1) % 3] += sin(2 * ong_PI * k / (POINTCOUNT * sphere->r)) * sphere->r;
						p2[(i + 2) % 3] += cos(2 * ong_PI * k / (POINTCOUNT * sphere->r)) * sphere->r;

						int idx1 = i*numVerts + j * 2;
						int idx2 = idx1 + 1;
						
						assert(idx1 < numVerts * 3);
						assert(idx2 < numVerts * 3);

						pVertexData[idx1].Position = PuRe_Vector3F(p1.x, p1.y, p1.z);
						pVertexData[idx1].Color = PuRe_Vector3F(1, 0, 0);

						pVertexData[idx2].Position = PuRe_Vector3F(p2.x, p2.y, p2.z);
						pVertexData[idx2].Color = PuRe_Vector3F(1, 0, 0);
					}
				}

				pVertexBuffer->Unlock();

				a_pGraphics->SetVertexBuffer(pVertexBuffer);
				a_pGraphics->Draw(PuRe_Primitive::Lines, numVerts*3);
				delete pVertexBuffer;

				break;
			}
			case ong::ShapeType::CAPSULE:
			{
				const ong::Capsule* c = pC->getShape();

				static const int POINTCOUNT = 32;
				int numVerts = 2 * POINTCOUNT * c->r;

				ong::vec3 v = ong::vec3(0, 1, 0);
				float h = length(c->c2 - c->c1) / 2;

				PuRe_IVertexBuffer* pVertexBuffer = a_pGraphics->CreateVertexBuffer(numVerts * 4 * sizeof(PuReEngine::Core::SVertex), sizeof(PuReEngine::Core::SVertex));
				PuReEngine::Core::SVertex* pVertexData = (PuReEngine::Core::SVertex*)pVertexBuffer->Lock();


				for (int i = 0; i < 3; i+=2)
				{
					for (int j = 0; j < numVerts / 2; j++)
					{
						int k = (j + 1) % (numVerts / 2);

						ong::vec3 p1 = 0.5f* (c->c1 + c->c2);
						ong::vec3 p2 = 0.5f* (c->c1 + c->c2);

						p1[(i + 1) % 3] += sin(2 * ong_PI * j / (POINTCOUNT * c->r)) * c->r;
						p1[(i + 2) % 3] += cos(2 * ong_PI * j / (POINTCOUNT * c->r)) * c->r;
																			  		   
						p2[(i + 1) % 3] += sin(2 * ong_PI * k / (POINTCOUNT * c->r)) * c->r;
						p2[(i + 2) % 3] += cos(2 * ong_PI * k / (POINTCOUNT * c->r)) * c->r;

						if (p1.y > 0.0f)
							p1.y += h;
						else
							p1.y -= h;

						if (p2.y > 0.0f)
							p2.y += h;
						else
							p2.y -= h;

						int idx1 = 0.5f*i*numVerts + j * 2;
						int idx2 = idx1 + 1;

						assert(idx1 < numVerts * 4);
						assert(idx2 < numVerts * 4);

						pVertexData[idx1].Position = PuRe_Vector3F(p1.x, p1.y, p1.z);
						pVertexData[idx1].Color = PuRe_Vector3F(1, 0, 0);

						pVertexData[idx2].Position = PuRe_Vector3F(p2.x, p2.y, p2.z);
						pVertexData[idx2].Color = PuRe_Vector3F(1, 0, 0);
					}
				}

				for (int i = -1; i < 2; i += 2)
				{
					for (int j = 0; j < numVerts / 2; j++)
					{
						int k = (j + 1) % (numVerts / 2);

						ong::vec3 p1 = 0.5f* (c->c1 + c->c2);
						ong::vec3 p2 = 0.5f* (c->c1 + c->c2);

						p1[2] += sin(2 * ong_PI * j / (POINTCOUNT * c->r)) * c->r;
						p1[1] += i*h;
						p1[0] += cos(2 * ong_PI * j / (POINTCOUNT * c->r)) * c->r;

						p2[2] += sin(2 * ong_PI * k / (POINTCOUNT * c->r)) * c->r;
						p2[1] += i*h;
						p2[0] += cos(2 * ong_PI * k / (POINTCOUNT * c->r)) * c->r;

						int idx1 = (i > 0 ? 2 : 3)*numVerts + j * 2;
						int idx2 = idx1 + 1;

						assert(idx1 < numVerts * 4);
						assert(idx2 < numVerts * 4);

						pVertexData[idx1].Position = PuRe_Vector3F(p1.x, p1.y, p1.z);
						pVertexData[idx1].Color = PuRe_Vector3F(1, 0, 0);

						pVertexData[idx2].Position = PuRe_Vector3F(p2.x, p2.y, p2.z);
						pVertexData[idx2].Color = PuRe_Vector3F(1, 0, 0);
					}

				}


				pVertexBuffer->Unlock();

				a_pGraphics->SetVertexBuffer(pVertexBuffer);
				a_pGraphics->Draw(PuRe_Primitive::Lines, numVerts * 4);
				delete pVertexBuffer;

				break;
			}


			}



		}
		

	}

}