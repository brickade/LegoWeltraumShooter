#include "EditorScene.h"

#include <Windows.h>
#include <wchar.h>
#include <Onager\myMath.h>

#include <TheBrick\DebugDraw.h>

#define _CRT_SECURE_WARNINGS_NO
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

					boxC.x *= TheBrick::CBrick::SEGMENT_WIDTH;
					boxC.z *= TheBrick::CBrick::SEGMENT_WIDTH;
					boxC.y = boxC.y * TheBrick::CBrick::SEGMENT_HEIGHT;

					boxE.x *= TheBrick::CBrick::SEGMENT_WIDTH;
					boxE.z *= TheBrick::CBrick::SEGMENT_WIDTH;
					boxE.y *= TheBrick::CBrick::SEGMENT_HEIGHT;

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

			}
		}
	}


	void CEditorScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
	{
		PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
		
		m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/editor/default");
		
		m_pPostCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
		m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
		m_pRenderTarget = a_pGraphics->CreateRendertarget(m_pPostMaterial);

		m_pCamera = new PuRe_Camera(PuRe_Vector2F(gdesc.ResolutionWidth, gdesc.ResolutionHeight), PuRe_Camera_Perspective);
		m_pCamera->SetFoV(45.0f);


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


			//new brick from obj
			if (fileType != 0 && wcscmp(fileType, L".obj") == 0)
			{

				mbstate_t conversionState;

				char fileName[FILENAME_MAX];
				const wchar_t* wideFileName = findFileData.cFileName;


				wcsrtombs(fileName, &wideFileName, FILENAME_MAX, &conversionState);

				char fileDir[FILENAME_MAX];
				strcpy_s(fileDir, dir);
				strcat_s(fileDir, fileName);

				printf("%s \n", fileDir);
				PuRe_Model* pModel = new PuRe_Model(a_pGraphics, m_pMaterial, fileDir);
				

				if (pModel != 0)
				{
					TheBrick::CBrick* newBrick = new TheBrick::CBrick(pModel);
					m_BrickQueue.push(newBrick);
				}
			}
			
			if (!FindNextFile(hFind, &findFileData))
			{
				int error = GetLastError();
				break;
			}
		}
		FindClose(hFind);



		ong::BodyDescription bodyDescr;
		bodyDescr.linearMomentum = ong::vec3(0, 0, 0);
		bodyDescr.angularMomentum = ong::vec3(0, 0, 0);
		bodyDescr.transform = ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1));
		bodyDescr.type = ong::BodyType::Static;

		m_pBody = m_World.createBody(bodyDescr);
		
	}


	bool CEditorScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
	{
		if (a_pInput->GetCursorLock())
			a_pInput->UnLockCursor();


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
				delete m_pCurrBrick;

			if (!m_BrickQueue.empty())
			{
				m_pCurrBrick = m_BrickQueue.front()->CreateInstance();
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
		PuRe_Color clear = PuRe_Color(0.1f, 0.1f, 0.1f);
		PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

		
		


		a_pGraphics->Begin(clear);
		
		m_pMaterial->Apply();
		m_pMaterial->SetVector3(PuRe_Vector3F(0.4, 0.4, 0.4), "brickColor");
		if (m_pCurrBrick)
			m_pCurrBrick->Draw(a_pGraphics, m_pCamera);

		TheBrick::DrawBody(m_pBody, m_pCamera, a_pGraphics);	

		a_pGraphics->End();
	}

	void CEditorScene::Exit()
	{
		m_Polling = false;
		m_InputThread.join();

		delete m_pCamera;
	}

}