#pragma once

#include <raylib.h>
#include "Constants.hpp"
#include "Engine/VerletEngine.hpp"

class Game {
public:
    Game(int screenWidth, int screenHeight, int frameRate = Constants::PREFERRED_FPS);
    ~Game();
    void SpawnFixedParticles(const std::initializer_list<Vector2>& positions, int particleRadius = Constants::PARTICLE_RADIUS);
    void SpawnParticles(float probability = 1.0, uint32_t limit = -1);
    void Run();
    void ShowFPS(bool shouldShow);
    void ShouldProcessInput(bool shouldProcess);

private:
    const int m_screenWidth, m_screenHeight;
    bool m_running, m_showFPS, m_processInput;
    VerletEngine m_engine;
    Texture2D m_particleTexture;
    
    void LoadResources();
    void UnloadResources();
    void DebugPrint(const char* str, ...);
    void DrawGameInfo();
    void ProcessInput();
    void Update();
    void Render();
};