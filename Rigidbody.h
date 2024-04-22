#ifndef Rigidbody_h
#define Rigidbody_h

#include "Actor.hpp"
#include "box2d/box2d.h"
#include "glm/glm.hpp"

class CollisionDetector : public b2ContactListener {
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
};

class Rigidbody
{
public:

	static inline bool world_initialized = false;
	static inline b2World* physics_world;
	
	static inline const float TIME_STEP = 1.0f / 60.0f;
	static inline const int VELOCITY_ITERATIONS = 8;
	static inline const int POSITION_ITERATIONS = 3;

	static inline CollisionDetector detector;

	/* Component default properties */
	std::string type = "Rigidbody";
	std::string key = "???";
	Actor* actor = nullptr;
	bool enabled = true;

	/* Rigidbody properties */
	bool created = false;
	float x = 0.0f;
	float y = 0.0f;
	std::string body_type = "dynamic";
	bool precise = true;
	float gravity_scale = 1.0f;
	float density = 1.0f;
	float angular_friction = 0.3f;
	float rotation = 0.0f;
	bool has_collider = true;
	bool has_trigger = true;

	/* collider properties */
	std::string collider_type = "box";
	float width = 1.0f;
	float height = 1.0f;
	float radius = 0.5f;
	float friction = 0.3f;
	float bounciness = 0.3f;

	/* trigger properties */
	std::string trigger_type = "box";
	float trigger_width = 1.0f;
	float trigger_height = 1.0f;
	float trigger_radius = 0.5f;

	b2Body* body;

	static void PhysicsStep();

	void CreateB2Body();

	void CreateColliderFixture();

	void CreateTriggerFixture();

	void CreateDefaultFixture();

	/* lua functions */

	void Ready();

	b2Vec2 GetPosition();

	float GetRotation();

	void AddForce(b2Vec2 force);

	void SetVelocity(b2Vec2 velocity);

	void SetPosition(b2Vec2 position);

	void SetRotation(float degrees_clockwise);

	void SetAngularVelocity(float degrees_clockwise);

	void SetGravityScale(float scale);

	void SetUpDirection(b2Vec2 direction);

	void SetRightDirection(b2Vec2 direction);

	b2Vec2 GetVelocity();

	float GetAngularVelocity();

	float GetGravityScale();

	b2Vec2 GetUpDirection();

	b2Vec2 GetRightDirection();

	void Start();

	void Destroy();
};

#endif

