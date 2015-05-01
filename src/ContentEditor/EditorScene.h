
#pragma once

// Framework specific includess
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/TheBrick.h>
#include <Onager\World.h>

#include "CommandQueue.h"


namespace Content
{
	
	class CEditorScene : public PuRe_IScene
	{
	private:

		void getInput();
		
		CCommandQueue m_Queue;

		PuRe_Application* m_pApplication;

		TheBrick::CBrick* m_pCurrBrick;
		PuRe_IMaterial* m_pMaterial;

		PuRe_Camera* m_pCamera;

		bool m_Polling;
		std::thread m_InputThread;

		std::queue<TheBrick::CBrick*> m_BrickQueue;


		ong::World m_world;
		ong::Collider* m_pCollider;
		ong::Body* m_pBody;

	public:
		CEditorScene(PuRe_Application* a_pApplication);
		~CEditorScene();
		
		void Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer);

		bool Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer);

		void Render(PuRe_IGraphics* a_pGraphics);

		void Exit();

	};




}