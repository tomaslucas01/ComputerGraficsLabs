#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "framework/../extra/picopng.h"
#include "framework/image.h"
#include "framework/utils.h"
#include "framework/camera.h"
#include "framework/mesh.h"
#include "ParticleSystem.h"
#include "framework.h"

void ParticleSystem::Init(int width, int height) { //Initializes the particles
	for (int i = 0; i < MAX_PARTICLES; i++) {
		int temp = rand() % 255; //Random color (between white and black)
		particles[i].position = Vector2(rand() % width, height + rand() % MAX_PARTICLES); //Sets the particle in a random position
		particles[i].velocity = Vector2(1, -0.5 - (1.0 / 25) * (rand() % 25));
		particles[i].color = Color(temp, temp, temp);
		particles[i].acceleration = 0;
		particles[i].ttl = 5 + rand() % 20;
		particles[i].inactive = false;
	}
}

void ParticleSystem::Render(Image* framebuffer) {
	for (int i = 0; i < MAX_PARTICLES; i++) { //Draws each particle as a small rectangle
		framebuffer->DrawRect(particles[i].position.x, particles[i].position.y, 3, 3, particles[i].color, 0, true, particles[i].color);
	}
}

void ParticleSystem::Update(float time, float dt, int window_width, int window_height) {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particles[i].inactive == false) { //If the particle is active, update its position, velocity and ttl
			particles[i].position = particles[i].position + (Vector2(250.0 * dt, 250.0 * dt) * particles[i].velocity);
			particles[i].velocity.x = 0.5 * sin(2 * time + i);
			particles[i].ttl -= dt;
			//If the particle is out of bounds or its ttl is over, the particle becomes inactive
			if (particles[i].position.y < 0 || particles[i].ttl < 0) {
				particles[i].inactive = true;
			}
		}

		if (particles[i].inactive == true) { //If the particle is inactive, reinitialize it
			particles[i].inactive = false;
			particles[i].ttl = 5 + rand() % 20;
			int x = (rand() % window_width); //Values out of the screen to make them appear from outside (above)
			int y = window_height + (rand() % MAX_PARTICLES);
			particles[i].position = Vector2(x, y);
		}
	}
}