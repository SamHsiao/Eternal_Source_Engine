//
//  Engine.cpp
//  game_engine
//
//  Created by 蕭翔群 on 2024/2/1.
//

#include "Engine.hpp"

#include <string>
#include <iostream>

Scene Engine::scene = Scene();
Renderer Engine::renderer = Renderer();

void Engine::GameLoop()
{
    Initialize();

    while (running)
    {
        Input();

        renderer.CleanUp();

        Update();
        EventBus::ProcessSubscriptions();
        Rigidbody::PhysicsStep();
        Render();
        if (proceed_to_scene) {
            proceed_to_scene = false;
            Scene::current_scene_actors = Scene::actors;
            for (int i = 0; i < Scene::actors.size(); i++) {
                scene.actors[i]->Destroy();
            }
            Scene::actors.clear();
            Actor::actorCount = 0;
            renderer.cam_position.x = 0;
            renderer.cam_position.y = 0;
            LoadScene(initial_scene);
            RunOnStart();
        }

        Input::LateUpdate();

        Helper::SDL_RenderPresent498(renderer.renderer);
    }

    //print_option();
}

void Engine::Initialize() {
    
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    AudioHelper::Mix_AllocateChannels498(50);
    Input::Init();
    ComponentManager::Initialize();
    LoadResources();
    renderer.Initialize(game_title);
    //LoadInitImageAndTextAndAudio();
    //PlayInitAudio();
    renderer.LoadImages();
    AudioManager::LoadAudio();
    LoadScene(initial_scene);
    LoadParticleSystem();
    RunOnStart();
    //LoadHpImage();
    //scene.InitializeColliderMap();
}

void Engine::LoadResources() {

    // See if resources exist
    if (!std::filesystem::exists("resources")) {
        std::cout << "error: resources/ missing";
        exit(0);
    }

    // See if resourses/game.config exist
    else if (!std::filesystem::exists("resources/game.config")) {
        std::cout << "error: resources/game.config missing";
        exit(0);
    }
    
    // Load resources in game.config
    rapidjson::Document document;
    ReadJsonFile("resources/game.config", document);
    if (document.HasMember("game_start_message")) {
        game_begin_message = document["game_start_message"].GetString();
    }
    if (document.HasMember("game_over_bad_message")) {
        game_end_bad_message = document["game_over_bad_message"].GetString();
    }
    if (document.HasMember("game_over_good_message")) {
        game_end_good_message = document["game_over_good_message"].GetString();
    }
    if (document.HasMember("game_title")) {
        game_title = document["game_title"].GetString();
    }
    if (document.HasMember("player_movement_speed")) {
        scene.player_movement_speed = document["player_movement_speed"].GetFloat();
    }
    if (document.HasMember("initial_scene")) {
        initial_scene = document["initial_scene"].GetString();
    }
    else {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }
    
    // Load resources in rendering.config 
    if (std::filesystem::exists("resources/rendering.config")) {
        ReadJsonFile("resources/rendering.config", document);
        if (document.HasMember("x_resolution")) {
            //SIZE_OF_MAP.x = document["x_resolution"].GetInt();
            renderer.x_resolution = document["x_resolution"].GetInt();
        }
        if (document.HasMember("y_resolution")) {
            //SIZE_OF_MAP.y = document["y_resolution"].GetInt();
            renderer.y_resolution = document["y_resolution"].GetInt();
        }
        if (document.HasMember("clear_color_r")) {
            renderer.clear_color_r = document["clear_color_r"].GetInt();
        }
        if (document.HasMember("clear_color_g")) {
            renderer.clear_color_g = document["clear_color_g"].GetInt();
        }
        if (document.HasMember("clear_color_b")) {
            renderer.clear_color_b = document["clear_color_b"].GetInt();
        }
        if (document.HasMember("zoom_factor")) {
            renderer.zoom_factor = document["zoom_factor"].GetDouble();
        }
        if (document.HasMember("cam_ease_factor")) {
            renderer.cam_ease_factor = document["cam_ease_factor"].GetFloat();
        }
    }
}

