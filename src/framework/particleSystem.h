#pragma once
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "framework/image.h"


class ParticleSystem {

    static const int MAX_PARTICLES = 1500; //A VALUE THAT WE CHOOSE

    struct Particle {
        Vector2 position;
        Vector2 velocity; // Normalized speed and direction of the particle
        Color color;
        float acceleration;
        float ttl; // Time left until the particle expires
        bool inactive; // Particle is not used/expired, so it can be recreated
    };

    Particle particles[MAX_PARTICLES];

public:
    void Init(int width, int height);
    void Render(Image* framebuffer);
    void Update(float time, float dt, int window_width, int window_height);
};