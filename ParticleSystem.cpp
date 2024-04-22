#include "ParticleSystem.h"

Particle::Particle() {
	life = 120.0f;
	position = glm::vec2(0.0f, 0.0f);
	velocity = glm::vec2(0.0f, 0.0f);
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	speed = 1.0f;
	image_angle = 0.0f;
	angle = 0.0f;
	lifespan = 120.0f;
	size = 1.0f;

	next = nullptr;
}

void Particle::Init(std::string image_name, glm::vec2 position, float image_angle, float angle, float speed,
	glm::vec2 start_and_end_size, glm::vec2 pivot, float particle_lifes, glm::vec4 start_color, glm::vec4 end_color, std::string blendmode) {
	
	this->image_name = image_name;
	this->position = position;
	this->image_angle = image_angle;
	this->angle = angle;
	this->speed = speed;
	this->start_and_end_size = start_and_end_size;
	this->pivot = pivot;
	life = particle_lifes;
	lifespan = particle_lifes;
	this->start_color = start_color;
	this->end_color = end_color;
	this->blendmode = blendmode;
}

void Particle::Update() {
	float angleRadians = angle * M_PI / 180.0f;
	velocity.x = speed * cos(angleRadians);
	velocity.y = speed * sin(angleRadians);
	//std::cout << velocity.x << " " << velocity.y << std::endl;

	position.x += velocity.x * (1 / 60.0f);
	position.y += velocity.y * (1 / 60.0f);

	float current_life_passed = (lifespan - life) / lifespan;
	size = start_and_end_size.x + (start_and_end_size.y - start_and_end_size.x) * current_life_passed;
	color = glm::mix(start_color, end_color, current_life_passed);

	life--;
}

void Particle::Draw() {
	int r = static_cast<int>(color.x * 255);
	int g = static_cast<int>(color.y * 255);
	int b = static_cast<int>(color.z * 255);
	int a = static_cast<int>(color.w * 255);
	Renderer::DrawEx(image_name, position.x, position.y, image_angle, size, size, pivot.x, pivot.y, r, g, b, a, 1);
}

bool Particle::IsAlive() {
	return life >= 0.0f;
}

Particle* Particle::GetNext() {
	return next;
}

void Particle::SetNext(Particle* particle) {
	next = particle;
}

ParticlePool::~ParticlePool() {
	delete[] particleArray;
}

void ParticlePool::Init(int pool_size) {
	// Fill the pool according to poolSize needed for the emitter
	poolSize = pool_size;
	particleArray = new Particle[poolSize];

	// The first particle is available
	firstAvailable = &particleArray[0];

	// Each particle points to the next one
	for (int i = 0; i < poolSize - 1; i++)
		particleArray[i].SetNext(&particleArray[i + 1]);

	// The last particle points to nullptr indicating the end of the vector
	particleArray[poolSize - 1].SetNext(nullptr);
}

void ParticlePool::Generate(std::string image_name, glm::vec2 position, float image_angle, float angle, float speed,
	glm::vec2 start_and_end_size, glm::vec2 pivot, float particle_lifes, glm::vec4 start_color, glm::vec4 end_color, std::string blendmode)
{
	// Check if the pool is not full
	assert(firstAvailable != nullptr);

	// Remove it from the available list
	if (firstAvailable == nullptr) {
		return;
	}
	Particle* newParticle = firstAvailable;
	firstAvailable = newParticle->GetNext();

	// Initialize new alive particle
	newParticle->Init(image_name, position, image_angle, angle, speed, start_and_end_size, pivot, particle_lifes, start_color, end_color, blendmode);
}

bool ParticlePool::Update()
{
	bool ret = false;

	for (int i = 0; i < poolSize; i++)
	{
		if (particleArray[i].IsAlive())
		{
			particleArray[i].Update();
			particleArray[i].Draw();
			ret = true;
		}
		else // if a particle dies it becomes the first available in the pool
		{
			// Add this particle to the front of the vector
			particleArray[i].SetNext(firstAvailable);
			firstAvailable = &particleArray[i];
		}
	}

	return ret;
}

