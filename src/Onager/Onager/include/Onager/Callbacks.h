#pragma once


namespace ong
{

	class Collider;

	//return true to stop querying
	typedef bool(*ColliderQueryCallBack)(Collider* self, Collider* other);
	//return true to stop querying
	typedef bool(*ShapeQueryCallBack)(Collider* other, void* userData);


}