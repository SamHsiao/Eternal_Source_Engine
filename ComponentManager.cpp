#include "ComponentManager.h"

#include "Scene.hpp"
#include "Engine.hpp"

lua_State* ComponentManager::lua_state = NULL;
std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> ComponentManager::component_tables = {};
//std::vector<std::shared_ptr<luabridge::LuaRef>> ComponentManager::on_start_calling = {};

lua_State* ComponentManager::GetLuaState() {
	return lua_state;
}

void ComponentManager::Initialize() {
	InitializeState();
	InitializeFunctions();
	InitializeComponents();
}

void ComponentManager::InitializeState() {
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
}

void ComponentManager::InitializeFunctions() {
	// Debug: Debug.Log, Debug.LogError
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", CppLog)
		.addFunction("LogError", CppLogError)
		.endNamespace();

	// glm::vec2
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<glm::vec2>("vec2")
		.addConstructor<void(*)(float, float)>()
		.addData("x", &glm::vec2::x)
		.addData("y", &glm::vec2::y)
		.endClass();

	// glm::vec4 (for the use of Color)
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<glm::vec4>("Color")
		.addConstructor<void(*)(float, float, float, float)>()
		.addData("r", &glm::vec4::r)
		.addData("g", &glm::vec4::g)
		.addData("b", &glm::vec4::b)
		.addData("a", &glm::vec4::a)
		.endClass();

	// Application
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Application")
		.addFunction("Quit", Quit)
		.addFunction("Sleep", Sleep)
		.addFunction("GetFrame", GetFrame)
		.addFunction("OpenURL", OpenURL)
		.endNamespace();

	// Actor:GetName, Actor:GetID, Actor:GetComponentByKey, Actor:GetComponent, Actor:GetComponents, Actor:AddComponent, Actor:RemoveComponent
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::GetName)
		.addFunction("GetID", &Actor::GetID)
		.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
		.addFunction("GetComponent", &Actor::GetComponent)
		.addFunction("GetComponents", &Actor::GetComponents)
		.addFunction("AddComponent", &Actor::LuaAddComponent)
		.addFunction("RemoveComponent", &Actor::LuaRemoveComponent)
		.endClass();

	// Actor.Find, Actor.FindAll
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Actor")
		.addFunction("Find", &Scene::FindActor)
		.addFunction("FindAll", &Scene::FindAllActors)
		.addFunction("Instantiate", &Scene::Instantiate)
		.addFunction("Destroy", &Scene::Destroy)
		.endNamespace();

	// Input
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Input")
		.addFunction("GetKey", &Input::GetKey)
		.addFunction("GetKeyDown", &Input::GetKeyDown)
		.addFunction("GetKeyUp", &Input::GetKeyUp)
		.addFunction("GetMousePosition", &Input::GetMousePosition)
		.addFunction("GetMouseButton", &Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
		.addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
		.endNamespace();

	// Text
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Text")
		.addFunction("Draw", &Renderer::DrawText)
		.endNamespace();

	// Audio
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Audio")
		.addFunction("Play", &AudioManager::PlayAudio)
		.addFunction("Halt", &AudioManager::StopAudio)
		.addFunction("SetVolume", &AudioManager::SetVolume)
		.endNamespace();

	// Image
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Image")
		.addFunction("DrawUI", &Renderer::DrawUI)
		.addFunction("DrawUIEx", &Renderer::DrawUIEx)
		.addFunction("Draw", &Renderer::Draw)
		.addFunction("DrawEx", &Renderer::DrawEx)
		.addFunction("DrawPixel", &Renderer::DrawPixel)
		.endNamespace();

	// Camera
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Camera")
		.addFunction("SetPosition", &Renderer::SetCameraPosition)
		.addFunction("GetPositionX", &Renderer::GetCameraPositionX)
		.addFunction("GetPositionY", &Renderer::GetCameraPositionY)
		.addFunction("SetZoom", &Renderer::SetCameraZoom)
		.addFunction("GetZoom", &Renderer::GetCameraZoom)
		.endNamespace();

	// Scene
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Scene")
		.addFunction("Load", &Engine::LoadNewScene)
		.addFunction("GetCurrent", &Engine::GetCurrentSceneName)
		.addFunction("DontDestroy", &Engine::DontDestroyOnLoad)
		.endNamespace();

	// Vector2
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<b2Vec2>("Vector2")
		.addConstructor<void(*) (float, float)>()
		.addData("x", &b2Vec2::x)
		.addData("y", &b2Vec2::y)
		.addFunction("Normalize", &b2Vec2::Normalize)
		.addFunction("Length", &b2Vec2::Length)
		.addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.addStaticFunction("Distance", &b2Distance)
		.addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
		.endClass();

	// Rigidbody
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Rigidbody>("Rigidbody")
		.addData("enabled", &Rigidbody::enabled)
		.addData("key", &Rigidbody::key)
		.addData("type", &Rigidbody::type)
		.addData("actor", &Rigidbody::actor)
		.addData("x", &Rigidbody::x)
		.addData("y", &Rigidbody::y)
		.addData("body_type", &Rigidbody::body_type)
		.addData("precise", &Rigidbody::precise)
		.addData("gravity_scale", &Rigidbody::gravity_scale)
		.addData("density", &Rigidbody::density)
		.addData("angular_friction", &Rigidbody::angular_friction)
		.addData("rotation", &Rigidbody::rotation)
		.addData("has_collider", &Rigidbody::has_collider)
		.addData("has_trigger", &Rigidbody::has_trigger)
		.addData("collider_type", &Rigidbody::collider_type)
		.addData("width", &Rigidbody::width)
		.addData("height", &Rigidbody::height)
		.addData("radius", &Rigidbody::radius)
		.addData("friction", &Rigidbody::friction)
		.addData("bounciness", &Rigidbody::bounciness)
		.addData("trigger_type", &Rigidbody::trigger_type)
		.addData("trigger_width", &Rigidbody::trigger_width)
		.addData("trigger_height", &Rigidbody::trigger_height)
		.addData("trigger_radius", &Rigidbody::trigger_radius)
		.addFunction("Ready", &Rigidbody::Ready)
		.addFunction("GetPosition", &Rigidbody::GetPosition)
		.addFunction("GetRotation", &Rigidbody::GetRotation)
		.addFunction("AddForce", &Rigidbody::AddForce)
		.addFunction("SetVelocity", &Rigidbody::SetVelocity)
		.addFunction("SetPosition", &Rigidbody::SetPosition)
		.addFunction("SetRotation", &Rigidbody::SetRotation)
		.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
		.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
		.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
		.addFunction("GetVelocity", &Rigidbody::GetVelocity)
		.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
		.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
		.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
		.addFunction("OnStart", &Rigidbody::Start)
		.addFunction("OnDestroy", &Rigidbody::Destroy)
		.endClass();
	
	// Collision
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Collision>("Collision")
		.addData("other", &Collision::other)
		.addData("point", &Collision::point)
		.addData("relative_velocity", &Collision::relative_velocity)
		.addData("normal", &Collision::normal)
		.endClass();

	// Physics.Raycast, Physics.RaycastAll
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Physics")
		.addFunction("Raycast", &Raycast::PhysicsRaycast)
		.addFunction("RaycastAll", &Raycast::PhysicsRaycastAll)
		.endNamespace();

	// HitResult
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<HitResult>("HitResult")
		.addData("actor", &HitResult::actor)
		.addData("point", &HitResult::point)
		.addData("normal", &HitResult::normal)
		.addData("is_trigger", &HitResult::is_trigger)
		.endClass();

	// Event.Publish, Event.Subscribe, Event.Unsubscribe
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Event")
		.addFunction("Publish", &EventBus::Publish)
		.addFunction("Subscribe", &EventBus::Subscribe)
		.addFunction("Unsubscribe", &EventBus::Unsubscribe)
		.endNamespace();

	// Particle
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Emitter>("Emitter")
		.addData("angle_range", &Emitter::angle_range)
		.addData("blendmode", &Emitter::blendmode)
		.addData("start_color", &Emitter::start_color)
		.addData("end_color", &Emitter::end_color)
		.addData("emit_number", &Emitter::emit_number)
		.addData("emit_variance", &Emitter::emit_variance)
		.addData("start_and_end_size", &Emitter::start_and_end_size)
		.addData("image_name", &Emitter::image_name)
		.addData("lifetime", &Emitter::lifetime)
		.addData("particle_lifes", &Emitter::particle_lifes)
		.addData("rotation_speed", &Emitter::rotation_speed)
		.addData("speed", &Emitter::speed)
		.endClass();

	// ParticleSystem
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("ParticleSystem")
		.addFunction("UpdateEmitter", &ParticleSystem::UpdateEmitter)
		.addFunction("UpdateEmitters", &ParticleSystem::UpdateEmitters)
		.addFunction("CreateEmitter", &ParticleSystem::CreateEmitter)
		.addFunction("RemoveEmitter", &ParticleSystem::RemoveEmitter)
		.addFunction("RemoveAllEmitter", &ParticleSystem::RemoveAllEmitter)
		.endNamespace();
}

