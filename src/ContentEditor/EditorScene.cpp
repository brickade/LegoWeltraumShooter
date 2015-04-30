#include "EditorScene.h"

#include <Windows.h>
#include <wchar.h>
#include <Onager\myMath.h>

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
			while (scanf("%s", buffer))
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
	
			}
		}
	}


	void CEditorScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
	{
		PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
		
		m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/default/default");

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
		wcscat(currentPath, L"\\..\\data\\newContent\\*");

		char* dir = "..\\data\\newContent\\";

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
				strcpy(fileDir, dir);
				strcat(fileDir, fileName);

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

			switch (c.type)
			{
			case SCommand::TEST:
				printf("test\n");
				break;
			case SCommand::NEXT:

				if (m_pCurrBrick)
					delete m_pCurrBrick;

				if (!m_BrickQueue.empty())
				{
					m_pCurrBrick = m_BrickQueue.front();
					m_BrickQueue.pop();
				}
				else
				{
					m_pCurrBrick = 0;
				}
			}

		}


		return false;
	}
		
	void CEditorScene::Render(PuRe_IGraphics* a_pGraphics)
	{
		PuRe_Color clear = PuRe_Color(0.1f, 0.1f, 0.1f);
		
		a_pGraphics->Begin(clear);
		//m_pMaterial->Apply();
		if (m_pCurrBrick)
			m_pCurrBrick->Draw(a_pGraphics, m_pCamera, PuRe_Vector3F(0,0,0), PuRe_Vector3F(1,1,1), PuRe_Vector3F(0,0,0));
		
		a_pGraphics->End();
	}

	void CEditorScene::Exit()
	{
		m_Polling = false;
		m_InputThread.join();

		delete m_pCamera;
	}

}