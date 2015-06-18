#include "EditorScene.h"

#include <Windows.h>
#include <wchar.h>
#include <Onager\myMath.h>

#include <TheBrick\DebugDraw.h>
#include <TheBrick\Conversion.h>

#include <PuReEngine\Camera.h>
#include <stdio.h>

#include "ParseModel.h"

namespace Content
{

	static const int SELECTED = 1;
	static const int UNSELECTED = 0;


	CEditorScene::CEditorScene(PuRe_Application* a_pApplication)
		: m_pApplication(a_pApplication),
		m_CameraType(CameraType::PERSP),
		m_OrthoZoom(10.0f)
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
					c.skip = 0;
					m_Queue.push(c);
				}
				else if (strcmp(buffer, "skip") == 0)
				{
					SCommand c;
					c.type = SCommand::NEXT;
					c.skip = 1;
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


    void CEditorScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
		printf("type \"help\" for help\n");

        PuRe_GraphicsDescription gdesc = graphics->GetDescription();
		
        m_pMaterial = graphics->LoadMaterial("../data/effects/editor/default");
		
		m_pPostCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
        m_pPostMaterial = graphics->LoadMaterial("../data/effects/Post/default");

        m_pCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective);
		m_pCamera->SetFoV(45.0f);
		m_pCamera->Update();
		m_CameraRotation = ong::Quaternion(ong::vec3(0, 0, 0), 1);
		m_pCurrBrick = 0;

		m_Polling = true;
		m_InputThread = std::thread(&CEditorScene::getInput, this);

		m_MouseDown = false;



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
                    PuRe_Model* pModel = new PuRe_Model(graphics, fileDir);


					if (pModel != 0)
					{
						char modelDir[FILENAME_MAX] = { 0 };
						strcat(modelDir, "..\\data\\models\\");
						strcat(modelDir, fileName);


                        TheBrick::CBrick* newBrick = new TheBrick::CBrick(*pModel, *m_pMaterial);
						newBrick->SetMaterial(m_pMaterial);
						newBrick->SetModelPath(modelDir);
						m_BrickQueue.push(std::make_pair(fileName, newBrick));
					}
				}
				else if (wcscmp(fileType, L".brick") == 0)
				{

                    TheBrick::CBrick* newBrick = new TheBrick::CBrick(*m_pMaterial);

					if (m_Serializer.OpenRead(fileDir))
					{
                        newBrick->Deserialize(m_Serializer, *graphics, m_World);
						m_Serializer.Close();

						const char* modelDir = newBrick->GetModelPath();

                        PuRe_Model* pModel = new PuRe_Model(graphics, modelDir);
						
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

		ong::ShapeDescription vertex;
		vertex.shapeType = ong::ShapeType::SPHERE;
		vertex.sphere.c = ong::vec3(0, 0, 0);
		vertex.sphere.r = 0.01;

		m_VertexShape = m_World.createShape(vertex);
		

		ong::BodyDescription bdescr;
		bdescr.type = ong::BodyType::Static;
		bdescr.linearMomentum = ong::vec3(0, 0, 0);
		bdescr.angularMomentum = ong::vec3(0, 0, 0);
		bdescr.transform.p = ong::vec3(0, 0, 0);
		bdescr.transform.q = ong::Quaternion(ong::vec3(0, 0, 0), 1);

		m_pMeshBody = m_World.createBody(bdescr);

		m_MouseValid = false;

	}


    int CEditorScene::Update(PuRe_Application* a_pApplication)
    {
        PuRe_IInput* input = a_pApplication->GetInput();
        PuRe_IWindow* window = a_pApplication->GetWindow();
        if (input->GetCursorLock())
            input->UnLockCursor();


		PuRe_WindowDescription wDescr = window->GetDescription();
		PuRe_GraphicsDescription gDescr = a_pApplication->GetGraphics()->GetDescription();

		if (input->KeyIsPressed(input->Alt))
		{
			if (input->MouseIsPressed(input->LeftClick))
			{
				//PuRe_Vector3F rot = PuRe_Vector3F(0, 1, 0) *  (input->GetRelativeMousePosition().X / 4.0f);

				//m_pCamera->Rotate(rot.X, rot.Y, rot.Z);

				//rot = m_pCamera->GetSide() *  (-input->GetRelativeMousePosition().Y / 4.0f);

				//m_pCamera->Rotate(rot.X, rot.Y, rot.Z);


				m_CameraRotation = ong::QuatFromAxisAngle(ong::vec3(0, 1, 0), input->GetRelativeMousePosition().X / 4.0f * ong_PI / 180.0f) * m_CameraRotation;
				m_CameraRotation = ong::QuatFromAxisAngle(ong::rotate(ong::vec3(1, 0, 0), m_CameraRotation), input->GetRelativeMousePosition().Y / 4.0f * ong_PI / 180.0f) * m_CameraRotation;
				
				m_pCamera->SetRotation(TheBrick::OngToPuRe(m_CameraRotation));
			}
			if (input->MouseIsPressed(input->MiddleClick))
			{
				m_pCamera->Move(PuRe_Vector3F(-input->GetRelativeMousePosition().X / 80.0f, input->GetRelativeMousePosition().Y / 80.0f, 0.0f));
			}
			if (input->GetMouseScroll())
			{
				if (m_CameraType == CameraType::PERSP)
				{
					m_pCamera->Move(PuRe_Vector3F(0, 0, input->GetMouseScroll()));
				}
				else
				{
					
					if (input->GetMouseScroll() < 0)
					{
						m_OrthoZoom *= 1.1;
					}
					else if ( input->GetMouseScroll() > 0)
					{
						m_OrthoZoom *= 0.9;
					}
					
					m_pCamera->SetProjection(PuRe_MatrixF::ProjectionOrthogonalLH(m_OrthoZoom, gDescr.ResolutionHeight / (float)gDescr.ResolutionWidth * m_OrthoZoom, 0.1, 100.0f));
					m_pCamera->Update();
					
				}
			}
		}
		else
		{
			if (m_MouseValid)
			{
				if (input->MousePressed(input->LeftClick))
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
				else if (input->MousePressed(input->RightClick))
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

			if (m_Mode == Mode::HULL)
			{
				if (input->MousePressed(input->LeftClick))
				{
					m_MouseDown = true;
					m_RectStart = input->GetAbsoluteMousePosition();
				}
				else if (m_MouseDown && !input->MouseIsPressed(input->LeftClick))
				{
					m_MouseDown = false;

					int mode = 0;
					// select vertices
					if (input->KeyIsPressed(input->Shift))
					{
						mode = 1;
					}
					else if (!input->KeyIsPressed(input->Ctrl))
					{
						for (auto c = m_pMeshBody->getCollider(); c != nullptr; c = c->getNext())
						{
							c->setUserData((void*)&UNSELECTED);
						}
						m_HullPoints.clear();
					}


					ong::ShapeQueryCallBack callbacks[2]
					{
						[](ong::Collider* other, void* userData)->bool // add selection
					{
						if (other->getUserData() != &SELECTED)
						{
							std::vector<ong::vec3>* hullPoints = (std::vector<ong::vec3>*)userData;
							hullPoints->push_back(other->getTransform().p);
							other->setUserData((void*)&SELECTED);
						}
						return true;
					},
						[](ong::Collider* other, void* userData)->bool // remove selection
					{
						if (other->getUserData() == &SELECTED)
						{
							std::vector<ong::vec3>* hullPoints = (std::vector<ong::vec3>*)userData;
							for (int i = 0; i < hullPoints->size(); ++i)
							{
								if ((*hullPoints)[i] == other->getTransform().p)
								{
									(*hullPoints)[i] = hullPoints->back();
									hullPoints->pop_back();
								}
							}
							other->setUserData((void*)&UNSELECTED);
						}
						return true;
					}
					};


					if (!!m_Rect)
					{
						m_pMeshBody->queryShape(m_Rect, ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1)), callbacks[mode], &m_HullPoints);
					}

					m_World.destroyShape(m_Rect);
					m_Rect = ong::ShapePtr();


					if (!!m_TempHull)
						m_World.destroyShape(m_TempHull);

					if (m_HullPoints.size() > 3)
					{

						ong::ShapeDescription shapeDescr;
						shapeDescr.constructionType = ong::ShapeConstruction::HULL_FROM_POINTS;
						shapeDescr.hullFromPoints.points = m_HullPoints.data();
						shapeDescr.hullFromPoints.numPoints = m_HullPoints.size();

						m_TempHull = m_World.createShape(shapeDescr);
					}
					else
					{
						m_TempHull = ong::ShapePtr();
					}

				}

				if (input->KeyPressed(input->Enter))
				{
					if (!!m_TempHull)
					{
						SCommand c;
						c.type = SCommand::NEWCOLLIDER;
						c.shape.type = ong::ShapeType::HULL;
						c.shape.hull = *m_TempHull.toHull();

						if (ProcessCommand(c))
							m_OldCommands.push(c);

						m_World.destroyShape(m_TempHull);
						m_TempHull = ong::ShapePtr();
					}
				}
			}


			if (input->KeyPressed(input->Q))
				m_Mode = Mode::NUB, m_NubPtr.isMale = true;
			else if (input->KeyPressed(input->W))
				m_Mode = Mode::NUB, m_NubPtr.isMale = false;
			else if (input->KeyPressed(input->E))
				m_Mode = Mode::ORIGIN;
			else if (input->KeyPressed(input->R))
				m_Mode = Mode::HULL;
		}



		
		//camera
		if (input->KeyPressed(input->One))
		{
			m_CameraType = CameraType::PERSP;
			m_pCamera->SetProjection(PuRe_MatrixF::ProjectionPerspectiveFovLH(45.0f, gDescr.ResolutionWidth/(float) gDescr.ResolutionHeight, 0.1f, 100.0f));
			m_pCamera->SetPosition(PuRe_Vector3F(-2, 2, -2));
			m_pCamera->SetRotation(PuRe_Vector3F(0, 0, 0));
			m_pCamera->Update();
		}
		else if (input->KeyPressed(input->Two))
		{
			m_CameraType = CameraType::ORTHO;
			m_pCamera->SetProjection(PuRe_MatrixF::ProjectionOrthogonalLH(m_OrthoZoom, gDescr.ResolutionHeight / (float)gDescr.ResolutionWidth * m_OrthoZoom, 0.1, 100.0f));
			if (input->KeyIsPressed(input->Ctrl))
			{
				m_pCamera->SetPosition(PuRe_Vector3F(0, -50, 0));
				m_pCamera->SetRotation(PuRe_Vector3F(-90, 0, 0));
			}
			else
			{
				m_pCamera->SetPosition(PuRe_Vector3F(0, 50, 0));
				m_pCamera->SetRotation(PuRe_Vector3F(90, 0, 0));
			}
			m_pCamera->Update();

		}
		else if (input->KeyPressed(input->Three))
		{
			m_CameraType = CameraType::ORTHO;
			m_pCamera->SetProjection(PuRe_MatrixF::ProjectionOrthogonalLH(m_OrthoZoom, gDescr.ResolutionHeight / (float)gDescr.ResolutionWidth * m_OrthoZoom, 0.1, 100.0f));
			if (input->KeyIsPressed(input->Ctrl))
			{
				m_pCamera->SetPosition(PuRe_Vector3F(0, 0, 50));
				m_pCamera->SetRotation(PuRe_Vector3F(0, 180, 0));
			}
			else
			{
				m_pCamera->SetPosition(PuRe_Vector3F(0, 0, -50));
				m_pCamera->SetRotation(PuRe_Vector3F(0, 0, 0));
			}
			m_pCamera->Update();
		}
		else if (input->KeyPressed(input->Four))
		{
			m_CameraType = CameraType::ORTHO;
			m_pCamera->SetProjection(PuRe_MatrixF::ProjectionOrthogonalLH(m_OrthoZoom, gDescr.ResolutionHeight / (float)gDescr.ResolutionWidth * m_OrthoZoom, 0.1, 100.0f));
			if (input->KeyIsPressed(input->Ctrl))
			{
				m_pCamera->SetPosition(PuRe_Vector3F(-50, 0, 0));
				m_pCamera->SetRotation(PuRe_Vector3F(0, 90, 0));
			}
			else 
			{
				m_pCamera->SetPosition(PuRe_Vector3F(50, 0, 0));
				m_pCamera->SetRotation(PuRe_Vector3F(0, -90, 0));
			}
			m_pCamera->Update();
		}

		//selectionRect
		if (m_Mode == Mode::HULL && m_MouseDown)
		{
			PuRe_Vector2F rectStart, rectEnd;
			rectStart.X = (2.0f*m_RectStart.X) / wDescr.Width - 1.0f;
			rectStart.Y = 1.0f - (2.0f * m_RectStart.Y) / wDescr.Height;

			rectEnd.X = (2.0f*input->GetAbsoluteMousePosition().X) / wDescr.Width - 1.0f;
			rectEnd.Y = 1.0f - (2.0f * input->GetAbsoluteMousePosition().Y) / wDescr.Height;

			//rectStart.X = -1.0;
			//rectStart.Y = 1.0;
			//rectEnd.X = 1.0;
			//rectEnd.Y = 1.0;

			if (abs(rectStart.X - rectEnd.X) >= 0.01f && abs(rectStart.Y - rectEnd.Y) >= 0.01f)
			{

				//PuRe_MatrixF inverseProjView = PuRe_MatrixF::Invert(m_pCamera->GetProjection())*PuRe_MatrixF::Invert(m_pCamera->GetView());
				PuRe_MatrixF inverseProjView = m_pCamera->GetInvertViewProjection();


				ong::vec3 box[8];

				if (m_CameraType == CameraType::PERSP)
				{
					PuRe_Vector3F corners[8];
					corners[0] = PuRe_Vector3F::Normalize(PuRe_Vector4F(rectStart.X, rectStart.Y, 1.0f, 1.0f) * inverseProjView);
					corners[1] = PuRe_Vector3F::Normalize(PuRe_Vector4F(rectEnd.X, rectStart.Y, 1.0f, 1.0f) * inverseProjView);
					corners[2] = PuRe_Vector3F::Normalize(PuRe_Vector4F(rectEnd.X, rectEnd.Y, 1.0f, 1.0f) * inverseProjView);
					corners[3] = PuRe_Vector3F::Normalize(PuRe_Vector4F(rectStart.X, rectEnd.Y, 1.0f, 1.0f) * inverseProjView);

					box[0] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[0] * 0.1f);
					box[1] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[1] * 0.1f);
					box[2] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[2] * 0.1f);
					box[3] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[3] * 0.1f);
					box[4] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[0] * 100.0f);
					box[5] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[1] * 100.0f);
					box[6] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[2] * 100.0f);
					box[7] = TheBrick::PuReToOng(m_pCamera->GetPosition() + corners[3] * 100.0f);
				}
				else
				{
					box[0] = TheBrick::PuReToOng(PuRe_Vector4F(rectStart.X, rectStart.Y, 0.0f, 1.0f)  * inverseProjView);
					box[1] = TheBrick::PuReToOng(PuRe_Vector4F(rectEnd.X, rectStart.Y, 0.0f, 1.0f)  * inverseProjView);
					box[2] = TheBrick::PuReToOng(PuRe_Vector4F(rectEnd.X, rectEnd.Y, 0.0f, 1.0f)  * inverseProjView);
					box[3] = TheBrick::PuReToOng(PuRe_Vector4F(rectStart.X, rectEnd.Y, 0.0f, 1.0f)  * inverseProjView);
					box[4] = TheBrick::PuReToOng(PuRe_Vector4F(rectStart.X, rectStart.Y, 1.0f, 1.0f) * inverseProjView);
					box[5] = TheBrick::PuReToOng(PuRe_Vector4F(rectEnd.X, rectStart.Y, 1.0f, 1.0f) * inverseProjView);
					box[6] = TheBrick::PuReToOng(PuRe_Vector4F(rectEnd.X, rectEnd.Y, 1.0f, 1.0f) * inverseProjView);
					box[7] = TheBrick::PuReToOng(PuRe_Vector4F(rectStart.X, rectEnd.Y, 1.0f, 1.0f) * inverseProjView);
				}

				ong::ShapeDescription shapeDescr;
				shapeDescr.constructionType = ong::ShapeConstruction::HULL_FROM_POINTS;
				shapeDescr.hullFromPoints.points = box;
				shapeDescr.hullFromPoints.numPoints = 8;

				if (!!m_Rect)
					m_World.destroyShape(m_Rect);

				m_Rect = m_World.createShape(shapeDescr);
			}
		}


		// ray cast
		PuRe_Vector3F mousePos;
		mousePos.X = (2.0f * input->GetAbsoluteMousePosition().X) / wDescr.Width - 1.0f;
		mousePos.Y = 1.0f - (2.0f * input->GetAbsoluteMousePosition().Y) / wDescr.Height;
		mousePos.Z = 0.0f;

		PuRe_Vector4F rayV = PuRe_Vector4F(mousePos.X, mousePos.Y, 1.0f, 1.0f) * PuRe_MatrixF::Invert(m_pCamera->GetProjection());
		PuRe_Vector4F rayW = rayV*PuRe_MatrixF::Invert(m_pCamera->GetView());

		ong::vec3 rayDir = ong::normalize(ong::vec3(rayW.X, rayW.Y, rayW.Z));
		ong::vec3 rayO = ong::vec3(m_pCamera->GetPosition().X, m_pCamera->GetPosition().Y, m_pCamera->GetPosition().Z);


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
			m_NubPtr.Position.X = (float)(floor(m_MouseTransform.p.x / w) + 0.5) * w - (rayResult.normal.x * 0.5f*w);
            m_NubPtr.Position.Y = (float)(floor(m_MouseTransform.p.y / h) + 0.5) * h - (rayResult.normal.y * 0.5f*h);
            m_NubPtr.Position.Z = (float)(floor(m_MouseTransform.p.z / w) + 0.5) * w - (rayResult.normal.z * 0.5f*w);

			m_NubPtr.Direction = TheBrick::OngToPuRe(rayResult.normal);

			break;
		case Mode::ORIGIN:
            m_Pivot.x = (float)(floor(m_MouseTransform.p.x / w) + 0.5) * w - (rayResult.normal.x * 0.5f*w);
            m_Pivot.y = (float)(floor(m_MouseTransform.p.y / h) + 0.5) * h - (rayResult.normal.y * 0.5f*h);
            m_Pivot.z = (float)(floor(m_MouseTransform.p.z / w) + 0.5) * w - (rayResult.normal.z * 0.5f*w);
			break;
		}




		while (!m_Queue.isEmpty())
		{
			SCommand c = m_Queue.pop();
			if(ProcessCommand(c))
				m_OldCommands.push(c);
		}


		return 1;
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
				// write collider
				if (!a_C.skip && m_Serializer.OpenWrite(m_CurrFileName.c_str()))
				{
					m_pCurrBrick->Serialize(m_Serializer);
					m_Serializer.Close();
				}

				// clear mesh
				for (ong::Collider* c = m_pMeshBody->getCollider(); c != nullptr; c = c->getNext())
				{
					m_World.destroyCollider(c);
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

				// generate mesh body
				ong::ColliderDescription colliderDescr;
				colliderDescr.isSensor = true;
				colliderDescr.material = 0;
				colliderDescr.shape = m_VertexShape;
				colliderDescr.transform.q = ong::Quaternion(ong::vec3(0, 0, 0), 1);

				
				std::vector<ong::vec3> vertices;
				parseModel(m_pCurrBrick->GetModelPath(), vertices);
				
				for (auto v : vertices)
				{
					colliderDescr.transform.p = v;
					ong::Collider* pCollider = m_World.createCollider(colliderDescr);
					pCollider->setUserData((void*)&UNSELECTED);
					m_pMeshBody->addCollider(pCollider);
				}
				
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
            cDescr.isSensor = false;
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
				m_pCurrBrick->SetIsTransparent(a_C.transparent == 1);
			return false;
		}
        return false;
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


	void CEditorScene::Render(PuRe_Application* a_pApplication)
	{
		static const PuRe_Vector3F MALE_COLOR = PuRe_Vector3F(0, 0, 1);
		static const PuRe_Vector3F FEMALE_COLOR = PuRe_Vector3F(1.0f, 0.4f, 0.7f);
		PuRe_IGraphics* graphics = a_pApplication->GetGraphics();

		auto drawNub = [&](const TheBrick::SNub& nub){
			ong::Transform t;
			t.p = TheBrick::PuReToOng(nub.Position);
			t.q = ong::QuatFromTwoVectors(TheBrick::PuReToOng(nub.Direction), ong::vec3(0, 1, 0));

			PuRe_Vector3F color = nub.isMale ? MALE_COLOR : FEMALE_COLOR;


			TheBrick::DrawShape(m_NubShape, t, color, m_pCamera, graphics);
		};

		auto drawPivot = [&](const ong::vec3& pivot)
		{
			ong::Transform t;
			t.p = pivot;
			t.q = ong::Quaternion(ong::vec3(0, 0, 0), 1.0f);

			TheBrick::DrawShape(m_OriginShape, t, PuRe_Vector3F(0, 1, 0), m_pCamera, graphics);
		};

		PuRe_Color clear = PuRe_Color(0.1f, 0.1f, 0.1f);
		PuRe_GraphicsDescription gdesc = graphics->GetDescription();


		graphics->Clear(clear);
		PuRe_BoundingBox box;
		box.m_Position = PuRe_Vector3F();
		box.m_Size = PuRe_Vector3F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight, 0.0f);
		graphics->Begin(box);

		// draw brick
		if (m_pCurrBrick)
		{

			m_pMaterial->Apply();
			m_pCurrBrick->GetModel()->Draw(m_pCamera, m_pMaterial, PuRe_Primitive::Triangles);


			for (auto nub : m_pCurrBrick->GetNubs())
			{
				drawNub(nub);
			}

			drawPivot(TheBrick::PuReToOng(m_pCurrBrick->GetPivotOffset()));

		}


		// draw gui stuff
		TheBrick::DrawShape(m_MousePtr, m_MouseTransform, PuRe_Vector3F(1, 1, 1), m_pCamera, graphics);

		TheBrick::DrawBody(m_pBody, m_pCamera, graphics);

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
		case Mode::HULL:
			//for (ong::Collider* c = m_pMeshBody->getCollider(); c != nullptr; c = c->getNext())
			//{
			//	PuRe_Vector3F color;
			//	if (c->getUserData() == &SELECTED)
			//	{
			//		color = PuRe_Vector3F(1, 1, 0);
			//		TheBrick::DrawShape(m_VertexShape, c->getTransform(), color, m_pCamera, graphics);
			//	}
			//	else
			//		color = PuRe_Vector3F(1, 1, 0);
			//}

			TheBrick::DrawShape(m_TempHull, ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1)), PuRe_Vector3F(1, 1, 1), m_pCamera, graphics);
			TheBrick::DrawShape(m_Rect, ong::Transform(ong::vec3(0,0,0), ong::Quaternion(ong::vec3(0,0,0), 1)), PuRe_Vector3F(0.5,0.5, 0.5), m_pCamera, graphics);


		}
	

        graphics->End();
	}

	void CEditorScene::Exit()
	{
		m_Polling = false;
		m_InputThread.join();

		delete m_pCamera;
	}

}