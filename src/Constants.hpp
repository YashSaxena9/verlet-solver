#pragma once
#include <raylib.h>
#include <cstdint>

namespace Constants {
    constexpr int32_t PREFERRED_FPS = 120;

    constexpr int32_t SCREEN_WIDTH = 800;
    constexpr int32_t SCREEN_HEIGHT = 600;

    constexpr bool SPAWN_ON_CLICK = true;
    constexpr bool SHOW_FPS = true;
    
    constexpr float SPAWN_PROBABLITY = 0.6f;
    constexpr int32_t SPAWN_LIMIT = 20000;

    constexpr Vector2 GRAVITY = Vector2 { 0.0f, 9.81f };
    constexpr Vector2 WIND_FORCE = Vector2 { 0.0f, 0.0f };

    constexpr int32_t PARTICLE_RADIUS = 2;

    constexpr float FIRE_UPWARD_DRAFT_FORCE = 3.0f;
    constexpr float PARTICLE_HEAT_LOSS_RATE = 0.005f;
    constexpr float PARTICLE_HEAT_DIFFUSION_RATE = 1.0f;
    constexpr float MIN_FLOOR_HEAT_DIFFUSION_RATE = 0.2f;
    constexpr float MAX_FLOOR_HEAT_DIFFUSION_RATE = 0.5f;
    constexpr float FIRE_THRESHOLD_TEMP = 250;
    constexpr float MAX_TEMPERATURE = 300;
}