
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


		ong::Quaternion m_CameraRotation;

		TheBrick::CBrick* m_pCurrBrick;
		std::string m_CurrFileName;

		bool m_Polling;
		std::thread m_InputThread;

		std::queue<std::pair<std::string, TheBrick::CBrick*>> m_BrickQueue;

		ong::World m_World;
		ong::Body* m_pBody;


		ong::Body* m_pMeshBody;
		std::vector<ong::vec3> m_HullPoints;
		bool m_MouseDown;
		PuRe_Vector2F m_RectStart;
		ong::ShapePtr m_Rect;
		ong::ShapePtr m_TempHull;
		
		enum CameraType
		{
			PERSP,
			ORTHO
		} m_CameraType;
		float m_OrthoZoom;

		bool m_MouseValid;

		TheBrick::SNub m_NubPtr;
		ong::vec3 m_Pivot;

		ong::Transform m_MouseTransform;
		ong::ShapePtr m_MousePtr;
		ong::ShapePtr m_NubShape;
		ong::ShapePtr m_OriginShape;
		ong::ShapePtr m_VertexShape;

		TheBrick::CSerializer m_Serializer;

		enum Mode
		{
			NUB,
			ORIGIN,
			HULL
		} m_Mode;



	public:
		CEditorScene(PuRe_Application* a_pApplication);
		~CEditorScene();
		
        void Initialize(PuRe_Application* a_pApplication);

        int Update(PuRe_Application* a_pApplication);

        void Render(PuRe_Application* a_pApplication);

		void Exit();

	};




}