void Engine::LoadScene(std::string& scene_name) {
    if (!std::filesystem::exists("resources/scenes/" + scene_name + ".scene")) {
        std::cout << "error: scene " + scene_name + " is missing";
        exit(0);
    }
    
    rapidjson::Document document;
    rapidjson::Document doc;
    ReadJsonFile("resources/scenes/" + scene_name + ".scene", document);
    const rapidjson::Value& actors_doc = document["actors"];
    for (rapidjson::SizeType i = 0; i < actors_doc.Size(); i++) {
        Actor actor; 
        std::shared_ptr<Actor> actor_ptr = std::make_shared<Actor>(actor);
        if (actors_doc[i].HasMember("template")) {
            std::string template_name = actors_doc[i]["template"].GetString();
            if (!std::filesystem::exists("resources/actor_templates/" + template_name + ".template")) {
                std::cout << "error: template " + template_name + " is missing";
                exit(0);
            }
            ReadJsonFile("resources/actor_templates/" + template_name + ".template", doc);
            if (doc.HasMember("name")) {
                actor_ptr->actor_name = doc["name"].GetString();
            }
            if (doc.HasMember("components")) {
                for (rapidjson::Value::ConstMemberIterator itr = doc["components"].MemberBegin(); itr != doc["components"].MemberEnd(); itr++) {
                    std::string component_name = itr->name.GetString();
                    if (doc["components"][component_name.c_str()].HasMember("type")) {
                        std::string component_filename = doc["components"][component_name.c_str()]["type"].GetString();
                        if (component_filename == "Rigidbody") {
                            ComponentManager::CreateRigidbody(component_name, actor_ptr);
                        }
                        else {
                            if (!std::filesystem::exists("resources/component_types/" + component_filename + ".lua")) {
                                std::cout << "error: failed to locate component " + component_filename;
                                exit(0);
                            }
                            ComponentManager::AddComponentsToActor(component_filename, component_name, actor_ptr);
                        }
                    }

                    for (rapidjson::Value::ConstMemberIterator itr2 = doc["components"][component_name.c_str()].MemberBegin();
                        itr2 != doc["components"][component_name.c_str()].MemberEnd(); itr2++) {
                        if (itr2->name.GetString() != "type") {
                            if (itr2->value.IsString()) {
                                (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetString();
                            }
                            else if (itr2->value.IsBool()) {
                                (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetBool();
                            }
                            else if (itr2->value.IsFloat()) {
                                (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetFloat();
                            }
                            else if (itr2->value.IsInt()) {
                                (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetInt();
                            }
                        }
                    }

                    if ((*(actor_ptr->lua_components[component_name]))["Ready"].isFunction()) {
                        (*(actor_ptr->lua_components[component_name]))["Ready"](*(actor_ptr->lua_components[component_name]));
                    }
                }
            }
        }
        if (actors_doc[i].HasMember("name")) {
            actor_ptr->actor_name = actors_doc[i]["name"].GetString();
        }
        if (actors_doc[i].HasMember("components")) {
            for (rapidjson::Value::ConstMemberIterator itr = actors_doc[i]["components"].MemberBegin(); itr != actors_doc[i]["components"].MemberEnd(); itr++) {
                std::string component_name = itr->name.GetString();
                if (actors_doc[i]["components"][component_name.c_str()].HasMember("type")) {
                    std::string component_filename = actors_doc[i]["components"][component_name.c_str()]["type"].GetString();
                    if (component_filename == "Rigidbody") {
                        ComponentManager::CreateRigidbody(component_name, actor_ptr);
                    }
                    else {
                        if (!std::filesystem::exists("resources/component_types/" + component_filename + ".lua")) {
                            std::cout << "error: failed to locate component " + component_filename;
                            exit(0);
                        }
                        ComponentManager::AddComponentsToActor(component_filename, component_name, actor_ptr);
                    }
                }

                for (rapidjson::Value::ConstMemberIterator itr2 = actors_doc[i]["components"][component_name.c_str()].MemberBegin();
                    itr2 != actors_doc[i]["components"][component_name.c_str()].MemberEnd(); itr2++) {
                    if (itr2->name.GetString() != "type") {
                        if (itr2->value.IsString()) {
                            (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetString();
                        }
                        else if (itr2->value.IsBool()) {
                            (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetBool();
                        }
                        else if (itr2->value.IsFloat()) {
                            (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetFloat();
                        }
                        else if (itr2->value.IsInt()) {
                            (*(actor_ptr->lua_components[component_name]))[itr2->name.GetString()] = itr2->value.GetInt();
                        }
                    }
                }

                if ((*(actor_ptr->lua_components[component_name]))["Ready"].isFunction()) {
                    (*(actor_ptr->lua_components[component_name]))["Ready"](*(actor_ptr->lua_components[component_name]));
                }
            }
        }
        
        Scene::actors.push_back(actor_ptr);
    }

    if (!Scene::current_scene_actors.empty()) {
        for (int i = 0; i < Scene::current_scene_actors.size(); i++) {
            if (Scene::current_scene_actors[i]->dontDestroyOnLoad) {
                Scene::actors.push_back(Scene::current_scene_actors[i]);
                Scene::current_scene_actors[i]->fromAnotherScene = true;
            }
        }
    }
    
    if (game_begin_message != "") {
        std::cout << game_begin_message << std::endl;
        game_begin_message = "";
    }
    //scene.mapActorsToPosInitial();
    //scene.RenderScene(SIZE_OF_MAP.x, SIZE_OF_MAP.y);
    scene.CleanUpReferences();
}

void Engine::LoadParticleSystem() {
    /* Load image files in resources/images */
    const std::string particleSystemDirectoryPath = "resources/particle_system";

    // Fills up loadedScenes if the path exists
    if (std::filesystem::exists(particleSystemDirectoryPath)) {
        for (const auto& particleFile : std::filesystem::directory_iterator(particleSystemDirectoryPath)) {
            if (particleFile.path() != particleSystemDirectoryPath + "/.DS_Store")
            {
                std::string fileName = particleFile.path().filename().stem().stem().string();
                rapidjson::Document document;
                ReadJsonFile("resources/particle_system/" + fileName + ".particle", document);
                const rapidjson::Value& emitters = document["emitters"];
                for (rapidjson::SizeType i = 0; i < emitters.Size(); i++) {
                    Emitter* e = nullptr;
                    if (emitters[i].HasMember("type")) {
                        e = ParticleSystem::CreateEmitter(emitters[i]["type"].GetString());
                    }
                    if (emitters[i].HasMember("properties")) {
                        if (!e) {
                            continue;
                        }
                        if (emitters[i]["properties"].HasMember("image_name")) {
                            e->image_name = emitters[i]["properties"]["image_name"].GetString();
                        }
                        if (emitters[i]["properties"].HasMember("angle_range")) {
                            e->angle_range = glm::vec2(emitters[i]["properties"]["angle_range"][0].GetFloat(), emitters[i]["properties"]["angle_range"][1].GetFloat());
                        }
                        if (emitters[i]["properties"].HasMember("rotation_speed")) {
                            e->rotation_speed = emitters[i]["properties"]["rotation_speed"].GetFloat();
                        }
                        if (emitters[i]["properties"].HasMember("speed")) {
                            e->speed = emitters[i]["properties"]["speed"].GetFloat();
                        }
                        if (emitters[i]["properties"].HasMember("start_and_end_size")) {
                            e->start_and_end_size = glm::vec2(emitters[i]["properties"]["start_and_end_size"][0].GetFloat(), emitters[i]["properties"]["start_and_end_size"][1].GetFloat());
                        }
                        if (emitters[i]["properties"].HasMember("emit_number")) {
                            e->emit_number = emitters[i]["properties"]["emit_number"].GetInt();
                        }
                        if (emitters[i]["properties"].HasMember("emit_variance")) {
                            e->emit_number = emitters[i]["properties"]["emit_variance"].GetFloat();
                        }
                        if (emitters[i]["properties"].HasMember("particle_lifespan")) {
                            e->particle_lifes = emitters[i]["properties"]["particle_lifespan"].GetInt();
                        }
                        if (emitters[i]["properties"].HasMember("start_color")) {
                            e->start_color = glm::vec4(emitters[i]["properties"]["start_color"][0].GetFloat(), emitters[i]["properties"]["start_color"][1].GetFloat(),
                                emitters[i]["properties"]["start_color"][2].GetFloat(), emitters[i]["properties"]["start_color"][3].GetFloat());
                        }
                        if (emitters[i]["properties"].HasMember("end_color")) {
                            e->start_color = glm::vec4(emitters[i]["properties"]["end_color"][0].GetFloat(), emitters[i]["properties"]["end_color"][1].GetFloat(),
                                emitters[i]["properties"]["end_color"][2].GetFloat(), emitters[i]["properties"]["end_color"][3].GetFloat());
                        }
                        if (emitters[i]["properties"].HasMember("blend_mode")) {
                            e->blendmode = emitters[i]["properties"]["blend_mode"].GetString();
                        }
                        if (emitters[i]["properties"].HasMember("lifetime")) {
                            e->lifetime = emitters[i]["properties"]["lifetime"].GetInt();
                        }
                    }
                }
            }
        }

    }
}

void Engine::RunOnStart() {
    //ComponentManager::RunOnStart();
    for (int i = 0; i < Scene::actors.size(); i++) {
        if (!Scene::actors[i]->fromAnotherScene) {
            Scene::actors[i]->Start();
        }
    }
}

void Engine::Input()
{
    SDL_Event e;
    while (Helper::SDL_PollEvent498(&e)) {
        Input::ProcessEvent(e);
    }

    if (Input::GetQuit()) {
        std::cout << game_end_bad_message;
        running = false;
    }
}

void Engine::RunAddActors() {
    for (int i = 0; i < scene.actors_to_add.size(); i++) {
        scene.actors_to_add[i]->Start();
        scene.actors.push_back(scene.actors_to_add[i]);
    }
    scene.actors_to_add.clear();
}

void Engine::RunAddComponents() {
    for (int i = 0; i < scene.actors.size(); i++) {
        scene.actors[i]->ProcessAddedComponents();
    }
}

void Engine::RunOnUpdate() {
    for (int i = 0; i < scene.actors.size(); i++) {
        scene.actors[i]->Update();
    }
}

void Engine::RunOnLateUpdate() {
    for (int i = 0; i < scene.actors.size(); i++) {
        scene.actors[i]->LateUpdate();
    }
}

void Engine::RunRemoveCompoenents() {
    for (int i = 0; i < scene.actors.size(); i++) {
        scene.actors[i]->ProcessRemovedComponents();
    }
}

void Engine::RunDestroyActors() {
    for (int i = 0; i < scene.actors_to_destroy.size(); i++) {
        scene.actors_to_destroy[i]->Destroy();
        auto it = std::find(scene.actors.begin(), scene.actors.end(), std::make_shared<Actor>(*scene.actors_to_destroy[i]));
        if (it != scene.actors.end()) {
            scene.actors.erase(it);
        }
    }
    scene.actors_to_destroy.clear();
}

void Engine::Update()
{
    RunAddActors();
    RunAddComponents();
    RunOnUpdate();
    //UpdateCoolDown();
   // scene.IterateActors(renderer);
    RunOnLateUpdate();
    RunDestroyActors();
    RunRemoveCompoenents();
}

void Engine::Render()
{
    std::stable_sort(image_render_requests.begin(), image_render_requests.end(), CompareImageRequests());
    std::stable_sort(ui_render_requests.begin(), ui_render_requests.end(), CompareUIRequests());
    SDL_RenderSetScale(renderer.renderer, renderer.zoom_factor, renderer.zoom_factor);

    HandleRenderImageRequests();

    SDL_RenderSetScale(Renderer::renderer, 1, 1);
    HandleRenderUIRequests();

    HandleRenderTextRequests();

    SDL_SetRenderDrawBlendMode(Renderer::renderer, SDL_BLENDMODE_BLEND);
    HandleRenderPixelRequests();
    SDL_SetRenderDrawBlendMode(Renderer::renderer, SDL_BLENDMODE_NONE);

    //scene.CleanUpReferences();

    //ProcessDialogue();
    //scene.RenderScene(renderer);
    //RenderDialogue();
    //print_dialogue();
}

void Engine::HandleRenderImageRequests() {
    float zoom_factor = Renderer::GetCameraZoomFactor();
    SDL_RenderSetScale(Renderer::renderer, zoom_factor, zoom_factor);

    for (ImageRenderRequest& request : image_render_requests) {
        Renderer::RenderImageRequest(request);
    }

    image_render_requests.clear();
}

void Engine::HandleRenderUIRequests() {
    for (UIRenderRequest &request : ui_render_requests) {
        Renderer::RenderUIRequest(request);
    }
    ui_render_requests.clear();
}


void Engine::HandleRenderTextRequests() {
    for (TextRenderRequest &request : text_render_requests) {
        Renderer::RenderTextRequest(request);
    }
    text_render_requests.clear();
}

void Engine::HandleRenderPixelRequests() {
    for (DrawPixelRequest& request : draw_pixel_requests) {
        Renderer::RenderPixelRequest(request);
    }
    draw_pixel_requests.clear();
}

void Engine::LoadNewScene(const std::string& scene_name) {
    proceed_to_scene = true;
    initial_scene = scene_name;
}

std::string Engine::GetCurrentSceneName() {
    return initial_scene;
}

void Engine::DontDestroyOnLoad(Actor* actor) {
    actor->dontDestroyOnLoad = true;
}