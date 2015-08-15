#ifndef _SPRITEREADER_H_
#define _SPRITEREADER_H_


// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <cstring> // std::string
#include <cstdio> // fopen
#include <unordered_map>

// Declare namespace sba
namespace sba
{
    /// @brief Data of Our Sprite to make a map out of it
    ///
    struct SSpriteData
    {
        PuRe_Vector2F Position;
        PuRe_Vector2F Size;
    };
    /// @brief A SpriteReader to read the Position of Sprites from a Textfile
    ///
    class CSpriteReader
    {
    private:
        /// @brief Used Sprite to render
        ///
        PuRe_Sprite* m_pSprite;
        /// @brief We save our Data into a map
        ///
        std::unordered_map<std::string, SSpriteData> m_Reader;
    public:
        /// @brief Constructor to initialize a SpriteReader
        ///
        /// @param Graphic pointer to initialize our Sprite
        /// @param Path to our Sprite
        /// @param Path to our Textfile
        ///
        CSpriteReader(PuRe_IGraphics* a_pGraphics, const char* a_pSpritePath, const char* a_pTextPath);
        /// @brief Destructor to destroy a SpriteReader
        ///
        ~CSpriteReader();
    public:
        /// @brief Draw a Part of the Sprite
        ///
        /// @param Index of the Rendertarget
        /// @param Renderer
        /// @param Name of the rendered Part
        /// @param Used Material to draw
        /// @param Position where to render
        /// @param Rotation of the Sprite
        /// @param Index of the drawcall, -1 for all
        /// @param Size in Range from 0 to 1
        /// @param UVSize in Range from 0 to 1
        /// @param Color
        ///
        void Draw(int a_Index, PuRe_Renderer* a_pRenderer, const char* a_pName, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, 
        PuRe_Vector3F a_Rotation, int a_RenderIndex = -1, PuRe_Vector2F Size = PuRe_Vector2F(1.0f, 1.0f), 
        PuRe_Vector2F UVSize = PuRe_Vector2F(1.0f, 1.0f), PuRe_Vector2F UVPos = PuRe_Vector2F(0.0f, 0.0f), PuRe_Color a_Color = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f));

        /// @brief Draw a Part of the Sprite
        ///
        /// @param Index of the Rendertarget
        /// @param Renderer
        /// @param Name of the rendered Part
        /// @param Used Material to draw
        /// @param Position where to render
        /// @param Rotation of the Sprite
        /// @param Index of the drawcall, -1 for all
        /// @param Size in Range from 0 to 1
        /// @param UVSize in Range from 0 to 1
        /// @param Color
        ///
        void Draw(int a_Index, PuRe_Renderer* a_pRenderer, const char* a_pName, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, 
        PuRe_MatrixF a_Rotation, int a_RenderIndex = -1, PuRe_Vector2F Size = PuRe_Vector2F(1.0f, 1.0f), 
        PuRe_Vector2F UVSize = PuRe_Vector2F(1.0f, 1.0f), PuRe_Vector2F UVPos = PuRe_Vector2F(0.0f, 0.0f),PuRe_Color a_Color=PuRe_Color(1.0f,1.0f,1.0f,1.0f));

        /// @brief Get the Size of the Sprite
        ///
        /// @remarks this is used to avoid sprites popping up.
        ///
        /// @returns the Size as Vector2F
        ///
        PuRe_Vector2F GetSize();
    };
}
#endif /* _SPRITEREADER_H_ */