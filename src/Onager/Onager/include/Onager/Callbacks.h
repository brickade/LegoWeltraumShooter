#pragma once


namespace ong
{

	class Collider;

	//return false to stop querying
	typedef bool(*ColliderQueryCallBack)(Collider* self, Collider* other);
	//return false to stop querying
	typedef bool(*ShapeQueryCallBack)(Collider* other, void* userData);


}