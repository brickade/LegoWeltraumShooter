#pragma once

#include <PuReEngine\Vector3.h>

namespace ong
{
	class Body;
	class Collider;
	class ShapePtr;
	struct Transform;
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
	void DrawCollider(ong::Collider* a_pCollider, ong::Transform& t, PuReEngine::Core::CCamera* a_pCam, PuReEngine::Core::IGraphics* a_pGraphics);
	void DrawShape(ong::ShapePtr a_Shape, ong::Transform& t, const PuReEngine::Core::Vector3<float>& a_color,  PuReEngine::Core::CCamera* a_pCam, PuReEngine::Core::IGraphics* a_pGraphics);

}