#ifndef _EDITORSCENE_H_
#define _EDITORSCENE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "EditorCamera.h"

namespace Game
{
  class CEditorScene : public PuRe_IScene
  {
  private:
    PuRe_Skydome* m_pSkyDome;
    int32 textureID;
    PuRe_Rendertarget* m_pRenderTarget;
    float32 rot;
    PuRe_Model* m_pModel;
    PuRe_IMaterial* m_pMaterial;
    PuRe_IMaterial* m_pPostMaterial;
    PuRe_Camera* m_pPostCamera;
    /// @brief Position for the MouseClick
    ///
    PuRe_Vector2F MouseClickPosition;
    /// @brief Cameras
    ///
    CEditorCamera* m_pCamera;
    /// @brief Engine's Application
    ///
    PuRe_Application* m_pApplication;
  public:
    /// @brief Constructor to Initialize the MainScene
    ///
      CEditorScene(PuRe_Application* a_pApplication);
  public:
    /// @brief Initializes the scene.
    ///
    /// @param graphics The graphics interface.
    ///
    void Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer);

    /// @brief Updates the scene.
    ///
    /// @param The graphic interface.
    /// @param The window interface.
    /// @param The input interface.
    /// @param The Time Object
    ///
    /// @returns if it is still running or not
    ///
    bool Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer);

    /// @brief Renders the scene.
    ///
    /// @param graphics The graphics interface.
    ///
    void Render(PuRe_IGraphics* a_pGraphics);

    /// @brief Exists the scene.
    ///
    void Exit();

  };
}

#endif /* _EDITORSCENE_H_ */