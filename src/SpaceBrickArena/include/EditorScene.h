#ifndef _EDITORSCENE_H_
#define _EDITORSCENE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/Brick.h>
#include <TheBrick/Serializer.h>

#include "Space.h"

#include "Editor_Worker.h"
#include "Editor_BrickSupervisor.h"

namespace Editor
{
    class CEditorScene : public PuRe_IScene
    {
    private:
        int m_playerIdx;

        PuRe_SkyBox* m_pSkyBox;
        int textureID;
        PuRe_IMaterial* m_pPostMaterial;
        PuRe_IMaterial* m_pSkyBoxMaterial;
        PuRe_PointLight* m_pPointLight;
        PuRe_DirectionalLight* m_pDirectionalLight;
        PuRe_IMaterial* m_pPointLightMaterial;
        PuRe_IMaterial* m_pDirectionalLightMaterial;

        PuRe_Camera* m_UICamera;
        
        Editor::CBrickSupervisor* m_pBrickSupervisor;
        Editor::CWorker* m_pWorker;

        PuRe_Color m_CurrentColor;

        /// @brief Position for the MouseClick
        ///
        PuRe_Vector2F MouseClickPosition;
        /// @brief Engine's Application
        ///
        PuRe_Application* m_pApplication;
    public:
        /// @brief Constructor to Initialize the MainScene
        ///
        CEditorScene(PuRe_Application* a_pApplication, int a_playerIdx);
    public:
        /// @brief Initializes the scene.
        ///
        /// @param graphics The graphics interface.
        ///
        void Initialize(PuRe_Application* a_pApplication);

        /// @brief Updates the scene.
        ///
        /// @param The graphic interface.
        /// @param The window interface.
        /// @param The input interface.
        /// @param The Time Object
        ///
        /// @returns if it is still running or not
        ///
        bool Update(PuRe_Application* a_pApplication);

        /// @brief Renders the scene.
        ///
        /// @param graphics The graphics interface.
        ///
        void Render(PuRe_Application* a_pApplication);

        /// @brief Exists the scene.
        ///
        void Exit();

    };
}

#endif /* _EDITORSCENE_H_ */