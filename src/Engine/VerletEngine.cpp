#include "VerletEngine.hpp"
#include <raymath.h>
#include "VerletEngine.hpp"
#include "utils/FeatureFlags.hpp"
#include "utils/ThreadPool.hpp"
#include "GridHasher.hpp"

VerletEngine::VerletEngine(mt::ThreadPool& threadPool)
    : m_threadPool(threadPool)
    {}

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
    m_particleLocks.emplace_back(std::make_unique<std::mutex>());
    maxParticleRadius = std::max(maxParticleRadius, radius);
}

size_t VerletEngine::ParticlesCount() const {
    return m_particles.size();
}

void VerletEngine::Update(float dt) {
    m_threadPool.dispatch(m_particles.size(), [&](size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            Particle& particle = m_particles[i];
            particle.Update(dt);
        }
    });
}

void VerletEngine::ApplyGravity(const Vector2& gravity) {
    m_threadPool.dispatch(m_particles.size(), [&](size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            Particle& particle = m_particles[i];
            particle.ApplyForce(gravity);
        }
    });
}

void VerletEngine::ApplyConstraints(uint32_t screenWidth, uint32_t screenHeight) {
    m_threadPool.dispatch(m_particles.size(), [&](size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            Particle& particle = m_particles[i];
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
    });
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


    // Check collisions
    std::vector<std::pair<size_t, size_t>> possibleCollisionPairs;
    possibleCollisionPairs.reserve(spatialGrid.size() * spatialGrid.size());

    for (const auto& cell : spatialGrid) {
        int64_t hash = cell.first;
        const auto& indicesA = cell.second;
        int32_t gx = (int32_t)(hash >> 32);
        int32_t gy = (int32_t)(hash & 0xFFFFFFFF);

        for (const int ox : this->DIR_X) {
            for (const int oy : this->DIR_Y) {
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
                        possibleCollisionPairs.emplace_back(i, j);

                    }
                }
            }
        }
    }
    // resolve collision with multithreading
    m_threadPool.dispatch(possibleCollisionPairs.size(), [&](size_t start, size_t end) {
        // iterate either forward or backward in each frame,
        // iterating only one side piles the particles on that side only
        // this happens because of float precision
        float probablity = (double)rand() / RAND_MAX;
        bool shouldIterateForward = probablity < 0.5;
        if (shouldIterateForward) {
            for (size_t i = start; i < end; i++) {
                auto [aIndex, bIndex] = possibleCollisionPairs[i];
                resolveParticlePairCollision(aIndex, bIndex);
            }
        } else {
            for (size_t i = end; i > start; i--) {
                auto [aIndex, bIndex] = possibleCollisionPairs[i - 1];
                resolveParticlePairCollision(aIndex, bIndex);
            }
        }
    });
}

void VerletEngine::resolveCollisionsWithNxNComparisons() {
    for (size_t i = 0, end = m_particles.size() - 1; i < end; i += 1) {
        for (size_t j = i + 1; j <= end; j += 1) {
            Particle& a = m_particles[i];
            Particle& b = m_particles[j];
            if (Particle::CheckCollision(a, b)) {
                Particle::ResolveCollision(a, b);
            }
        }
    }
}

void VerletEngine::resolveParticlePairCollision(size_t idx1, size_t idx2) {
    if (idx1 > idx2) {
        // swap to Lock in consistent order
        idx1 = idx1 ^ idx2;
        idx2 = idx1 ^ idx2;
        idx1 = idx1 ^ idx2;
    }

    std::lock(*m_particleLocks[idx1], *m_particleLocks[idx2]);
    std::lock_guard<std::mutex> lockA(*m_particleLocks[idx1], std::adopt_lock);
    std::lock_guard<std::mutex> lockB(*m_particleLocks[idx2], std::adopt_lock);
    Particle& a = m_particles[idx1];
    Particle& b = m_particles[idx2];
    if (Particle::CheckCollision(a, b)) {
        Particle::ResolveCollision(a, b);
    }
}

void VerletEngine::Draw(const Texture2D* particleTexture) const {
    m_threadPool.wait();
    for (const auto& particle : m_particles) {
        particle.Draw(particleTexture);
    }
}