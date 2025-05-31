#include "VerletEngine.hpp"
#include <raymath.h>

void VerletEngine::EnsureCapacity(size_t additionalCount) {
    size_t requiredSize = m_particles.size() + additionalCount;
    if (requiredSize > m_particles.capacity()) {
        size_t newCapacity = std::max(
            requiredSize + 1,
            m_particles.capacity() * 3 / 2
        );
        m_particles.reserve(newCapacity);
    }
}

void VerletEngine::AddParticle(const Vector2& position, float radius, Color color) {
    addParticle(position, radius, color, false);
}

void VerletEngine::AddFixedParticle(const Vector2& position, float radius, Color color) {
    addParticle(position, radius, color, true);
}

void VerletEngine::addParticle(const Vector2& position, float radius, Color color, bool isFixed) {
    m_particles.emplace_back(position, radius, color, isFixed);
    maxParticleRadius = std::max(maxParticleRadius, radius);
}

size_t VerletEngine::ParticlesCount() const {
    return m_particles.size();
}

void VerletEngine::Update(float dt) {
    for (auto& particle : m_particles) {
        particle.Update(dt);
    }
}

void VerletEngine::ApplyGravity(const Vector2& gravity) {
    for (auto& particle : m_particles) {
        particle.ApplyForce(gravity);
    }
}

void VerletEngine::ApplyConstraints(uint32_t screenWidth, uint32_t screenHeight) {
    for (auto& particle : m_particles) {
        Vector2 position = particle.GetPosition();
        float radius = particle.GetRadius();
        /// as an optimisation we can use bitwise operators
        /// but I am lazy, and its will be less readable
        bool changedX = false, changedY = false;
        if (position.x - radius < 0) {
            // Left
            position.x = radius;
            changedX = true;
        }
        if (position.x + radius > screenWidth) {
            // Right
            position.x = screenWidth - radius;
            changedX = true;
        }
        if (position.y - radius < 0) {
            // Top
            position.y = radius;
            changedY = true;
        }
        if (position.y + radius > screenHeight) {
            // Bottom
            position.y = screenHeight - radius;
            changedY = true;
        }
        if (!changedX && !changedY) {
            continue;
        }
        Vector2 velocity = particle.GetVelocity();
        if (changedX) {
            velocity.x *= -1;
        }
        if (changedY) {
            velocity.y *= -1;
        }
        particle.SetPosition(position);
        particle.SetVelocity(velocity);
    }
}

void VerletEngine::ResolveCollisions() {
    for (size_t i = 0, end = m_particles.size() - 1; i < end; i += 1) {
        Particle& first = m_particles[i];
        for (size_t j = i + 1; j <= end; j += 1) {
            Particle& second = m_particles[j];
            if (Particle::CheckCollision(first, second)) {
                Particle::ResolveCollision(first, second);
            }
        }
    }
}

void VerletEngine::Draw(const Texture2D* particleTexture) const {
    for (const auto& particle : m_particles) {
        particle.Draw();
    }
}