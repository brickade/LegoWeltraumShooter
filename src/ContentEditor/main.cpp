
#include <PuReEngine/Platform.h>
#include <PuReEngine/Core.h>
// Application specific includes
#include "EditorScene.h"



int main()
{



	using namespace Content;
		
	PuRe_IPlatform* pPlatform = PuRe_CreatePlatform();
	PuRe_ApplicationDescription description;
	description.Graphics.ResolutionWidth = 800;
	description.Graphics.ResolutionHeight = 600;
	description.Graphics.Module = PuRe_GraphicsModuleType::OpenGL;
	description.Window.Title = "Content - Editor";
	description.Window.Width = 800;
	description.Window.Height = 600;
	description.Window.X = 200;
	description.Window.Y = 200;
	description.Window.DisplaySetting = PuRe_DisplaySetting::Windowed;
	description.Window.ShowCursor = true;
	description.Window.CursorInWindow = false;
	PuRe_Application* pApplication = new PuRe_Application(pPlatform, description);


	CEditorScene scene(pApplication);

	pApplication->Run(&scene);

	SAFE_DELETE(pApplication);
	SAFE_DELETE(pPlatform);
}