void ComponentManager::InitializeComponents() {
	/* Load image files in resources/component_types/ */
	const std::string luaDirectoryPath = "resources/component_types";

	// Fills up loadedScenes if the path exists
	if (std::filesystem::exists(luaDirectoryPath)) {
		for (const auto& entry : std::filesystem::directory_iterator(luaDirectoryPath)) {
			if (luaL_dofile(lua_state, entry.path().string().c_str()) != LUA_OK) {
				std::cout << "problem with lua file " + entry.path().stem().string();
				exit(0);
			}

			std::string component_name = entry.path().stem().string();
			component_tables.insert({ component_name, 
				std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(lua_state, component_name.c_str()))
				});

			(*component_tables[component_name])["key"] = component_name;
			(*component_tables[component_name])["type"] = component_name;
			(*component_tables[component_name])["enabled"] = true;
		}
	}
}

std::shared_ptr<luabridge::LuaRef> ComponentManager::SetComponent(const std::string& component_type, const std::string& component_key) {
	luabridge::LuaRef instance_table = luabridge::newTable(lua_state);
	EstablishInheritance(instance_table, *component_tables[component_type]);
	instance_table["key"] = component_key;
	instance_table["enabled"] = true;
	return std::make_shared<luabridge::LuaRef>(instance_table);
}

