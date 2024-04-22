#define ParticleManager_h
#ifdef ParticleManager_h

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <random>
#include <cassert>
#include <iostream>
#include "box2d/box2d.h"
#include "glm/glm.hpp"
#include "Renderer.h"
#include "SDL2_image/SDL_image.h"

class Emitter;

class Particle {

private:
	float life;
	std::string image_name;
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec4 color;
	glm::vec2 pivot;
	float speed;
	float image_angle;
	float angle;
	float lifespan;
	float size;

	glm::vec2 start_and_end_size;
	glm::vec4 start_color;
	glm::vec4 end_color;
	std::string blendmode;

	Particle* next;

public:

	Particle();

	void Init(std::string image_name, glm::vec2 position, float image_angle, float angle, float speed,
		glm::vec2 start_and_end_size, glm::vec2 pivot, float particle_lifes, glm::vec4 start_color, glm::vec4 end_color, std::string blendmode);

	void Update();

	void Draw();

	bool IsAlive();

	Particle* GetNext();

	void SetNext(Particle* particle);
};

class ParticlePool
{
private:

	int poolSize = 0;
	Particle* firstAvailable;
	Particle* particleArray = nullptr;

public:

	~ParticlePool();

	// Initialize Pool
	void Init(int pool_size);

	//Generates a new particle each time it's called
	void Generate(std::string image_name, glm::vec2 position, float image_angle, float angle, float speed,
		glm::vec2 start_and_end_size, glm::vec2 pivot, float particle_lifes, glm::vec4 start_color, glm::vec4 end_color, std::string blendmode);

	// Update (move and draw) particles in the pool. If there are no particles alive returns false
	bool Update();
};

class Emitter {
private:

	static inline int emitter_count = 0;

public:

	/* Unique ID of the Emitter */
	int id;

	/* The texture of the particle - we use the same texture per Emitter */
	std::string image_name;

	/* Angle spectrum of the particle flow. */
	glm::vec2 angle_range;

	/* How fast you want the texture to spin. */
	float rotation_speed;

	/* How fast you want the texture to spin itself. */
	float image_rotation_speed;

	/* How fast particle move. */
	float speed;

	/* Particle size when born and when die. */
	glm::vec2 start_and_end_size;

	/* How many particles are generated per frame. */
	int emit_number;

	/* An offset to randomize particle generation. */
	float emit_variance;

	/* How many frames a particle lives. */
	int particle_lifes;

	/* Start and end color of the particle. We use linear interpolation for this. */
	glm::vec4 start_color;
	glm::vec4 end_color;

	/* How colors interact wich each other. We can create glow of effects using additive blending in SDL. */
	std::string blendmode;

	/* How many frames an emitter lives. -1 means forever */
	int lifetime;

	/* Particle Pool */
	ParticlePool pool;

	/* pool size*/
	int pool_size;

	/* Initialize Default values */
	Emitter();

	/* Update the Emitter*/
	virtual void Update(float pos_x, float pos_y) = 0;
};

class SmokeEmitter : public Emitter{
private:
	float current_angle;

public:

	SmokeEmitter();

	void Update(float pos_x, float pos_y) override;
};

class FireEmitter : public Emitter {
private:
	float current_angle;

public:

	FireEmitter();

	void Update(float pos_x, float pos_y) override;

};

class LaserEmitter : public Emitter {
private:
	float current_angle;

public:

	LaserEmitter();

	void Update(float pos_x, float pos_y) override;

};

class SparkleEmitter : public Emitter {
private:
	float current_angle;
	float current_image_angle;
	float rotation_variance;

public:

	SparkleEmitter();

	void Update(float pos_x, float pos_y) override;

};

class ParticleSystem {

private:

	static inline std::unordered_map<int, std::shared_ptr<Emitter>> emitters;

public:

	static void UpdateEmitter(float pos_x, float pos_y, int emitter_id);

	static void UpdateEmitters(float pos_x, float pos_y);

	static Emitter* CreateEmitter(const std::string& type);

	static void RemoveEmitter(Emitter* emitter);

	static void RemoveAllEmitter();
	
};

//class ParticleManager
//{
//	// Initializes the particle manager.
//	static void Init();
//
//	// Adds a specified particle system to the manager.
//	static void AddSystem();
//
//	// Removes a specified particle system.
//	static void RemoveSystem();
//
//	// Updates all active particles systems and removes all system which died after the update.
//	static void Update();
//
//	// Renders all active and visible systems.
//	static void Render();
//
//	// Shuts down the manager(removes all allocated systems).
//	static void ShutDown();
//
//	// Checks whether a given particle system will exists in the particle manager (if it has not been removed yet).
//	static bool DoesExist();
//};

#endif

