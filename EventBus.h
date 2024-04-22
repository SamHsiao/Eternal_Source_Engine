#ifndef EventBus_h
#define EventBus_h

#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class EventBus
{
public:

	static inline std::queue<std::string> current_publish;

	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> subscriptions;

	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> subscriptions_this_frame;

	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> unsubscriptions_this_frame;
	
	static void ProcessSubscriptions();

	static void Publish(std::string event_type, luabridge::LuaRef event_object);

	static void Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);

	static void Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);
};

#endif
