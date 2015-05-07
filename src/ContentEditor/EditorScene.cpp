#include "EditorScene.h"

#include <Windows.h>
#include <wchar.h>
#include <Onager\myMath.h>

#include <TheBrick\DebugDraw.h>
#include <TheBrick\Conversion.h>

#include <PuReEngine\Camera.h>
#include <stdio.h>

namespace Content
{
	CEditorScene::CEditorScene(PuRe_Application* a_pApplication)
		: m_pApplication(a_pApplication)
	{
	}

	CEditorScene::~CEditorScene()
	{
		
	}

	void CEditorScene::getInput()
	{
		while (m_Polling)
		{
			char buffer[1024];

			//todo change to hashmap
			while (scanf("%1024s", buffer))
			{
				if (strcmp(buffer, "test") == 0)
				{
					SCommand c;
					c.type = SCommand::TEST;

					m_Queue.push(c);
				}
				else if (strcmp(buffer, "next") == 0)
				{
					SCommand c;
					c.type = SCommand::NEXT;
					m_Queue.push(c);
				}
				else if (strcmp(buffer, "box") == 0)
				{
					SCommand c;
					c.type = SCommand::NEWCOLLIDER;
					c.shape.constructionType = ong::ShapeConstruction::HULL_FROM_BOX;

					ong::vec3& boxC = c.shape.hullFromBox.c;
					ong::vec3& boxE = c.shape.hullFromBox.e;

					scanf("%f %f %f %f %f %f", &boxC.x, &boxC.y, &boxC.z, &boxE.x, &boxE.y, &boxE.z);


				

					boxC.x = (boxC.x - 0.25f*boxE.x) * 2.0f*TheBrick::CBrick::SEGMENT_WIDTH;
					boxC.z = (boxC.z + 0.25f*boxE.z) * 2.0f*TheBrick::CBrick::SEGMENT_WIDTH;
					boxC.y = (boxC.y + 0.25f*boxE.y) * 2.0f*TheBrick::CBrick::SEGMENT_HEIGHT;

					boxE.x *= 0.5f*TheBrick::CBrick::SEGMENT_WIDTH;
					boxE.z *= 0.5f*TheBrick::CBrick::SEGMENT_WIDTH;
					boxE.y *= 0.5f*TheBrick::CBrick::SEGMENT_HEIGHT;

					m_Queue.push(c);
				}
				else if (strcmp(buffer, "sphere") == 0)
				{
					SCommand c;
					c.type = SCommand::NEWCOLLIDER;
					c.shape.shapeType = ong::ShapeType::SPHERE;

					ong::vec3& sC = c.shape.sphere.c;
					float& sR= c.shape.sphere.r;

					scanf("%f %f %f %f", &sC.x, &sC.y, &sC.z, &sR);

					sC.x *= TheBrick::CBrick::SEGMENT_WIDTH;
					sC.z *= TheBrick::CBrick::SEGMENT_WIDTH;
					sC.y *= TheBrick::CBrick::SEGMENT_HEIGHT;

					sR *= TheBrick::CBrick::SEGMENT_WIDTH;
					m_Queue.push(c);
				}
				else if (strcmp(buffer, "capsule") == 0)
				{
					SCommand c;
					c.type = SCommand::NEWCOLLIDER;
					c.shape.shapeType = ong::ShapeType::CAPSULE;

					ong::vec3& cC1 = c.shape.capsule.c1;
					ong::vec3& cC2 = c.shape.capsule.c2;
					float& cR = c.shape.capsule.r;

					scanf("%f %f %f %f %f %f %f", &cC1.x, &cC1.y, &cC1.z, &cC2.x, &cC2.y, &cC2.z, &cR);

					cC1.x *= TheBrick::CBrick::SEGMENT_WIDTH;
					cC1.z *= TheBrick::CBrick::SEGMENT_WIDTH;
					cC1.y *= TheBrick::CBrick::SEGMENT_HEIGHT;
					
					cC2.x *= TheBrick::CBrick::SEGMENT_WIDTH;
					cC2.z *= TheBrick::CBrick::SEGMENT_WIDTH;
					cC2.y *= TheBrick::CBrick::SEGMENT_HEIGHT;

					cR *= TheBrick::CBrick::SEGMENT_WIDTH;
					m_Queue.push(c);
				}
				else if (strcmp(buffer, "hull") == 0)
				{
					SCommand c;
					c.type = SCommand::NEWCOLLIDER;
					c.shape.constructionType = ong::ShapeConstruction::HULL_FROM_POINTS;
					
					int& numPoints = c.shape.hullFromPoints.numPoints;
					scanf("%d", &numPoints);

					c.shape.hullFromPoints.points = new ong::vec3[numPoints];

					for (int i = 0; i < numPoints; ++i)
					{
						ong::vec3& p = c.shape.hullFromPoints.points[i];

						scanf("%f %f %f", &p.x, &p.y, &p.z, &p.x, &p.y, &p.z);

						p.x *= TheBrick::CBrick::SEGMENT_WIDTH;
						p.z *= TheBrick::CBrick::SEGMENT_WIDTH;
						p.y *= TheBrick::CBrick::SEGMENT_HEIGHT;
					}

					m_Queue.push(c);
				}
				else if (strcmp(buffer, "undo") == 0)
				{
					SCommand c;
					c.type = SCommand::UNDO;
					m_Queue.push(c);
				}
				else if (strcmp(buffer, "brickID") == 0)
				{
					SCommand c;
					c.type = SCommand::BRICKID;
					scanf("%u", &c.brickID);
					m_Queue.push(c);
				}
				else if (strcmp(buffer, "transparent") == 0)
				{
					SCommand c;
					c.type = SCommand::TRANSPARENT;
					scanf("%d", &c.transparent);
					m_Queue.push(c);
				}
				else if (strcmp(buffer, "help") == 0)
				{
					printf(
						"Control camera with alt + mouse.\n"
						"Keyboard shortcuts:\n"
						"\'q\' male nubs\n"
						"\'w\' female nubs\n"
						"\'e\' pivot\n\n"
						"Commands:\n"
						"\"next\" \n"
						"get next file in the directory and save current brick\n"
						"\"undo\" \n"
						"delete the last constructed collider\n"
						"\"brickID id\" \n"
						"set the brickid of the current brick to id\n"
						"\"transparent i\" \n"
						"set if the brick is transparent (0 = false, != 0 = true)\n"
						"create box collider with center c and width w, height h, and depth d\n"
						"\"box cx cy cz w h d\" \n"
						"create box collider with center c and width w, height h, and depth d\n"
						"\"sphere cx cy cz r\" \n"
						"create sphere collider with center c and and radius r\n"
						"\"capsule c1x c1y c1z c2x c2y c2z r\" \n"
						"create capsule collider from c1 to c2 and with radius rd\n"
						"\"hull n p1x p1y p1z p2x p2y p2z ... \"\n"
						"create hull collider from n points with the points p1, p2 etc\n"
						);
				}
				else
				{
					printf("unknown command\n");
				}

			}
		}
	}


