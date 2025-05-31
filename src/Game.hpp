#pragma once

#include <raylib.h>
#include "Constants.hpp"
#include "Engine/VerletEngine.hpp"

class Game {
public:
    Game(uint32_t screenWidth, uint32_t screenHeight, uint32_t frameRate = Constants::PREFERRED_FPS);
    ~Game();
    void SpawnFixedParticles(const std::initializer_list<Vector2>& positions, float particleRadius = Constants::PARTICLE_RADIUS);
    void SpawnParticles(float probability = 1.0, uint32_t limit = UINT_MAX, float partcleRadius = Constants::PARTICLE_RADIUS);
    void Run();
    void ShowFPS(bool shouldShow);
    void ShouldProcessInput(bool shouldProcess);

private:
    static constexpr uint32_t updateSubsteps = 4u;
    const uint32_t m_screenWidth, m_screenHeight;
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