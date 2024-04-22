//
//  Scene.hpp
//  game_engine
//
//  Created by 蕭翔群 on 2024/2/1.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <algorithm>

#include "Actor.hpp"
#include "AudioManager.h"
#include "ComponentManager.h"
#include "Renderer.h"
#include "rapidjson/include/rapidjson/filereadstream.h"
#include "rapidjson/include/rapidjson/document.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Scene
{
public:

    // Define the hash struct for glm::ivec2
    struct IVecCompare {
        size_t operator()(const glm::ivec2& vec) const {
            // A simple hash combination method: bit shift and exclusive OR
            return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
        }
    };

    static uint64_t create_composite_key(int x, int y) {

        // cast to ensure the ints become exactly 32 bits in size
        uint32_t ux = static_cast<uint32_t>(x);
        uint32_t uy = static_cast<uint32_t>(y);

        // place x into right 32 bits
        uint64_t result = static_cast<uint64_t>(ux);

        // move x to left 32 bits
        result = result << 32;

        // place y into right 32 bits.
        result = result | static_cast<uint64_t>(uy);

        return result;
    }

    static inline std::vector<std::shared_ptr<Actor>> actors = {};

    static inline std::vector<std::shared_ptr<Actor>> current_scene_actors = {};

    std::vector<int> actorIndices;

    static inline std::vector<std::shared_ptr<Actor>> actors_to_add = {};

    static inline std::vector<Actor*> actors_to_destroy = {};

    std::unordered_map<glm::ivec2, std::unordered_set<Actor*>, IVecCompare> collider_map;

    std::vector<Actor*> triggered_actors;
    
    int player_actor_index = -1;

    float player_movement_speed = 0.02f;
    
    glm::vec2 directionOfInput;

    static void LoadTemplate(const std::string& template_name, Actor* new_actor);

    void RenderScene(Renderer& renderer);
    
    void CleanUpReferences();

    /* Functions for lua call */

    static Actor* FindActor(const std::string& name);

    static luabridge::LuaRef FindAllActors(const std::string& name);

    static Actor* Instantiate(const std::string& actor_template_name);

    static void Destroy(Actor* actor);

};

#endif /* Scene_hpp */
