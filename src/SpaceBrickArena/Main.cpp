#include <PuReEngine/Platform.h>
#include <PuReEngine/Core.h>
// Application specific includes
#include "include/MainScene.h"



int32 PURE_MAIN
{
    //Memory Leak detection
    //static unsigned int breakId = 162;
    //_CrtSetBreakAlloc(breakId);

    PuRe_IPlatform* pplatform = PuRe_CreatePlatform();
    PuRe_ApplicationDescription description;
    description.Graphics.ResolutionWidth = 1920;
    description.Graphics.ResolutionHeight = 1080;
    description.Graphics.Module = PuRe_GraphicsModuleType::OpenGL;
    description.Window.Title = "SpaceBrickArena - Development";
    description.Window.Width = 1920;
    description.Window.Height = 1080;
    description.Window.X = 100;
    description.Window.Y = 10;
    description.Window.DisplaySetting = PuRe_DisplaySetting::Windowed;
    description.Window.ShowCursor = false;
    PuRe_Application* papplication = new PuRe_Application(pplatform, description);
    Game::CMainScene scene(papplication);

    papplication->Run(&scene);

    SAFE_DELETE(papplication);
    SAFE_DELETE(pplatform);
    //Print memory leak in output
    //_CrtDumpMemoryLeaks();
}