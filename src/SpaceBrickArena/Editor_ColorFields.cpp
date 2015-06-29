#include "include/Editor_ColorFields.h"

#include "include/Navigation.h"
#include "include/Space.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CColorFields::CColorFields(int a_PlayerIdx)
    {
        this->m_PlayerIdx = a_PlayerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    CColorFields::~CColorFields()
    {
        SAFE_DELETE(this->m_pQuad);
    }

    // **************************************************************************
    // **************************************************************************
    void CColorFields::Initialize(PuRe_IGraphics& a_pGraphics)
    {
        this->m_pQuad = new PuRe_Quad(&a_pGraphics);
        /*
        http://www.brothers-brick.com/downloads/2010-LEGO-color-palette.pdf
        http://www.peeron.com/cgi-bin/invcgis/colorguide.cgi
        Fancy copy paste action:
        https://docs.google.com/spreadsheets/d/1ylRPAiBlgef9n3LTzAtl0Q1UpJdUz5UcqEF_XTQfrSM
        */
        this->Add(0.9490196078f, 0.9529411765f, 0.9490196078f);
        this->Add(0.631372549f, 0.6470588235f, 0.6352941176f);
        this->Add(0.9764705882f, 0.9137254902f, 0.6f);
        this->Add(0.8431372549f, 0.7725490196f, 0.6f);
        this->Add(0.7607843137f, 0.8549019608f, 0.7215686275f);
        this->Add(0.9098039216f, 0.7294117647f, 0.7803921569f);
        this->Add(0.7960784314f, 0.5176470588f, 0.2588235294f);
        this->Add(0.8f, 0.5568627451f, 0.4078431373f);
        this->Add(0.768627451f, 0.1568627451f, 0.1058823529f);
        this->Add(0.768627451f, 0.4392156863f, 0.6274509804f);
        this->Add(0.05098039216f, 0.4117647059f, 0.6705882353f);
        this->Add(0.9607843137f, 0.8039215686f, 0.1843137255f);
        this->Add(0.3843137255f, 0.2784313725f, 0.1960784314f);
        this->Add(0.1058823529f, 0.1647058824f, 0.2039215686f);
        this->Add(0.4274509804f, 0.431372549f, 0.4235294118f);
        this->Add(0.1568627451f, 0.4980392157f, 0.2745098039f);
        this->Add(0.631372549f, 0.768627451f, 0.5450980392f);
        this->Add(0.9529411765f, 0.8117647059f, 0.6078431373f);
        this->Add(0.2941176471f, 0.5921568627f, 0.2901960784f);
        this->Add(0.6274509804f, 0.3725490196f, 0.2039215686f);
        this->Add(0.7568627451f, 0.7921568627f, 0.8705882353f);
        this->Add(0.7058823529f, 0.8235294118f, 0.8901960784f);
        this->Add(0.9333333333f, 0.768627451f, 0.7137254902f);
        this->Add(0.8549019608f, 0.5254901961f, 0.4745098039f);
        this->Add(0.431372549f, 0.6f, 0.7882352941f);
        this->Add(0.7803921569f, 0.7568627451f, 0.7176470588f);
        this->Add(0.4196078431f, 0.1960784314f, 0.4823529412f);
        this->Add(0.8862745098f, 0.6078431373f, 0.2470588235f);
        this->Add(0.8549019608f, 0.5215686275f, 0.2509803922f);
        this->Add(0.0f, 0.5607843137f, 0.6078431373f);
        this->Add(0.4078431373f, 0.3607843137f, 0.262745098f);
        this->Add(0.262745098f, 0.3294117647f, 0.5764705882f);
        this->Add(0.4078431373f, 0.4549019608f, 0.6745098039f);
        this->Add(0.7803921569f, 0.8235294118f, 0.2352941176f);
        this->Add(0.3333333333f, 0.6470588235f, 0.6862745098f);
        this->Add(0.7176470588f, 0.8431372549f, 0.8352941176f);
        this->Add(0.6431372549f, 0.7411764706f, 0.2745098039f);
        this->Add(0.8509803922f, 0.8941176471f, 0.6549019608f);
        this->Add(0.9058823529f, 0.6745098039f, 0.3450980392f);
        this->Add(0.8274509804f, 0.4352941176f, 0.2980392157f);
        this->Add(0.5725490196f, 0.2235294118f, 0.4705882353f);
        this->Add(0.9176470588f, 0.7215686275f, 0.568627451f);
        this->Add(0.862745098f, 0.737254902f, 0.5058823529f);
        this->Add(0.6823529412f, 0.4784313725f, 0.3490196078f);
        this->Add(0.6117647059f, 0.6392156863f, 0.6588235294f);
        this->Add(0.4549019608f, 0.5254901961f, 0.6117647059f);
        this->Add(0.5294117647f, 0.4862745098f, 0.5647058824f);
        this->Add(0.8784313725f, 0.5960784314f, 0.3921568627f);
        this->Add(0.5843137255f, 0.5411764706f, 0.4509803922f);
        this->Add(0.1254901961f, 0.2274509804f, 0.337254902f);
        this->Add(0.1529411765f, 0.2745098039f, 0.1725490196f);
        this->Add(0.4745098039f, 0.5333333333f, 0.631372549f);
        this->Add(0.5843137255f, 0.5568627451f, 0.6392156863f);
        this->Add(0.5764705882f, 0.5294117647f, 0.4039215686f);
        this->Add(0.3411764706f, 0.3450980392f, 0.3411764706f);
        this->Add(0.0862745098f, 0.1137254902f, 0.1960784314f);
        this->Add(0.6705882353f, 0.6784313725f, 0.6745098039f);
        this->Add(0.4705882353f, 0.5647058824f, 0.5058823529f);
        this->Add(0.5843137255f, 0.4745098039f, 0.462745098f);
        this->Add(0.4823529412f, 0.1803921569f, 0.1843137255f);
        this->Add(0.4588235294f, 0.4235294118f, 0.3843137255f);
        this->Add(0.8431372549f, 0.662745098f, 0.2941176471f);
        this->Add(0.5098039216f, 0.5411764706f, 0.3647058824f);
        this->Add(0.9764705882f, 0.8392156863f, 0.1803921569f);
        this->Add(0.9098039216f, 0.6705882353f, 0.1764705882f);
        this->Add(0.4117647059f, 0.2509803922f, 0.1529411765f);
        this->Add(0.8117647059f, 0.3764705882f, 0.1411764706f);
        this->Add(0.6392156863f, 0.6352941176f, 0.6431372549f);
        this->Add(0.2745098039f, 0.4039215686f, 0.6431372549f);
        this->Add(0.137254902f, 0.2784313725f, 0.5450980392f);
        this->Add(0.5568627451f, 0.2588235294f, 0.5215686275f);
        this->Add(0.3882352941f, 0.3725490196f, 0.3803921569f);
        this->Add(0.8980392157f, 0.8941176471f, 0.8705882353f);
        this->Add(0.6901960784f, 0.5568627451f, 0.2666666667f);
        this->Add(0.4392156863f, 0.5843137255f, 0.4705882353f);
        this->Add(0.4745098039f, 0.7098039216f, 0.7098039216f);
        this->Add(0.6235294118f, 0.7647058824f, 0.9137254902f);
        this->Add(0.4235294118f, 0.5058823529f, 0.7176470588f);
        this->Add(0.5607843137f, 0.2980392157f, 0.1647058824f);
        this->Add(0.4862745098f, 0.3607843137f, 0.2705882353f);
        this->Add(0.5882352941f, 0.4392156863f, 0.6235294118f);
        this->Add(0.4196078431f, 0.3843137255f, 0.6078431373f);
        this->Add(0.6549019608f, 0.662745098f, 0.8078431373f);
        this->Add(0.8039215686f, 0.3843137255f, 0.5960784314f);
        this->Add(0.8941176471f, 0.6784313725f, 0.7843137255f);
        this->Add(0.862745098f, 0.5647058824f, 0.5843137255f);
        this->Add(0.9411764706f, 0.8352941176f, 0.6274509804f);
        this->Add(0.9215686275f, 0.7215686275f, 0.4980392157f);
        this->Add(0.9921568627f, 0.9176470588f, 0.5490196078f);
        this->Add(0.4901960784f, 0.7333333333f, 0.8666666667f);
        this->Add(0.2039215686f, 0.168627451f, 0.4588235294f);
        this->m_pNavigation = new sba::CNavigation(19, this->m_Colors.size() - 1);
    }

    // **************************************************************************
    // **************************************************************************
    void CColorFields::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer)
    {
        this->m_pNavigation->Update(a_pTimer, sba_Input->Direction(sba_Direction::EditorChooseElement, this->m_PlayerIdx));
    }

    // **************************************************************************
    // **************************************************************************
    void CColorFields::Render(PuRe_IGraphics& a_pGraphics, float a_Visibility)
    {
        for (int i = 0; i <= this->m_pNavigation->GetLastElementId(); i++)
        {
            PuRe_Vector2F listPos = PuRe_Vector2F((float)(i % this->m_pNavigation->GetElementsCountPerLine()), floorf((float)i / (float)this->m_pNavigation->GetElementsCountPerLine()));
            PuRe_Vector3F pos = PuRe_Vector3F(this->m_ListStart + this->m_ListStep * listPos, 0);
            pos.Y -= (1.0f - a_Visibility) * this->m_ListStart.Y - this->m_ListStep.Y;

            float size = this->m_ElementSize;
            if (this->m_pNavigation->GetFocusedElementId() == i)
            {
                size *= 1.4f;
            }
            sba_Renderer->Draw(1, false, this->m_pQuad->GetBuffer(), this->m_pQuad->GetBuffer()->GetSize(), PuRe_Primitive::Trianglestrip, sba_BrickManager->GetBrickUIMaterial(), pos, PuRe_MatrixF::Identity(), PuRe_Vector3F::Zero(), PuRe_Vector3F(size, size, size), this->m_Colors[i]);
        }
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Color& CColorFields::GetCurrentColor()
    {
        return this->m_Colors[this->m_pNavigation->GetFocusedElementId()];
    }

    // **************************************************************************
    // **************************************************************************
    void CColorFields::Add(const PuRe_Color color)
    {
        this->m_Colors.push_back(color);
    }

    // **************************************************************************
    // **************************************************************************
    void CColorFields::Add(float r, float g, float b)
    {
        this->Add(PuRe_Color(r, g, b, 1.0f));
    }

    // **************************************************************************
    // **************************************************************************
    void CColorFields::Remove(const PuRe_Color color)
    {
        this->m_Colors.erase(std::remove(this->m_Colors.begin(), this->m_Colors.end(), color), this->m_Colors.end());
    }
}