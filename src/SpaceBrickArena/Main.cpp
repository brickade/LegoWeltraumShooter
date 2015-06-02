#include <PuReEngine/Platform.h>
#include <PuReEngine/Core.h>
// Application specific includes
#include "include/INIReader.h"
#include "include/MainScene.h"



int PURE_MAIN
{
    //Memory Leak detection
    //static unsigned int breakId = 162;
    //_CrtSetBreakAlloc(breakId);

    std::string userpath = std::getenv("USERPROFILE");
    userpath += "\\Documents\\SpaceBrickArena\\";
    //Create Directory if it doesn't exist
    #ifdef _WIN32
        CreateDirectory(userpath.c_str(), NULL);
    #else
        mkdir(a_pDirectoryPath, 777);
    #endif

    userpath += "Options.ini";
    //Create IniFile
    Game::CIniReader::SetPath(userpath.c_str());

    PuRe_IPlatform* pplatform = PuRe_CreatePlatform();
    PuRe_ApplicationDescription description;
    description.Graphics.ResolutionWidth = std::stoi(Game::CIniReader::Instance()->GetValue("ResolutionWidth"));
    description.Graphics.ResolutionHeight = std::stoi(Game::CIniReader::Instance()->GetValue("ResolutionHeight"));
    description.Graphics.Module = PuRe_GraphicsModuleType::OpenGL;
    description.Window.Title = "SpacebrickArena - Development";
    description.Window.Width = std::stoi(Game::CIniReader::Instance()->GetValue("WindowWidth"));
    description.Window.Height = std::stoi(Game::CIniReader::Instance()->GetValue("WindowHeight"));
    description.Window.X = 100;
    description.Window.Y = 200;
    std::string DisplayMode = Game::CIniReader::Instance()->GetValue("DisplayMode");
    if (DisplayMode == "Windowed")
        description.Window.DisplaySetting = PuRe_DisplaySetting::Windowed;
    else if (DisplayMode == "FullscreenWindowed")
        description.Window.DisplaySetting = PuRe_DisplaySetting::FullscreenWindowed;
    else
        description.Window.DisplaySetting = PuRe_DisplaySetting::Fullscreen;
    description.Window.ShowCursor = true;
    description.Window.CursorInWindow = false;
    description.Window.LockCursor = false;
    PuRe_Application* papplication = new PuRe_Application(pplatform, description);
    sba::CMainScene scene(papplication);

    papplication->Run(&scene);

    SAFE_DELETE(papplication);
    SAFE_DELETE(pplatform);
    //Print memory leak in output
    //_CrtDumpMemoryLeaks();
}