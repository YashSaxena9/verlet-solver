#pragma once

#include <vector>
#include "Particle.hpp"
#include "utils/ThreadPool.hpp"

class VerletEngine {
public:
    VerletEngine(mt::ThreadPool& threadPool);
    void EnsureCapacity(size_t additionalCount);
    void AddParticle(const Vector2& position, float radius);
    void AddFixedParticle(const Vector2& position, float radius);
    size_t ParticlesCount() const;
    void Update(float dt);
    void UpwardDraftOnHighTemperature(int32_t temp);
    void ApplyConstraints(uint32_t screenWidth, uint32_t screenHeight);
    void ApplyGravity(const Vector2& gravity);
    void Draw(const Texture2D* particleTexture) const;
    void ResolveCollisions();

    inline float GetMaxParticleRadiusInSystem() {
        return maxParticleRadius;
    }
private:
    mt::ThreadPool& m_threadPool;
    float maxParticleRadius = 0;
    std::vector<Particle> m_particles;
    std::vector<std::unique_ptr<std::mutex>> m_particleLocks;

    // Neighboring offsets for spatial hashing collision resolution
    static const int s_gridSide = 3;
    const int32_t DIR_X[3] = { -1, 0, 1 };
    const int32_t DIR_Y[3] = { -1, 0, 1 };

    void addParticle(const Vector2& position, float radius, bool isFixed);
    void resolveParticlePairCollision(size_t idx1, size_t idx2);
    void resolveCollisionsWithSpatialHashing();
    void resolveCollisionsWithNxNComparisons();
};