//
//  Engine.hpp
//  game_engine
//
//  Created by 蕭翔群 on 2024/2/1.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <queue>

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Rigidbody.h"
#include "Scene.hpp"
#include "Renderer.h"
#include "Helper.h"
#include "Input.h"
#include "ActorTemplate.hpp"
#include "AudioManager.h"
#include "ComponentManager.h"
#include "rapidjson/include/rapidjson/filereadstream.h"
#include "rapidjson/include/rapidjson/document.h"

class Engine {
public:
    void GameLoop();
    
    static void ReadJsonFile(const std::string& path, rapidjson::Document & out_document)
    {
        FILE* file_pointer = nullptr;
    #ifdef _WIN32
        fopen_s(&file_pointer, path.c_str(), "rb");
    #else
        file_pointer = fopen(path.c_str(), "rb");
    #endif
        char buffer[65536];
        rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
        out_document.ParseStream(stream);
        std::fclose(file_pointer);

        if (out_document.HasParseError()) {
            rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
            std::cout << "error parsing json at [" << path << "]" << std::endl;
            exit(0);
        }
    }
    
    static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase) {
        // Find the position of the phrase in the string
        size_t pos = input.find(phrase);
        // If phrase is not found, return an empty string
        if (pos == std::string::npos) return "";
        // Find the starting position of the next word (skip spaces after the phrase)
        pos += phrase.length();
        while (pos < input.size() && std::isspace(input[pos])) {
            ++pos;
        }
        
        // If we're at the end of the string, return an empty string
        if (pos == input.size()) return "";
        // Find the end position of the word (until a space or the end of the string)
        size_t endPos = pos;
        while (endPos < input.size() && !std::isspace(input[endPos])) {
            ++endPos;
        }
        // Extract and return the word
        return input.substr(pos, endPos - pos);
    }

    struct CompareImageRequests {
        bool operator()(const ImageRenderRequest& lhs, const ImageRenderRequest& rhs) const {
            return lhs.sorting_order < rhs.sorting_order;
        }
    };

    struct CompareUIRequests {
        bool operator()(const UIRenderRequest& lhs, const UIRenderRequest& rhs) const {
            return lhs.sorting_order < rhs.sorting_order;
        }
    };

    /* Requests */
    static inline std::vector<TextRenderRequest> text_render_requests;
    static inline std::vector<UIRenderRequest> ui_render_requests;
    static inline std::vector<ImageRenderRequest> image_render_requests;
    static inline std::vector<DrawPixelRequest> draw_pixel_requests;

    /* Scene related Lua calls */
    static void LoadNewScene(const std::string& scene_name);
    static std::string GetCurrentSceneName();
    static void DontDestroyOnLoad(Actor* actor);

private:
    void LoadResources();
    void LoadScene(std::string& scene_name);
    void LoadParticleSystem();

    void RunOnStart();
    void RunAddActors();
    void RunAddComponents();
    void RunOnUpdate();
    void RunOnLateUpdate();
    void RunRemoveCompoenents();
    void RunDestroyActors();

    void Initialize();
    void Input();
    void Update();
    void Render();

    void HandleRenderImageRequests();
    void HandleRenderUIRequests();
    void HandleRenderTextRequests();
    void HandleRenderPixelRequests();
    
    bool running = true;
    std::string input;
    
    /* Gameplay Related (todo : move to Lua scripts later) */
    int health = 3, score = 0;
    std::string GAME_OVER = "game over";
    std::string YOU_WIN = "you win";
    std::string SCORE_UP = "score up";
    std::string HEALTH_DOWN = "health down";
    bool game_over = false;
    bool win = false;
    static inline bool proceed_to_scene = false;
    
    /* Gameplay Message */
    std::string game_begin_message = "";
    std::string game_end_bad_message = "";
    std::string game_end_good_message = "";
    
    /* Game Manager */
    int game_state = 0; // game state 0: initial, 1: scene, 2: end
    int intro_image_index = 0;
    bool intro_image_specified = false;
    int intro_text_index = 0;
    bool intro_text_specified = false;
    bool intro_bgm_specified = false;

    // cool down after being hit
    bool player_hurt_cooldown = false;
    int hurtFrame = 0;

    // dialogue related
    int score_up = 0;
    int health_down = 0;

    std::vector<std::string> dialogues;

    /* Scene Related */
    static Scene scene;
    static Renderer renderer;
    static inline std::string initial_scene = "";
    std::string game_title = "";
    std::string intro_bgm = "";
};

#endif /* Engine_hpp */
