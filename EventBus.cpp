#include "EventBus.h"

void EventBus::ProcessSubscriptions() {
	// Process subscriptions
	for (auto it = subscriptions_this_frame.begin(); it != subscriptions_this_frame.end(); it++) {
		for (int i = 0; i < it->second.size(); i++) {
			subscriptions[it->first].push_back((it->second)[i]);
		}
	}

	// Process unsubscriptions
	for (auto it = unsubscriptions_this_frame.begin(); it != unsubscriptions_this_frame.end(); it++) {
		std::string event_type = it->first;
		for (int i = 0; i < it->second.size(); i++) {
			std::pair<luabridge::LuaRef, luabridge::LuaRef> ref_pair = (it->second)[i];
			auto it = std::find(subscriptions[event_type].begin(), subscriptions[event_type].end(), ref_pair);
			if (it != subscriptions[event_type].end()) {
				subscriptions[event_type].erase(it);
			}
		}
	}

	subscriptions_this_frame.clear();
	unsubscriptions_this_frame.clear();
}

void EventBus::Publish(std::string event_type, luabridge::LuaRef event_object) {
	for (int i = 0; i < subscriptions[event_type].size(); i++) {
		luabridge::LuaRef component = subscriptions[event_type][i].first;
		luabridge::LuaRef function = subscriptions[event_type][i].second;
		function(component, event_object);
	}
}

void EventBus::Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
	subscriptions_this_frame[event_type].push_back({ component, function });
}

void EventBus::Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
	unsubscriptions_this_frame[event_type].push_back({ component, function });
}
