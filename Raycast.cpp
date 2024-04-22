#include "Raycast.h"

float RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {

	Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
	if (actor == nullptr) {
		return -1.0f;
	}

	HitResult hitResult;
	hitResult.point = point;
	hitResult.normal = normal;
	hitResult.actor = actor;
	hitResult.is_trigger = fixture->IsSensor();

	hit_result.insert({ fraction, hitResult });

	return 1.0f;
}

luabridge::LuaRef Raycast::PhysicsRaycast(b2Vec2 pos, b2Vec2 dir, float dist) {
	if (dist <= 0.0f || !Rigidbody::world_initialized) {
		return luabridge::LuaRef(ComponentManager::GetLuaState());
	}

	dir.Normalize();
	b2Vec2 pos2 = b2Vec2(pos.x + dir.x * dist, pos.y + dir.y * dist);
	RayCastCallback raycast_callback;
	Rigidbody::physics_world->RayCast(&raycast_callback, pos, pos2);

	if (raycast_callback.hit_result.empty()) {
		return luabridge::LuaRef(ComponentManager::GetLuaState());
	}

	for (auto it = raycast_callback.hit_result.begin(); it != raycast_callback.hit_result.end(); it++) {
		luabridge::LuaRef hitResultRef(ComponentManager::GetLuaState(), &(it->second));
		return hitResultRef;
	}
    
    return luabridge::LuaRef(ComponentManager::GetLuaState());
}

luabridge::LuaRef Raycast::PhysicsRaycastAll(b2Vec2 pos, b2Vec2 dir, float dist) {
	if (dist <= 0.0f || !Rigidbody::world_initialized) {
		return luabridge::LuaRef(ComponentManager::GetLuaState());
	}

	dir.Normalize();
	b2Vec2 pos2 = b2Vec2(pos.x + dir.x * dist, pos.y + dir.y * dist);
	RayCastCallback raycast_callback;
	Rigidbody::physics_world->RayCast(&raycast_callback, pos, pos2);

	if (raycast_callback.hit_result.empty()) {
		return luabridge::LuaRef(ComponentManager::GetLuaState());
	}

	luabridge::LuaRef table = luabridge::newTable(ComponentManager::GetLuaState());
	int index = 1;
	for (auto it = raycast_callback.hit_result.begin(); it != raycast_callback.hit_result.end(); it++) {
		luabridge::LuaRef hitResultRef(ComponentManager::GetLuaState(), &(it->second));
		table[index] = hitResultRef;
		index++;
	}

	return table;
}