Emitter::Emitter() {
	id = emitter_count;
	emitter_count++;
	image_rotation_speed = 0.0f;
}

SmokeEmitter::SmokeEmitter() : Emitter() {
	angle_range = glm::vec2(0.0f, 360.0f);
	current_angle = angle_range.x;
	rotation_speed = 0.06f;
	speed = 0.05f;
	start_and_end_size = glm::vec2(1.0f, 1.7f);
	emit_number = 2;
	emit_variance = 4.5f;
	particle_lifes = 200.0f;
	start_color = glm::vec4(0.784f, 0.784f, 0.784f, 0.5f);
	end_color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	blendmode = "none";
	lifetime = -1;

	pool_size = (particle_lifes + 1) * emit_number;
	pool.Init(pool_size);
}

void SmokeEmitter::Update(float pos_x, float pos_y) {

	// Generate phase (Update)
	current_angle += rotation_speed;
	int range = angle_range.y - angle_range.x + 1;
	int offsetNumber = current_angle - angle_range.x;
	int wrappedNumber = ((offsetNumber % range) + range) % range + angle_range.x;
	current_angle = wrappedNumber;

	// Create a random number generator
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator

	// Define the range of float numbers you want to generate
	std::uniform_real_distribution<float> dis(-emit_variance, emit_variance);

	for (int i = 0; i < emit_number; i++) {
		glm::vec2 pivot = glm::vec2(dis(gen), dis(gen));
		pool.Generate(image_name, glm::vec2(pos_x + pivot.x, pos_y + pivot.y), 0.0f, current_angle, speed, start_and_end_size, pivot, particle_lifes, start_color, end_color, blendmode);
	}

	// Update phase (Render)
	pool.Update();
}

FireEmitter::FireEmitter() : Emitter() {
	angle_range = glm::vec2(-100.0f, -80.0f);
	current_angle = angle_range.x;
	rotation_speed = 2.4f;
	speed = 0.8f;
	start_and_end_size = glm::vec2(1.8f, 0.6f);
	emit_number = 2;
	emit_variance = 1.0f;
	particle_lifes = 120.0f;
	start_color = glm::vec4(1.0f, 0.647f, 0.0f, 1.0f);
	end_color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	blendmode = "additive";
	lifetime = -1;

	pool_size = (particle_lifes + 1) * emit_number;
	pool.Init(pool_size);
}

void FireEmitter::Update(float pos_x, float pos_y) {

	// Generate phase (Update)
	current_angle += rotation_speed;
	int range = angle_range.y - angle_range.x + 1;
	int offsetNumber = current_angle - angle_range.x;
	int wrappedNumber = ((offsetNumber % range) + range) % range + angle_range.x;
	current_angle = wrappedNumber;

	// Create a random number generator
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator

	// Define the range of float numbers you want to generate
	std::uniform_real_distribution<float> dis(-emit_variance, emit_variance);

	for (int i = 0; i < emit_number; i++) {
		glm::vec2 pivot = glm::vec2(dis(gen), dis(gen));
		pool.Generate(image_name, glm::vec2(pos_x + pivot.x, pos_y + pivot.y), 0.0f, current_angle, speed, start_and_end_size, pivot, particle_lifes, start_color, end_color, blendmode);
	}

	// Update phase (Render)
	pool.Update();
}

LaserEmitter::LaserEmitter() : Emitter() {
	angle_range = glm::vec2(0.0f, 0.0f);
	current_angle = angle_range.x;
	rotation_speed = 0.0f;
	speed = 50.0f;
	start_and_end_size = glm::vec2(2.0f, 2.0f);
	emit_number = 1;
	emit_variance = 0.01f;
	particle_lifes = 60.0f;
	start_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	end_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	blendmode = "additive";
	lifetime = -1;

	pool_size = (particle_lifes + 1) * emit_number;
	pool.Init(pool_size);
}

