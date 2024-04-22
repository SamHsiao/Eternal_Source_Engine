#ifndef ComponentManager_h
#define ComponentManager_h

#include <string>
#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <thread>
#include "Helper.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "box2d/box2d.h"
#include "Rigidbody.h"
#include "Raycast.h"
#include "EventBus.h"
#include "ParticleSystem.h"
#include "Actor.hpp"
#include "Input.h"

class ComponentManager
{
private:

	static lua_State* lua_state;

	static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> component_tables;

	struct LuaRefComparator {
		bool operator()(const std::shared_ptr<luabridge::LuaRef>& lhs,
			const std::shared_ptr<luabridge::LuaRef>& rhs) const {
			// Assuming LuaRef objects reference Lua tables with a 'priority' field
			if (!lhs || !rhs) return false; // Handle null pointers if necessary
			return (*lhs)["key"] < (*rhs)["key"];
		}
	};

public:

	static lua_State* GetLuaState();

	static void Initialize();

	static void InitializeState();

	static void InitializeFunctions();

	static void InitializeComponents();

	static std::shared_ptr<luabridge::LuaRef> SetComponent(const std::string& component_type, const std::string& component_key);

	static std::shared_ptr<luabridge::LuaRef> SetRigidbody(const std::string& component_key);

	static void AddComponentsToActor(const std::string& lua_file, const std::string& component_name, Actor* actor);

	static void AddComponentsToActor(const std::string& lua_file, const std::string& component_name, std::shared_ptr<Actor> actor);

	static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);

	static std::shared_ptr<luabridge::LuaRef> CreateRigidbody(const std::string& component_name, std::shared_ptr<Actor> actor);

	static std::shared_ptr<luabridge::LuaRef> CreateRigidbody(const std::string& component_name, Actor* actor);

	/* Lifecycle Functions */

	static void CppLog(const std::string& message);

	static void CppLogError(const std::string& message);

	static void Quit();

	static void Sleep(int milliseconds);

	static int GetFrame();

	static void OpenURL(const std::string& url);
};

#endif /* ComponentManager.h */
