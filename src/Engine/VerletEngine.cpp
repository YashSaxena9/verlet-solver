#include "VerletEngine.hpp"
#include <raymath.h>
#include "utils/FeatureFlags.hpp"
#include "GridHasher.hpp"

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
            velocity.x *= -1 * Particle::dampening;
        }
        if (changedY) {
            velocity.y *= -1 * Particle::dampening;
        }
        particle.SetPosition(position);
        particle.SetVelocity(velocity);
    }
}

void VerletEngine::ResolveCollisions() {
    if (FeatureFlags::Instance().IsEnabled(Feature::SpatialHash)) {
        resolveCollisionsWithSpatialHashing();
    } else {
        resolveCollisionsWithNxNComparisons();
    }
}

void VerletEngine::resolveCollisionsWithSpatialHashing() {
    // largest radius particle's diameter is cell size for spatial hash
    const float cellSize = GetMaxParticleRadiusInSystem() * 2;
    GridHasher grid(cellSize);
    std::unordered_map<int64_t, std::vector<size_t>> spatialGrid;

    // Fill grid
    for (size_t i = 0; i < m_particles.size(); i++) {
        const Vector2& pos = m_particles[i].GetPosition();
        int32_t gx = grid.GridCoord(pos.x);
        int32_t gy = grid.GridCoord(pos.y);
        int64_t hashValue = grid.Hash(gx, gy);
        spatialGrid[hashValue].push_back(i);
    }

    // Neighboring offsets
    const int dx[] = { -1, 0, 1 };
    const int dy[] = { -1, 0, 1 };

    // Check collisions
    for (const auto& cell : spatialGrid) {
        int64_t hash = cell.first;
        const auto& indicesA = cell.second;
        int32_t gx = (int32_t)(hash >> 32);
        int32_t gy = (int32_t)(hash & 0xFFFFFFFF);

        for (int ox : dx) {
            for (int oy : dy) {
                int64_t neighborHash = grid.Hash(gx + ox, gy + oy);
                if (spatialGrid.find(neighborHash) == spatialGrid.end()) {
                    continue;
                }

                const auto& indicesB = spatialGrid[neighborHash];

                for (size_t i : indicesA) {
                    for (size_t j : indicesB) {
                        if (i >= j) {
                            // Avoid double or self check
                            continue;
                        }
                        Particle& a = m_particles[i];
                        Particle& b = m_particles[j];
                        if (Particle::CheckCollision(a, b)) {
                            Particle::ResolveCollision(a, b);
                        }
                    }
                }
            }
        }
    }
}

void VerletEngine::resolveCollisionsWithNxNComparisons() {
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