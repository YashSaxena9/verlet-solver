#pragma once

#include <vector>
#include "Particle.hpp"

class VerletEngine {
public:
    void EnsureCapacity(size_t additionalCount);
    void AddParticle(const Vector2& position, float radius, Color color);
    void AddFixedParticle(const Vector2& position, float radius, Color color);
    size_t ParticlesCount() const;
    void Update(float dt);
    void ApplyConstraints(int screenWidth, int screenHeight);
    void ApplyGravity(const Vector2& gravity);
    void Draw(const Texture2D* particleTexture) const;
    void ResolveCollisions();
private:
    std::vector<Particle> m_particles;
};