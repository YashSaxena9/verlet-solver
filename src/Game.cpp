#include <assert.h>
#include <random>
#include "Game.hpp"

Game::Game(uint32_t screenWidth, uint32_t screenHeight, uint32_t frameRate)
    : m_screenWidth(screenWidth)
    , m_screenHeight(screenHeight)
    , m_running(true)
    , m_processInput(true)
    , m_engine() {
    InitWindow(m_screenWidth, m_screenHeight, "Verlet Game");
    SetTargetFPS(frameRate);
    LoadResources();
    // seed random number generator
    srand(time(0));
}

Game::~Game() {
    UnloadResources();
    CloseWindow();
}

void Game::LoadResources() {
    m_particleTexture = LoadTexture("res/circle.png");
}

void Game::UnloadResources() {
    UnloadTexture(m_particleTexture);
}

void Game::SpawnFixedParticles(
    const std::initializer_list<Vector2>& positions,
    float particleRadius
) {
    // optional: it prevents frequent resize
    m_engine.EnsureCapacity(positions.size());
    for (const Vector2& position : positions) {
        m_engine.AddFixedParticle(position, particleRadius, GRAY);
    }
}

void Game::SpawnParticles(const float probability, uint32_t limit, float particleRadius) {
    assert(probability <= 1.0f);
    m_engine.EnsureCapacity(limit); // optional: it prevents frequent resize
    const float particleDiameter = particleRadius * 2;
    const uint32_t rows = (uint32_t)round(m_screenHeight / particleDiameter);
    const uint32_t columns = (uint32_t)round(m_screenWidth / particleDiameter);
    const bool shouldCalculateChances = probability < 1.0f;
    uint32_t actualLimit = std::min(limit, rows * columns);
    /// ideally the column loop should be above and rows should be nested
    /// BUT, I need to stop(break) particle creation if limit is reached and I want it to fill
    /// in the order of top to bottom rather than left to right
    for (uint32_t row = 0; row < rows && actualLimit > 0; row += 1) {
        for (uint32_t col = 0; col < columns && actualLimit > 0; col += 1) {
            if (shouldCalculateChances) {
                float chance = (double)rand() / RAND_MAX;
                // continue if chance of spawning lies outside permisible probability
                if (chance > probability) {
                    continue;
                }
            }
            actualLimit -= 1;
            Vector2 generatedPosition = Vector2 {
                (float)(col * particleDiameter) + particleRadius,
                (float)(row * particleDiameter) + particleRadius
            };
            m_engine.AddParticle(
                generatedPosition,
                particleRadius,
                RED
            );
        }
    }
}

void Game::DebugPrint(const char* format, ...) {
    printf("[LOG] >> ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

void Game::DrawGameInfo() {
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Particles: %d", m_engine.ParticlesCount()), 10, 35, 15, GRAY);
}

void Game::Run() {
    while (!WindowShouldClose()) {
        if (m_processInput) {
            ProcessInput();
        }
        Update();
        Render();
    }
}

void Game::ShowFPS(bool shouldShow) {
    m_showFPS = shouldShow;
}

void Game::ShouldProcessInput(bool shouldProcess) {
    m_processInput = shouldProcess;
}

void Game::ProcessInput() {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        const Vector2& mousePos = GetMousePosition();
        m_engine.AddParticle(mousePos, Constants::PARTICLE_RADIUS, RED);
    }
}

void Game::Update() {
    float dt = GetFrameTime();
    m_engine.ApplyGravity(Constants::GRAVITY);
    m_engine.Update(dt);
    m_engine.ApplyConstraints(m_screenWidth, m_screenHeight);
    m_engine.ResolveCollisions();
}

void Game::Render() {
    BeginDrawing();

    ClearBackground(BLACK);
    m_engine.Draw(&m_particleTexture);

    // render fps if required
    if (m_showFPS) {
        DrawGameInfo();
    }

    EndDrawing();
}