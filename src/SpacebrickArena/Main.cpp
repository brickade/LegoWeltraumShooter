#include <PuReEngine/Platform.h>
#include <PuReEngine/Core.h>
// Application specific includes

#include "include/INIReader.h"
#include "include/MainScene.h"


#include "Onager\Profiler.h"

int PURE_MAIN
{
    //Memory Leak detection
    //static unsigned int breakId = 162;
    //_CrtSetBreakAlloc(breakId);
    

    //for floating point determinism
    _controlfp(_PC_24, _MCW_PC);
    _controlfp(_RC_NEAR, _MCW_RC);


    PuRe_IPlatform* pplatform = PuRe_CreatePlatform();
    PuRe_ApplicationDescription description;
    description.Graphics.ResolutionWidth = std::stoi(sba_Options->GetValue("ResolutionWidth"));
    description.Graphics.ResolutionHeight = std::stoi(sba_Options->GetValue("ResolutionHeight"));
    description.Graphics.Module = PuRe_GraphicsModuleType::OpenGL;
    description.Window.Title = "SpacebrickArena - Development";
    description.Window.Width = std::stoi(sba_Options->GetValue("WindowWidth"));
    description.Window.Height = std::stoi(sba_Options->GetValue("WindowHeight"));
    description.Window.X = 100;
    description.Window.Y = 100;
    std::string DisplayMode = sba_Options->GetValue("DisplayMode");
    if (DisplayMode == "Windowed")
        description.Window.DisplaySetting = PuRe_DisplaySetting::Windowed;
    else if (DisplayMode == "FullscreenWindowed")
        description.Window.DisplaySetting = PuRe_DisplaySetting::FullscreenWindowed;
    else
        description.Window.DisplaySetting = PuRe_DisplaySetting::Fullscreen;
    description.Window.ShowCursor = false;
    description.Window.CursorInWindow = false;
    description.Window.LockCursor = true; //Breaks Editor Mouse Functionality if changed
    PuRe_Application* papplication = new PuRe_Application(pplatform, description);
    sba::CMainScene scene(papplication);

    papplication->Run(&scene);

    SAFE_DELETE(papplication);
    SAFE_DELETE(pplatform);
    SAFE_DELETE(sba_Options);
    //Print memory leak in output
    //_CrtDumpMemoryLeaks();
}