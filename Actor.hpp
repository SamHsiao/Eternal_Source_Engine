//
//  Actor.hpp
//  game_engine
//
//  Created by 蕭翔群 on 2024/2/1.
//

#ifndef Actor_hpp
#define Actor_hpp

#include <stdio.h>
#include <string>
#include <optional>
#include <unordered_set>
#include <map>
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "glm/glm.hpp"
#include "box2d/box2d.h"
#include "Helper.h"

struct Actor;

class Collision {
public:
    Actor* other;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

struct Actor
{
private:
    // Define the hash struct for glm::ivec2
    struct IVecCompare {
        size_t operator()(const glm::ivec2& vec) const {
            // A simple hash combination method: bit shift and exclusive OR
            return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
        }
    };

public:
    static inline int componentAdded = 0;
    static inline int actorCount = 0;

    std::string actor_name = "";
    int actorID = 0;
    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> lua_components;
    std::vector<std::shared_ptr<luabridge::LuaRef>> components_have_start;

    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components_to_add;
    std::vector<std::string> components_to_remove;
    bool dontDestroyOnLoad = false;
    bool fromAnotherScene = false;

    /*int frameDamaged = -50;
    int frameAttacked = -50;
    bool has_collider = false;
    bool has_trigger = false;
    bool met_player = false;
    std::unordered_set<Actor*> colliding_actors_this_frame;
    std::unordered_set<glm::ivec2, IVecCompare> prev_collider;*/
    
    Actor();

    void Start();

    void ProcessAddedComponents();

    void Update();

    void LateUpdate();

    void Destroy();

    void ProcessRemovedComponents();

    void OnCollisionEnter(Collision collision);

    void OnCollisionExit(Collision collision);

    void OnTriggerEnter(Collision collision);

    void OnTriggerExit(Collision collision);

    void InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref);

    void ReportError(const std::string& actor_name, const luabridge::LuaException& e);

    std::string GetName();

    int GetID();

    luabridge::LuaRef GetComponentByKey(std::string key);

    luabridge::LuaRef GetComponent(std::string type_name);

    luabridge::LuaRef GetComponents(std::string type_name);

    luabridge::LuaRef LuaAddComponent(const std::string& type_name);

    void LuaRemoveComponent(luabridge::LuaRef component_ref);
};

#endif /* Actor_hpp */