std::shared_ptr<luabridge::LuaRef> ComponentManager::SetRigidbody(const std::string& component_key) {
	Rigidbody* rigidbody = new Rigidbody();
	luabridge::LuaRef componentRef(lua_state, rigidbody);

	std::shared_ptr<luabridge::LuaRef> rigidbodyRef = std::make_shared<luabridge::LuaRef>(componentRef);
	(*rigidbodyRef)["key"] = component_key;
	(*rigidbodyRef)["enabled"] = true;
	return rigidbodyRef;
}

void ComponentManager::AddComponentsToActor(const std::string& lua_file, const std::string& component_name, Actor* actor) {
	luabridge::LuaRef instance_table = luabridge::newTable(lua_state);
	actor->lua_components[component_name] = std::make_shared<luabridge::LuaRef>(instance_table);
	EstablishInheritance(instance_table, *component_tables[lua_file]);
	(*(actor->lua_components[component_name]))["key"] = component_name;
	(*(actor->lua_components[component_name]))["enabled"] = true;
	actor->InjectConvenienceReferences(actor->lua_components[component_name]);
}

void ComponentManager::AddComponentsToActor(const std::string& lua_file, const std::string& component_name, std::shared_ptr<Actor> actor) {
	luabridge::LuaRef instance_table = luabridge::newTable(lua_state);
	actor->lua_components[component_name] = std::make_shared<luabridge::LuaRef>(instance_table);
	EstablishInheritance(instance_table, *component_tables[lua_file]);
	(*(actor->lua_components[component_name]))["key"] = component_name;
	(*(actor->lua_components[component_name]))["enabled"] = true;
	actor->InjectConvenienceReferences(actor->lua_components[component_name]);
}

void ComponentManager::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table) {

	/* We must create a metatable to establish inheritance in lua. */
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = parent_table;

	/* We must use the raw lua C-API (lua stack) to perform a "setmetatable" operation. */
	instance_table.push(lua_state);
	new_metatable.push(lua_state);
	lua_setmetatable(lua_state, -2);
	lua_pop(lua_state, 1);
}

std::shared_ptr<luabridge::LuaRef> ComponentManager::CreateRigidbody(const std::string& component_name, std::shared_ptr<Actor> actor) {
	Rigidbody* rigidbody = new Rigidbody();
	luabridge::LuaRef componentRef(lua_state, rigidbody);

	actor->lua_components[component_name] = std::make_shared<luabridge::LuaRef>(componentRef);
	(*(actor->lua_components[component_name]))["key"] = component_name;
	(*(actor->lua_components[component_name]))["enabled"] = true;
	actor->InjectConvenienceReferences(actor->lua_components[component_name]);
	return actor->lua_components[component_name];
}

std::shared_ptr<luabridge::LuaRef> ComponentManager::CreateRigidbody(const std::string& component_name, Actor* actor) {
	Rigidbody* rigidbody = new Rigidbody();
	luabridge::LuaRef componentRef(lua_state, rigidbody);

	actor->lua_components[component_name] = std::make_shared<luabridge::LuaRef>(componentRef);
	(*(actor->lua_components[component_name]))["key"] = component_name;
	(*(actor->lua_components[component_name]))["enabled"] = true;
	actor->InjectConvenienceReferences(actor->lua_components[component_name]);
	return actor->lua_components[component_name];
}

void ComponentManager::CppLog(const std::string& message) {
	std::cout << message << std::endl;
}

void ComponentManager::CppLogError(const std::string& message) {
	std::cerr << message << std::endl;
}

void ComponentManager::Quit() {
	exit(0);
}

void ComponentManager::Sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int ComponentManager::GetFrame() {
	return Helper::GetFrameNumber();
}

void ComponentManager::OpenURL(const std::string& url) {
	std::string cmdline;
#ifdef _WIN32
	cmdline = "start " + url;
	// Windows-specific code here
#elif __APPLE__
	cmdline = "open " + url;
	// macOS-specific code here
#else
	cmdline = "xdg-open " + url;
	// Linux-specific code here
#endif
	std::system(cmdline.c_str());
}