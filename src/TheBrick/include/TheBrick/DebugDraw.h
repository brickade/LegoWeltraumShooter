#pragma once


namespace ong
{
	class Body;
	class Collider;
}

namespace PuReEngine
{
	namespace Core
	{
		struct IGraphics;
		class CCamera;
	}
}



namespace TheBrick
{

	void DrawBody(ong::Body* a_pBody, PuReEngine::Core::CCamera* a_pCam, PuReEngine::Core::IGraphics* a_pGraphics);

}