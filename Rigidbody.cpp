#include "Rigidbody.h"

void CollisionDetector::BeginContact(b2Contact* contact) {
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
	Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);

	// It means that at least one of them are a phantom shape
	if (actorA == nullptr || actorB == nullptr) {
		return;
	}

	Collision collision;

	b2WorldManifold world_manifold;
	contact->GetWorldManifold(&world_manifold);

	collision.point = world_manifold.points[0];
	collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
	collision.normal = world_manifold.normal;
	collision.other = actorB;

	if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
		collision.point = b2Vec2(-999.0f, -999.0f);
		collision.normal = b2Vec2(-999.0f, -999.0f);

		actorA->OnTriggerEnter(collision);

		collision.other = actorA;
		actorB->OnTriggerEnter(collision);
	}
	else if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
		actorA->OnCollisionEnter(collision);
		
		collision.other = actorA;
		actorB->OnCollisionEnter(collision);
	}
}

void CollisionDetector::EndContact(b2Contact* contact) {
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
	Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);

	// It means that at least one of them are a phantom shape
	if (actorA == nullptr || actorB == nullptr) {
		return;
	}

	Collision collision;
	collision.point = b2Vec2(-999.0f, -999.0f);
	collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
	collision.normal = b2Vec2(-999.0f, -999.0f);
	collision.other = actorB;

	if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
		actorA->OnTriggerExit(collision);

		collision.other = actorA;
		actorB->OnTriggerExit(collision);
	}
	else if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
		actorA->OnCollisionExit(collision);

		collision.other = actorA;
		actorB->OnCollisionExit(collision);
	}
}

void Rigidbody::PhysicsStep() {
	if (world_initialized) {
		physics_world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	}
}

void Rigidbody::Ready() {
	if (created) {
		return;
	}
	created = true;
	
	if (!world_initialized) {
		physics_world = new b2World(b2Vec2(0.0f, 9.8f));
		physics_world->SetContactListener(&detector);

		world_initialized = true;
	}

	CreateB2Body();
	if (has_collider) {
		CreateColliderFixture();
	}
	if (has_trigger) {
		CreateTriggerFixture();
	}

	if (!has_collider && !has_trigger) {
		CreateDefaultFixture();
	}
}

void Rigidbody::CreateB2Body() {
	b2BodyDef body_def;
	if (body_type == "dynamic") {
		body_def.type = b2_dynamicBody;
	}
	else if (body_type == "kinematic") {
		body_def.type = b2_kinematicBody;
	}
	else if (body_type == "static") {
		body_def.type = b2_staticBody;
	}

	body_def.position = b2Vec2(x, y);
	body_def.bullet = precise;
	body_def.gravityScale = gravity_scale;
	body_def.angularDamping = angular_friction;
	body_def.angle = rotation * (b2_pi / 180.0f);

	body = physics_world->CreateBody(&body_def);
}

void Rigidbody::CreateColliderFixture() {
	b2Shape* shape = nullptr;
	if (collider_type == "box") {
		b2PolygonShape* polygon_shape = new b2PolygonShape();
		polygon_shape->SetAsBox(width * 0.5f, height * 0.5f);
		shape = polygon_shape;
	}
	else if (collider_type == "circle") {
		b2CircleShape* circle_shape = new b2CircleShape();
		circle_shape->m_radius = radius;
		shape = circle_shape;
	}

	b2FixtureDef fixture_def;
	fixture_def.shape = shape;
	fixture_def.density = density;
	fixture_def.isSensor = false;
	fixture_def.restitution = bounciness;
	fixture_def.friction = friction;
	fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);

	body->CreateFixture(&fixture_def);
}

void Rigidbody::CreateTriggerFixture() {
	b2Shape* shape = nullptr;
	if (trigger_type == "box") {
		b2PolygonShape* polygon_shape = new b2PolygonShape();
		polygon_shape->SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
		shape = polygon_shape;
	}
	else if (trigger_type == "circle") {
		b2CircleShape* circle_shape = new b2CircleShape();
		circle_shape->m_radius = trigger_radius;
		shape = circle_shape;
	}

	b2FixtureDef fixture_def;
	fixture_def.shape = shape;
	fixture_def.density = density;
	fixture_def.isSensor = true;
	fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);

	body->CreateFixture(&fixture_def);
}

void Rigidbody::CreateDefaultFixture() {
	b2PolygonShape phantom_shape;
	phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &phantom_shape;
	fixture.density = density;

	// Because it is a sensor (with no callback even), no collisions will ever occur
	fixture.isSensor = true;
	body->CreateFixture(&fixture);
}

b2Vec2 Rigidbody::GetPosition() {
	if (body == nullptr) {
		return b2Vec2(x, y);
	}
	return body->GetPosition();
}

float Rigidbody::GetRotation() {
	return body->GetAngle() * (180.0f / b2_pi);
}

void Rigidbody::AddForce(b2Vec2 force) {
	body->ApplyForceToCenter(force, true);
}

void Rigidbody::SetVelocity(b2Vec2 velocity) {
	body->SetLinearVelocity(velocity);
}

void Rigidbody::SetPosition(b2Vec2 position) {
	if (body == nullptr) {
		x = position.x;
		y = position.y;
	}
	else {
		body->SetTransform(position, body->GetAngle());
	}
}

void Rigidbody::SetRotation(float degrees_clockwise) {
	body->SetTransform(body->GetPosition(), degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetAngularVelocity(float degrees_clockwise) {
	body->SetAngularVelocity(degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetGravityScale(float scale) {
	body->SetGravityScale(scale);
}

void Rigidbody::SetUpDirection(b2Vec2 direction) {
	direction.Normalize();
	float up_direction = glm::atan(direction.x, -direction.y);
	body->SetTransform(body->GetPosition(), up_direction);
}

void Rigidbody::SetRightDirection(b2Vec2 direction) {
	direction.Normalize();
	float right_direction = glm::atan(direction.x, -direction.y) - b2_pi / 2.0f;
	body->SetTransform(body->GetPosition(), right_direction);
}

b2Vec2 Rigidbody::GetVelocity() {
	return body->GetLinearVelocity();
}

float Rigidbody::GetAngularVelocity() {
	return body->GetAngularVelocity() * (180.0f / b2_pi);
}

float Rigidbody::GetGravityScale() {
	return body->GetGravityScale();
}

b2Vec2 Rigidbody::GetUpDirection() {
	float angle = body->GetAngle();
	b2Vec2 result = b2Vec2(glm::sin(angle), -glm::cos(angle));
	result.Normalize();
	return result;
}

b2Vec2 Rigidbody::GetRightDirection() {
	float angle = body->GetAngle();
	b2Vec2 result = b2Vec2(glm::cos(angle), glm::sin(angle));
	result.Normalize();
	return result;
}

void Rigidbody::Start() {
	Ready();
}

void Rigidbody::Destroy() {
	physics_world->DestroyBody(body);
}