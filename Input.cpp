#include "Input.h"

void Input::Init() {
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++) {
        keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
    }
    for (int code = 1; code <= 3; code++) {
        mouse_button_states[code] = INPUT_STATE_UP;
    }
}

void Input::ProcessEvent(const SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        quit = true;
    }
    else if (e.type == SDL_KEYDOWN) {
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_scancodes.push_back(e.key.keysym.scancode);
    }
    else if (e.type == SDL_KEYUP) {
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(e.key.keysym.scancode);
    }
    else if (e.type == SDL_MOUSEMOTION) {
        mouse_position = glm::vec2(e.button.x, e.button.y);
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN) {
        mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_buttons.push_back(e.button.button);
    }
    else if (e.type == SDL_MOUSEBUTTONUP) {
        mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_buttons.push_back(e.button.button);
    }
    else if (e.type == SDL_MOUSEWHEEL) {
        mouse_scroll_this_frame = e.wheel.preciseY;
    }
}

void Input::LateUpdate() {
    mouse_scroll_this_frame = 0;

    for (const SDL_Scancode& code : just_became_down_scancodes) {
        keyboard_states[code] = INPUT_STATE_DOWN;
    }
    just_became_down_scancodes.clear();
    for (const SDL_Scancode& code : just_became_up_scancodes) {
        keyboard_states[code] = INPUT_STATE_UP;
    }
    just_became_up_scancodes.clear();
    for (const int code : just_became_down_buttons) {
        mouse_button_states[code] = INPUT_STATE_DOWN;
    }
    just_became_down_buttons.clear();
    for (const int code : just_became_up_buttons) {
        mouse_button_states[code] = INPUT_STATE_UP;
    }
    just_became_up_buttons.clear();
}

bool Input::GetQuit() {
    return quit;
}

bool Input::GetKey(const std::string& keycode) {
    if (__keycode_to_scancode.find(keycode) == __keycode_to_scancode.end()) {
        return false;
    }
    SDL_Scancode scancode = __keycode_to_scancode.at(keycode);
    return keyboard_states[scancode] == INPUT_STATE_DOWN || keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(const std::string& keycode) {
    if (__keycode_to_scancode.find(keycode) == __keycode_to_scancode.end()) {
        return false;
    }
    SDL_Scancode scancode = __keycode_to_scancode.at(keycode);
    return keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(const std::string& keycode) {
    if (__keycode_to_scancode.find(keycode) == __keycode_to_scancode.end()) {
        return false;
    }
    SDL_Scancode scancode = __keycode_to_scancode.at(keycode);
    return keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_UP;
}

luabridge::LuaRef Input::GetMousePosition() {
    luabridge::push(ComponentManager::GetLuaState(), mouse_position);
    luabridge::LuaRef vec2ref(ComponentManager::GetLuaState(), luabridge::Stack<luabridge::LuaRef>::get(ComponentManager::GetLuaState(), lua_gettop(ComponentManager::GetLuaState())));
    lua_pop(ComponentManager::GetLuaState(), 1);
    return vec2ref;
}

bool Input::GetMouseButton(int button) {
    return mouse_button_states[button] == INPUT_STATE_DOWN || mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}
bool Input::GetMouseButtonDown(int button) {
    return mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}
bool Input::GetMouseButtonUp(int button) {
    return mouse_button_states[button] == INPUT_STATE_JUST_BECAME_UP;
}
float Input::GetMouseScrollDelta() {
    return mouse_scroll_this_frame;
}
