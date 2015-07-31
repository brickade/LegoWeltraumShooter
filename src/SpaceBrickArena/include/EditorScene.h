#ifndef _EDITORSCENE_H_
#define _EDITORSCENE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/Brick.h>
#include <TheBrick/Serializer.h>

#include "Space.h"

#include "Editor_Worker.h"
#include "Editor_ShipHandler.h"
#include "Editor_BrickSupervisor.h"
#include "Editor_ColorFields.h"
#include "Editor_ModeMenu.h"
#include "UIElementFader.h"

namespace Editor
{
    class CEditorScene : public PuRe_IScene
    {
        struct EEditorState
        {
            enum Type
            {
                SelectShip,
                EditShip
            };
        };

    private:
        int m_PlayerIdx;

        EEditorState::Type m_State;
        
        PuRe_IMaterial* m_pPostMaterial;
        PuRe_IMaterial* m_pUIMaterial;

        PuRe_PointLight* m_pPointLight;
        PuRe_IMaterial* m_pPointLightMaterial;
        PuRe_DirectionalLight* m_pDirectionalLight;
        PuRe_IMaterial* m_pDirectionalLightMaterial;

        PuRe_Camera* m_UICamera;
        
        Editor::CBrickSupervisor* m_pBrickSupervisor;
        Editor::CWorker* m_pWorker;
        Editor::CShipHandler* m_pShipHandler;
        Editor::CColorFields* m_pColorFields;
        Editor::CModeMenu* m_pModeMenu;
        sba::CUIElementFader* m_pBrickSupervisorFader;
        sba::CUIElementFader* m_pColorFieldsFader;
        sba::CUIElementFader* m_pModeMenuFader;

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
        int Update(PuRe_Application* a_pApplication);

        /// @brief Renders the scene.
        ///
        /// @param graphics The graphics interface.
        ///
        void Render(PuRe_Application* a_pApplication);

        static void PreRender(PuRe_DirectionalLight* a_pDirectionalLight, PuRe_IMaterial* a_pDirectionalLightMaterial,bool a_Skybox);
        static void PostRender(Editor::CCamera* a_pSceneCamera, PuRe_Camera* a_pUICamera, PuRe_IMaterial* a_pUIMaterial, PuRe_IMaterial* a_pPostMaterial);
        //To make sure the Ship Previews get rendered the same way as in the Editor itself. Therefore more overhead, which is bad but its only at initialization, so i guess its worth it so we have no duplicated code and forget to update both parts... The only indipendent part is currently the Editor::CCamera which has its default setup in its own functions.
        static void InitCommonRenderComponents(PuRe_IMaterial** a_ppPostMaterial = nullptr, PuRe_DirectionalLight** a_ppDirectionalLight = nullptr, PuRe_IMaterial** a_ppDirectionalLightMaterial = nullptr, PuRe_PointLight** a_ppPointLight = nullptr, PuRe_IMaterial** a_ppPointLightMaterial = nullptr, PuRe_Camera** a_ppUICamera = nullptr);

        /// @brief Exists the scene.
        ///
        void Exit();

    };
}

#endif /* _EDITORSCENE_H_ */