	void CEditorScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
	{

		printf("type \"help\" for help\n");

		PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
		
		m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/editor/default");
		
		m_pPostCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
		m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
		m_pRenderTarget = a_pGraphics->CreateRendertarget(m_pPostMaterial);

		m_pCamera = new PuRe_Camera(PuRe_Vector2F(gdesc.ResolutionWidth, gdesc.ResolutionHeight), PuRe_Camera_Perspective);
		m_pCamera->SetFoV(45.0f);
		m_pCamera->Update();

		m_pCurrBrick = 0;

		m_Polling = true;
		m_InputThread = std::thread(&CEditorScene::getInput, this);


		// get all bricks in directories
		WIN32_FIND_DATA findFileData;
		HANDLE hFind;

		wchar_t currentPath[FILENAME_MAX];

		GetCurrentDirectory(sizeof(currentPath), currentPath);
		wcscat_s(currentPath, L"\\..\\data\\newContent\\*");

		char dir[] = "..\\data\\newContent\\";

		printf("search Dir: %S \n", currentPath);

		hFind = FindFirstFile(currentPath, &findFileData);
		
		while (hFind != INVALID_HANDLE_VALUE)
		{
			findFileData.cFileName;

			wchar_t* fileType = wcsrchr(findFileData.cFileName, '.');

			if (fileType != 0)
			{
				mbstate_t conversionState;

				char fileName[FILENAME_MAX];
				const wchar_t* wideFileName = findFileData.cFileName;


				wcsrtombs(fileName, &wideFileName, FILENAME_MAX, &conversionState);

				char fileDir[FILENAME_MAX];
				strcpy_s(fileDir, dir);
				strcat_s(fileDir, fileName);

				printf("%s \n", fileName);

				//new brick from obj
				if (wcscmp(fileType, L".obj") == 0)
				{
					PuRe_Model* pModel = new PuRe_Model(a_pGraphics, m_pMaterial, fileDir);


					if (pModel != 0)
					{
						char modelDir[FILENAME_MAX] = { 0 };
						strcat(modelDir, "..\\data\\models\\");
						strcat(modelDir, fileName);


						TheBrick::CBrick* newBrick = new TheBrick::CBrick(pModel);
						newBrick->SetMaterial(m_pMaterial);
						newBrick->SetModelPath(modelDir);
						m_BrickQueue.push(std::make_pair(fileName, newBrick));
					}
				}
				else if (wcscmp(fileType, L".brick") == 0)
				{

					TheBrick::CBrick* newBrick = new TheBrick::CBrick();

					if (m_Serializer.OpenRead(fileDir))
					{
						newBrick->Deserialize(&m_Serializer, a_pGraphics, m_pMaterial, &m_World);
						m_Serializer.Close();


						//char modelDir[FILENAME_MAX] = { 0 };
						//strcat(modelDir, "..\\data\\models\\");
						//strncat(modelDir, fileName, strrchr(fileName, '.') - fileName);
						//strcat(modelDir, ".obj");

						const char* modelDir = newBrick->GetModelPath();

						PuRe_Model* pModel = new PuRe_Model(a_pGraphics, m_pMaterial, modelDir);
						
						newBrick->SetModel(pModel);
						newBrick->SetMaterial(m_pMaterial);
						m_BrickQueue.push(std::make_pair(fileName, newBrick));
					}
					else
					{
						printf("failed to load %s\n", fileDir);
					}




				}
			}
			if (!FindNextFile(hFind, &findFileData))
			{
				int error = GetLastError();
				break;
			}
		}
		FindClose(hFind);

		m_Queue.push({ SCommand::NEXT });

		ong::BodyDescription bodyDescr;
		bodyDescr.linearMomentum = ong::vec3(0, 0, 0);
		bodyDescr.angularMomentum = ong::vec3(0, 0, 0);
		bodyDescr.transform = ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1));
		bodyDescr.type = ong::BodyType::Static;

		m_pBody = m_World.createBody(bodyDescr);
		

		m_MouseTransform.p = ong::vec3(0, 0, 0);
		m_MouseTransform.q = ong::QuatFromAxisAngle(ong::vec3(1, 0, 0), 0);

		ong::ShapeDescription mouse;
		mouse.shapeType = ong::ShapeType::SPHERE;
		mouse.sphere.c = ong::vec3(0, 0, 0);
		mouse.sphere.r = 0.01f;

		//mouse.constructionType = ong::ShapeConstruction::HULL_FROM_BOX;
		//mouse.hullFromBox.c = ong::vec3(0, 0.1, 0);
		//mouse.hullFromBox.e = ong::vec3(0.01, 0.1, 0.01);

		m_MousePtr = m_World.createShape(mouse);

		ong::vec3 nubPoints[] = 
		{
			ong::vec3(TheBrick::CBrick::SEGMENT_WIDTH/2, 0, TheBrick::CBrick::SEGMENT_WIDTH/2),
			ong::vec3(-TheBrick::CBrick::SEGMENT_WIDTH/2, 0, TheBrick::CBrick::SEGMENT_WIDTH/2),
			ong::vec3(TheBrick::CBrick::SEGMENT_WIDTH/2, 0, -TheBrick::CBrick::SEGMENT_WIDTH/2),
			ong::vec3(-TheBrick::CBrick::SEGMENT_WIDTH/2, 0, -TheBrick::CBrick::SEGMENT_WIDTH/2),
			ong::vec3(0, TheBrick::CBrick::SEGMENT_HEIGHT, 0)
		};

		ong::ShapeDescription nub;
		nub.constructionType = ong::ShapeConstruction::HULL_FROM_POINTS;
		nub.hullFromPoints.points = nubPoints;
		nub.hullFromPoints.numPoints = 5;
		
		m_NubShape = m_World.createShape(nub);

		ong::ShapeDescription origin;
		origin.shapeType = ong::ShapeType::SPHERE;
		origin.sphere.c = ong::vec3(0.0f, 0.0f, 0.0f);
		origin.sphere.r = 0.1f;

		m_OriginShape = m_World.createShape(origin);

		m_MouseValid = false;

	}


	bool CEditorScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
	{
		if (a_pInput->GetCursorLock())
			a_pInput->UnLockCursor();


		if (a_pInput->KeyIsPressed(a_pInput->Alt))
		{
			if (a_pInput->MouseIsPressed(a_pInput->LeftClick))
			{
				PuRe_Vector3F rot = PuRe_Vector3F(0, 1, 0) *  (a_pInput->GetRelativeMousePosition().X / 4.0f);

				m_pCamera->Rotate(rot.X, rot.Y, rot.Z);

				rot = m_pCamera->GetSide() *  (-a_pInput->GetRelativeMousePosition().Y / 4.0f);

				m_pCamera->Rotate(rot.X, rot.Y, rot.Z);
			}
			if (a_pInput->MouseIsPressed(a_pInput->MiddleClick))
			{
				m_pCamera->Move(PuRe_Vector3F(-a_pInput->GetRelativeMousePosition().X / 80.0f, a_pInput->GetRelativeMousePosition().Y / 80.0f, 0.0f));
			}
			if (a_pInput->GetMouseScroll())
			{
				m_pCamera->Move(PuRe_Vector3F(0, 0, a_pInput->GetMouseScroll()));
			}
		}
		else
		{
			if (m_MouseValid)
			{
				if (a_pInput->MousePressed(a_pInput->LeftClick))
				{
					switch (m_Mode)
					{
					case Mode::NUB:
					{
						bool duplicate = false;
						for (auto& nub : m_pCurrBrick->GetNubs())
						{
							if (nub.Position == m_NubPtr.Position)
							{
								duplicate = true;
								break;
							}
						}

						if (!duplicate)
							m_pCurrBrick->GetNubs().push_back(m_NubPtr);
						break;
					}
					case Mode::ORIGIN:
						m_pCurrBrick->SetPivotOffset(TheBrick::OngToPuRe(m_Pivot));
						break;
					}

				}
				else if (a_pInput->MousePressed(a_pInput->RightClick))
				{
					switch (m_Mode)
					{
					case Mode::NUB:
					{
						for (auto nub = m_pCurrBrick->GetNubs().begin(); nub != m_pCurrBrick->GetNubs().end();nub++)
						{
							if (nub->Position == m_NubPtr.Position && nub->isMale == m_NubPtr.isMale)
							{
								m_pCurrBrick->GetNubs().erase(nub);
								break;
							}
						}
						break;
					}

					}
				}
			}



			if (a_pInput->KeyPressed(a_pInput->Q))
				m_Mode = Mode::NUB, m_NubPtr.isMale = true;
			else if (a_pInput->KeyPressed(a_pInput->W))
				m_Mode = Mode::NUB, m_NubPtr.isMale = false;
			else if (a_pInput->KeyPressed(a_pInput->E))
				m_Mode = Mode::ORIGIN;
		}

		// ray cast
		PuRe_WindowDescription wDescr = a_pWindow->GetDescription();
		PuRe_Vector3F mousePos;
		mousePos.X = (2.0f * a_pInput->GetAbsoluteMousePosition().X) / wDescr.Width - 1.0f;
		mousePos.Y = 1.0f - (2.0f * a_pInput->GetAbsoluteMousePosition().Y) / wDescr.Height;
		mousePos.Z = 0.0f;

		PuRe_Vector4F rayV = PuRe_Vector4F(mousePos.X, mousePos.Y, 1.0f, 1.0f) * PuRe_MatrixF::Invert(m_pCamera->GetProjection());
		PuRe_Vector4F rayW = rayV*PuRe_MatrixF::Invert(m_pCamera->GetView());

		ong::vec3 rayDir = ong::normalize(ong::vec3(rayW.X, rayW.Y, rayW.Z));
		ong::vec3 rayO = ong::vec3(m_pCamera->GetPosition().X, m_pCamera->GetPosition().Y, m_pCamera->GetPosition().Z);

		//printf("%f %f %f\n", rayDir.x, rayDir.y, rayDir.z);

		ong::RayQueryResult rayResult = { 0 };
		if (m_pBody->queryRay(rayO, rayDir, &rayResult))
		{
			m_MouseTransform.p = rayO + 0.999f*rayResult.t* rayDir;
			//m_MouseTransform.p = rayResult.point;
			m_MouseValid = true;
		}
		else
		{
			m_MouseTransform.p = ong::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
			m_MouseValid = false;
		}

		float w = TheBrick::CBrick::SEGMENT_WIDTH;
		float h = TheBrick::CBrick::SEGMENT_HEIGHT;


		PuRe_Vector3F dir = m_pCamera->GetForward();


		switch (m_Mode)
		{
		case Mode::NUB:
			m_NubPtr.Position.X = (floor(m_MouseTransform.p.x / w) + 0.5) * w - (rayResult.normal.x * 0.5f*w);
			m_NubPtr.Position.Y = (floor(m_MouseTransform.p.y / h) + 0.5) * h - (rayResult.normal.y * 0.5f*h);
			m_NubPtr.Position.Z = (floor(m_MouseTransform.p.z / w) + 0.5) * w - (rayResult.normal.z * 0.5f*w);

			m_NubPtr.Direction = TheBrick::OngToPuRe(rayResult.normal);

			break;
		case Mode::ORIGIN:
			m_Pivot.x = (floor(m_MouseTransform.p.x / w) + 0.5) * w - (rayResult.normal.x * 0.5f*w);
			m_Pivot.y = (floor(m_MouseTransform.p.y / h) + 0.5) * h - (rayResult.normal.y * 0.5f*h);
			m_Pivot.z = (floor(m_MouseTransform.p.z / w) + 0.5) * w - (rayResult.normal.z * 0.5f*w);
			break;
		}




		while (!m_Queue.isEmpty())
		{
			SCommand c = m_Queue.pop();
			if(ProcessCommand(c))
				m_OldCommands.push(c);
		}


		return false;
	}
		

	bool CEditorScene::ProcessCommand(SCommand& a_C)
	{

		switch (a_C.type)
		{
		case SCommand::TEST:
			printf("test\n");
			return false;
		case SCommand::NEXT:
			if (m_pCurrBrick)
			{
				ong::Collider* pCollider = m_pBody->getCollider();
				while (pCollider != 0)
				{
					ong::Collider* pNext = pCollider->getNext();
					m_pCurrBrick->GetColliderData().push_back(pCollider->getData());

					m_World.destroyCollider(pCollider);
					pCollider = pNext;
				}
				if (m_Serializer.OpenWrite(m_CurrFileName.c_str()))
				{
					m_pCurrBrick->Serialize(&m_Serializer);
					m_Serializer.Close();
				}


				delete m_pCurrBrick;
			}

			if (!m_BrickQueue.empty())
			{
				auto front = m_BrickQueue.front();
				printf("currBrick: %s\n", front.first.c_str());
				m_pCurrBrick = front.second;
				m_CurrFileName = "..\\data\\bricks\\" + std::string(front.first.begin(), front.first.begin()+front.first.find_last_of('.'));
				m_CurrFileName.append(".brick");

				for (auto& colliderData : m_pCurrBrick->GetColliderData())
				{
					m_pBody->addCollider(m_World.createCollider(colliderData));
				}
				m_pCurrBrick->GetColliderData().clear();

				m_BrickQueue.pop();
			}
			else
			{
				m_pCurrBrick = 0;
			}
			return false;
		case SCommand::UNDO:
			if (!m_OldCommands.empty())
			{
				UndoCommand(m_OldCommands.top());
				m_OldCommands.pop();
			}

			return false;
		case SCommand::NEWCOLLIDER:
		{

			//todo replace with real material!!!
			static ong::Material* g_pMaterial = m_World.createMaterial({ 1.0f, 1.0f, 1.0f });


			ong::ShapePtr shape = m_World.createShape(a_C.shape);

			ong::ColliderDescription cDescr;
			cDescr.shape = shape;
			cDescr.material = g_pMaterial;
			cDescr.transform = ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1));

			ong::Collider* pCollider = m_World.createCollider(cDescr);

			m_pBody->addCollider(pCollider);

			return true;
		}
		case SCommand::BRICKID:
		{
			if (m_pCurrBrick)
				m_pCurrBrick->SetBrickId(a_C.brickID);
			return false;
		}
		case SCommand::TRANSPARENT:
			if (m_pCurrBrick)
				m_pCurrBrick->SetIsTransparent(a_C.transparent);
			return false;
		}
	}

	void CEditorScene::UndoCommand(SCommand& a_C)
	{
		switch (a_C.type)
		{
		case SCommand::NEWCOLLIDER:
			m_World.destroyCollider(m_pBody->getCollider());
			break;
		}
	}


	void CEditorScene::Render(PuRe_IGraphics* a_pGraphics)
	{
		static const PuRe_Vector3F MALE_COLOR = PuRe_Vector3F(0, 0, 1);
		static const PuRe_Vector3F FEMALE_COLOR = PuRe_Vector3F(1, 0.4, 0.7);


		auto drawNub = [&](const TheBrick::SNub& nub){
			ong::Transform t;
			t.p = TheBrick::PuReToOng(nub.Position);
			t.q = ong::QuatFromTwoVectors(TheBrick::PuReToOng(nub.Direction), ong::vec3(0, 1, 0));

			PuRe_Vector3F color = nub.isMale ? MALE_COLOR : FEMALE_COLOR;


			TheBrick::DrawShape(m_NubShape, t, color, m_pCamera, a_pGraphics);
		};

		auto drawPivot = [&](const ong::vec3& pivot)
		{
			ong::Transform t;
			t.p = pivot;
			t.q = ong::Quaternion(ong::vec3(0, 0, 0), 1.0f);

			TheBrick::DrawShape(m_OriginShape, t, PuRe_Vector3F(0, 1, 0), m_pCamera, a_pGraphics);
		};

		PuRe_Color clear = PuRe_Color(0.1f, 0.1f, 0.1f);
		PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

		
		


		a_pGraphics->Begin(clear);
		
		// draw brick
		if (m_pCurrBrick)
		{

			m_pMaterial->Apply();
			m_pCurrBrick->Draw(a_pGraphics, m_pCamera, PuRe_Vector3F(0, 0, 0), PuRe_Vector3F(0, 0, 0), PuRe_Color(0.4, 0.4, 0.4));

			
			for (auto nub : m_pCurrBrick->GetNubs())
			{
				drawNub(nub);
			}

			drawPivot(TheBrick::PuReToOng(m_pCurrBrick->GetPivotOffset()));

		}


		// draw gui stuff
		TheBrick::DrawShape(m_MousePtr, m_MouseTransform,PuRe_Vector3F(1,1,1), m_pCamera, a_pGraphics);

		TheBrick::DrawBody(m_pBody, m_pCamera, a_pGraphics);	

		switch (m_Mode)
		{
		case Mode::NUB:
		{
			drawNub(m_NubPtr);
			break;
		}
		case Mode::ORIGIN:
			drawPivot(m_Pivot);
			break;
		}

		a_pGraphics->End();
	}

	void CEditorScene::Exit()
	{
		m_Polling = false;
		m_InputThread.join();

		delete m_pCamera;
	}

}