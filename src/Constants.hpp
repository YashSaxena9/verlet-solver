#pragma once
#include <raylib.h>

namespace Constants {
    constexpr int PREFERRED_FPS = 120;

    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;

    constexpr bool SPAWN_ON_CLICK = true;
    constexpr bool SHOW_FPS = true;
    
    constexpr float SPAWN_PROBABLITY = 0.6f;
    constexpr int SPAWN_LIMIT = 40000;

    constexpr Vector2 GRAVITY = Vector2 { 0.0f, 9.81f };
    constexpr Vector2 WIND_FORCE = Vector2 { 0.0f, 0.0f };

    constexpr int PARTICLE_RADIUS = 1.0;
}