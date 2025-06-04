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
    constexpr int32_t SPAWN_LIMIT = 3000;

    constexpr Vector2 GRAVITY = Vector2 { 0.0f, 9.81f };
    constexpr Vector2 WIND_FORCE = Vector2 { 0.0f, 0.0f };

    constexpr int32_t PARTICLE_RADIUS = 2.0;

    constexpr float PARTICLE_HEAT_DIFFUSION_RATE = 0.04;
    constexpr float FLOOR_HEAT_DIFFUSION_RATE = 0.8;
    constexpr int32_t FIRE_THRESHOLD_TEMP = 90;
    constexpr int32_t FLOOR_TEMPERATURE = 100;
}