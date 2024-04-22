#ifndef Raycast_h
#define Raycast_h

#include "Actor.hpp"
#include "box2d/box2d.h"
#include "Rigidbody.h"
#include "ComponentManager.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class HitResult {
public:
	Actor* actor;
	b2Vec2 point = b2Vec2(0, 0);
	b2Vec2 normal = b2Vec2(0, 0);
	bool is_trigger = false;
};

class RayCastCallback : public b2RayCastCallback {
public:

	std::multimap<float, HitResult> hit_result;

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction);
};

class Raycast
{
public:
	static luabridge::LuaRef PhysicsRaycast(b2Vec2 pos, b2Vec2 dir, float dist);

	static luabridge::LuaRef PhysicsRaycastAll(b2Vec2 pos, b2Vec2 idr, float dist);
};

#endif