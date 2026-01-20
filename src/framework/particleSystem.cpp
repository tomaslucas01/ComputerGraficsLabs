#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "framework/../extra/picopng.h"
#include "framework/image.h"
#include "framework/utils.h"
#include "framework/camera.h"
#include "framework/mesh.h"
#include "particleSystem.h"
#include "framework.h"

void ParticleSystem::Init() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		particles[i].position.Random(999); //Sets the particle in a random position
	}
}

void ParticleSystem::Render(Image* framebuffer) {

}