void LaserEmitter::Update(float pos_x, float pos_y) {

	// Generate phase (Update)
	current_angle += rotation_speed;
	int range = angle_range.y - angle_range.x + 1;
	int offsetNumber = current_angle - angle_range.x;
	int wrappedNumber = ((offsetNumber % range) + range) % range + angle_range.x;
	current_angle = wrappedNumber;

	// Create a random number generator
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator

	// Define the range of float numbers you want to generate
	std::uniform_real_distribution<float> dis(-emit_variance, emit_variance);

	for (int i = 0; i < emit_number; i++) {
		glm::vec2 pivot = glm::vec2(dis(gen), dis(gen));
		pool.Generate(image_name, glm::vec2(pos_x + pivot.x, pos_y + pivot.y), 0.0f, current_angle, speed, start_and_end_size, pivot, particle_lifes, start_color, end_color, blendmode);
	}

	// Update phase (Render)
	pool.Update();
}

SparkleEmitter::SparkleEmitter() : Emitter() {
	angle_range = glm::vec2(-120.0f, -60.0f);
	current_angle = angle_range.x;
	image_rotation_speed = 3.0f;
	rotation_speed = 3.0f;
	speed = 2.0f;
	start_and_end_size = glm::vec2(2.5f, 1.0f);
	emit_number = 4;
	emit_variance = 0.3f;
	rotation_variance = 260.0f;
	particle_lifes = 60.0f;
	start_color = glm::vec4(0.784f, 0.784f, 0.784f, 0.5f);
	end_color = glm::vec4(0.502f, 0.502f, 0.502f, 0.0f);
	blendmode = "additive";
	lifetime = -1;

	pool_size = (particle_lifes + 1) * emit_number;
	pool.Init(pool_size);
}

void SparkleEmitter::Update(float pos_x, float pos_y) {

	// Generate phase (Update)
	current_angle += rotation_speed;
	current_image_angle += image_rotation_speed;
	int range = angle_range.y - angle_range.x + 1;
	int offsetNumber = current_angle - angle_range.x;
	int wrappedNumber = ((offsetNumber % range) + range) % range + angle_range.x;
	current_angle = wrappedNumber;

	// Create a random number generator
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator

	// Define the range of float numbers you want to generate
	std::uniform_real_distribution<float> dis(-emit_variance, emit_variance);
	std::uniform_real_distribution<float> dis2(0, rotation_variance);

	for (int i = 0; i < emit_number; i++) {
		current_image_angle += dis2(gen);
		glm::vec2 pivot = glm::vec2(dis(gen), dis(gen));
		pool.Generate(image_name, glm::vec2(pos_x + pivot.x, pos_y + pivot.y), current_image_angle, current_angle, speed, start_and_end_size, pivot, particle_lifes, start_color, end_color, blendmode);
	}

	// Update phase (Render)
	pool.Update();
}

void ParticleSystem::UpdateEmitter(float pos_x, float pos_y, int emitter_id) {
	for (auto it = emitters.begin(); it != emitters.end(); it++) {
		if ((it->second)->id == emitter_id) {
			(it->second)->Update(pos_x, pos_y);
			break;
		}
	}
}

void ParticleSystem::UpdateEmitters(float pos_x, float pos_y) {
	for (auto it = emitters.begin(); it != emitters.end(); it++) {
		(it->second)->Update(pos_x, pos_y);
	}
}

Emitter* ParticleSystem::CreateEmitter(const std::string & type) {
	if (type == "Smoke") {
		auto emitter = std::make_shared<SmokeEmitter>();
		emitters[emitter->id] = emitter;
		return emitters[emitter->id].get();
	}
	else if (type == "Fire") {
		auto emitter = std::make_shared<FireEmitter>();
		emitters[emitter->id] = emitter;
		return emitters[emitter->id].get();
	}
	else if (type == "Laser") {
		auto emitter = std::make_shared<LaserEmitter>();
		emitters[emitter->id] = emitter;
		return emitters[emitter->id].get();
	}
	else if (type == "Sparkle") {
		auto emitter = std::make_shared<SparkleEmitter>();
		emitters[emitter->id] = emitter;
		return emitters[emitter->id].get();
	}
	else {
		return nullptr;
	}
}

void ParticleSystem::RemoveEmitter(Emitter* emitter) {
	emitters.erase(emitter->id);
}

void ParticleSystem::RemoveAllEmitter() {
	emitters.clear();
}