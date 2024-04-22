//
//  Actor.cpp
//  game_engine
//
//  Created by 蕭翔群 on 2024/2/1.
//

#include "Actor.hpp"
#include "ComponentManager.h"

Actor::Actor() {
    actorID = actorCount;
    actorCount++;
}

void Actor::Start() {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnStart"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                if ((*(it->second))["enabled"].cast<bool>() == true) {
                    (*(it->second))["OnStart"](*(it->second));
                }
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::ProcessAddedComponents() {
    for (auto it = components_to_add.begin(); it != components_to_add.end(); it++) {
        InjectConvenienceReferences(it->second);
        lua_components.insert({ it->first, it->second });
        
        // onStart or ready
        try {
            if ((*(it->second))["enabled"].cast<bool>() == true) {
                if (!(*(it->second))["Ready"].isNil()) {
                    (*(it->second))["Ready"](*(it->second));
                }
                else {
                    (*(it->second))["OnStart"](*(it->second));
                }
            }
        }
        catch (const luabridge::LuaException& e) {
            ReportError(actor_name, e);
        }
    }
    components_to_add.clear();
}

void Actor::Update() {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnUpdate"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                if ((*(it->second))["enabled"].cast<bool>() == true) {
                    (*(it->second))["OnUpdate"](*(it->second));
                }
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::LateUpdate() {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnLateUpdate"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                if ((*(it->second))["enabled"].cast<bool>() == true) {
                    (*(it->second))["OnLateUpdate"](*(it->second));
                }
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::ProcessRemovedComponents() {
    for (int i = 0; i < components_to_remove.size(); i++) {
        if (!(*lua_components[components_to_remove[i]])["OnDestroy"].isNil()) {
            try {
                (*lua_components[components_to_remove[i]])["OnDestroy"]((*lua_components[components_to_remove[i]]));
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
        lua_components.erase(components_to_remove[i]);
    }
    components_to_remove.clear();
}

void Actor::Destroy() {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnDestroy"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                (*(it->second))["OnDestroy"](*(it->second));
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::OnCollisionEnter(Collision collision) {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnCollisionEnter"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                (*(it->second))["OnCollisionEnter"](*(it->second), &collision);
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::OnCollisionExit(Collision collision) {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnCollisionExit"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                (*(it->second))["OnCollisionExit"](*(it->second), &collision);
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::OnTriggerEnter(Collision collision) {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnTriggerEnter"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                (*(it->second))["OnTriggerEnter"](*(it->second), &collision);
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::OnTriggerExit(Collision collision) {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (!(*(it->second))["OnTriggerExit"].isNil()) {
            InjectConvenienceReferences(it->second);

            try {
                (*(it->second))["OnTriggerExit"](*(it->second), &collision);
            }
            catch (const luabridge::LuaException& e) {
                ReportError(actor_name, e);
            }
        }
    }
}

void Actor::InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref) {
    (*component_ref)["actor"] = this;
}

void Actor::ReportError(const std::string& actor_name, const luabridge::LuaException& e) {
    std::string error_message = e.what();

    /* Normalize file path across platforms */
    std::replace(error_message.begin(), error_message.end(), '\\', '/');

    /* Display (with color codes) */
    std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
}

std::string Actor::GetName() {
    return actor_name;
}

int Actor::GetID() {
    return actorID;
}

luabridge::LuaRef Actor::GetComponentByKey(std::string key) {
    if (lua_components.find(key) == lua_components.end()) {
        return luabridge::LuaRef(ComponentManager::GetLuaState());
    }

    if (std::find(components_to_remove.begin(), components_to_remove.end(), key) != components_to_remove.end()) {
        return luabridge::LuaRef(ComponentManager::GetLuaState());
    }

    return *lua_components[key];
}

luabridge::LuaRef Actor::GetComponent(std::string type_name) {
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if ((*(it->second))["type"].tostring() == type_name) {
            if (std::find(components_to_remove.begin(), components_to_remove.end(), it->first) == components_to_remove.end()) {
                return *(it->second);
            }
        }
    }
    return luabridge::LuaRef(ComponentManager::GetLuaState());
}

luabridge::LuaRef Actor::GetComponents(std::string type_name) {
    luabridge::LuaRef table = luabridge::newTable(ComponentManager::GetLuaState());
    int index = 1;
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if ((*(it->second))["type"].tostring() == type_name) {
            if (std::find(components_to_remove.begin(), components_to_remove.end(), it->first) == components_to_remove.end()) {
                table[index] = *(it->second);
                index++;
            }
        }
    }

    table.push(ComponentManager::GetLuaState());
    return table;
}

luabridge::LuaRef Actor::LuaAddComponent(const std::string& type_name) {
    std::string current_key = "r" + std::to_string(componentAdded);
    componentAdded++;
    std::shared_ptr<luabridge::LuaRef> ref;
    if (type_name == "Rigidbody") {
        ref = ComponentManager::SetRigidbody(current_key);
    }
    else {
        ref = ComponentManager::SetComponent(type_name, current_key);
    }
    components_to_add[current_key] = ref;
    return *ref;
}

void Actor::LuaRemoveComponent(luabridge::LuaRef component_ref) {
    component_ref["enabled"] = false;
    for (auto it = lua_components.begin(); it != lua_components.end(); it++) {
        if (*(it->second) == component_ref) {
            components_to_remove.push_back(it->first);
        }
    }
}