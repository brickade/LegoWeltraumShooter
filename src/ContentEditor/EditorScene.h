
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
		bool ProcessCommand(SCommand& a_C);
		void UndoCommand(SCommand& a_C);


		CCommandQueue m_Queue;
		std::stack<SCommand> m_OldCommands;

		PuRe_Application* m_pApplication;
		PuRe_IMaterial* m_pMaterial;
		PuRe_Camera* m_pCamera;
		PuRe_Camera* m_pPostCamera;
		PuRe_IMaterial* m_pPostMaterial;
		PuRe_Rendertarget* m_pRenderTarget;

		TheBrick::CBrick* m_pCurrBrick;
		std::string m_CurrFileName;

		bool m_Polling;
		std::thread m_InputThread;

		std::queue<std::pair<std::string, TheBrick::CBrick*>> m_BrickQueue;

		ong::World m_World;
		ong::Body* m_pBody;

		bool m_MouseValid;

		TheBrick::SNub m_NubPtr;
		ong::vec3 m_Pivot;

		ong::Transform m_MouseTransform;
		ong::ShapePtr m_MousePtr;
		ong::ShapePtr m_NubShape;
		ong::ShapePtr m_OriginShape;

		TheBrick::CSerializer m_Serializer;

		enum Mode
		{
			NUB,
			ORIGIN
		} m_Mode;

	public:
		CEditorScene(PuRe_Application* a_pApplication);
		~CEditorScene();
		
        void Initialize(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer);

		bool Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer);

		void Render(PuRe_IGraphics* a_pGraphics);

		void Exit();

	};




}