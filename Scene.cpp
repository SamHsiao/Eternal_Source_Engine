//
//  Scene.cpp
//  game_engine
//
//  Created by 蕭翔群 on 2024/2/1.
//

#include <iostream>
#include "Scene.hpp"
#include "Engine.hpp"

void Scene::LoadTemplate(const std::string& template_name, Actor* new_actor) {
    rapidjson::Document doc;
    if (!std::filesystem::exists("resources/actor_templates/" + template_name + ".template")) {
        std::cout << "error: template " + template_name + " is missing";
        exit(0);
    }
    Engine::ReadJsonFile("resources/actor_templates/" + template_name + ".template", doc);
    if (doc.HasMember("name")) {
        new_actor->actor_name = doc["name"].GetString();
    }
    if (doc.HasMember("components")) {
        for (rapidjson::Value::ConstMemberIterator itr = doc["components"].MemberBegin(); itr != doc["components"].MemberEnd(); itr++) {
            std::string component_name = itr->name.GetString();
            if (doc["components"][component_name.c_str()].HasMember("type")) {
                std::string component_filename = doc["components"][component_name.c_str()]["type"].GetString();
                if (component_filename == "Rigidbody") {
                    ComponentManager::CreateRigidbody(component_name, new_actor);
                }
                else {
                    if (!std::filesystem::exists("resources/component_types/" + component_filename + ".lua")) {
                        std::cout << "error: failed to locate component " + component_filename;
                        exit(0);
                    }
                    ComponentManager::AddComponentsToActor(component_filename, component_name, new_actor);
                }
            }

            for (rapidjson::Value::ConstMemberIterator itr2 = doc["components"][component_name.c_str()].MemberBegin();
                itr2 != doc["components"][component_name.c_str()].MemberEnd(); itr2++) {
                if (itr2->name.GetString() != "type") {
                    if (itr2->value.IsString()) {
                        (*(new_actor->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetString();
                    }
                    else if (itr2->value.IsBool()) {
                        (*(new_actor->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetBool();
                    }
                    else if (itr2->value.IsFloat()) {
                        (*(new_actor->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetFloat();
                    }
                    else if (itr2->value.IsInt()) {
                        (*(new_actor->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetInt();
                    }
                }
            }
        }
    }
}

void Scene::RenderScene(Renderer& renderer) {
    // Set zoom factor
    SDL_RenderSetScale(renderer.renderer, renderer.zoom_factor, renderer.zoom_factor);

    // Set zoom factor
    SDL_RenderSetScale(renderer.renderer, 1.0, 1.0);
}

void Scene::CleanUpReferences() {
    //posOfActors.clear();
    //posBlocked.clear();
    actorIndices.clear();
    triggered_actors.clear();
    //orderOfActorRender.clear();
}

Actor* Scene::FindActor(const std::string& name) {
    for (int i = 0; i < actors.size(); i++) {
        if (actors[i]->actor_name == name) {
            if (std::find(actors_to_destroy.begin(), actors_to_destroy.end(), actors[i].get()) == actors_to_destroy.end()) {
                return actors[i].get();
            }
        }
    }
    for (int i = 0; i < actors_to_add.size(); i++) {
        if (actors_to_add[i]->actor_name == name) {
            if (std::find(actors_to_destroy.begin(), actors_to_destroy.end(), actors_to_add[i].get()) == actors_to_destroy.end()) {
                /*luabridge::push(ComponentManager::GetLuaState(), *actors[i]);
                luabridge::LuaRef actorRef(ComponentManager::GetLuaState(), luabridge::Stack<luabridge::LuaRef>::get(ComponentManager::GetLuaState(), lua_gettop(ComponentManager::GetLuaState())));
                lua_pop(ComponentManager::GetLuaState(), 1);*/
                return actors_to_add[i].get();
            }
        }
    }
    return luabridge::LuaRef(ComponentManager::GetLuaState());
}

luabridge::LuaRef Scene::FindAllActors(const std::string& name) {
    luabridge::LuaRef table = luabridge::newTable(ComponentManager::GetLuaState());
    int index = 1;
    for (int i = 0; i < actors.size(); i++) {
        if (actors[i]->actor_name == name) {
            if (std::find(actors_to_destroy.begin(), actors_to_destroy.end(), actors[i].get()) == actors_to_destroy.end()) {
                table[index] = actors[i].get();
                index++;
            }
        }
    }

    for (int i = 0; i < actors_to_add.size(); i++) {
        if (actors_to_add[i]->actor_name == name) {
            if (std::find(actors_to_destroy.begin(), actors_to_destroy.end(), actors_to_add[i].get()) == actors_to_destroy.end()) {
                table[index] = actors_to_add[i].get();
                index++;
            }
        }
    }

    table.push(ComponentManager::GetLuaState());
    return table;
}

Actor* Scene::Instantiate(const std::string& actor_template_name) {
    Actor new_actor;

    LoadTemplate(actor_template_name, &new_actor);

    std::shared_ptr<Actor> actor_ref = std::make_shared<Actor>(new_actor);

    actors_to_add.push_back(actor_ref);

    return actor_ref.get();
}

void Scene::Destroy(Actor* actor) {

    for (auto it = actor->lua_components.begin(); it != actor->lua_components.end(); it++) {
        (*(it->second))["enabled"] = false;
    }

    actors_to_destroy.push_back(actor);

}