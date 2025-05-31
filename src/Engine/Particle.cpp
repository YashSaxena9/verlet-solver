#include <iostream>
#include "Particle.hpp"
#include <raymath.h>

Particle::Particle(const Vector2& pos, float radius, const Color color, bool isFixed)
    : m_position(pos)
    , m_oldPosition(pos)
    , m_acceleration(Vector2 { 0, 0 })
    , m_radius(radius)
    , m_color(color)
    , m_isFixed(isFixed)
    {}

// move constructor for more performance when vector grows
Particle::Particle(Particle&& particle) noexcept
    : m_position(particle.m_position)
    , m_oldPosition(particle.m_oldPosition)
    , m_acceleration(particle.m_acceleration)
    , m_radius(particle.m_radius)
    , m_color(particle.m_color)
    , m_isFixed(particle.m_isFixed)
    {}

Particle& Particle::operator=(Particle&& particle) noexcept {
    if (this == &particle) {
        // Prevent self-assignment
        return *this;
    }
    m_position = particle.m_position;
    m_oldPosition = particle.m_oldPosition;
    m_acceleration = particle.m_acceleration;
    m_radius = particle.m_radius;
    m_color = particle.m_color;
    m_isFixed = particle.m_isFixed;
    return *this;
}

void Particle::Update(float dt) {
    if (m_isFixed) {
        return;
    }
    Vector2 velocity = Vector2Subtract(m_position, m_oldPosition);
    m_oldPosition = m_position;
    m_position.x += velocity.x + m_acceleration.x * dt * dt;
    m_position.y += velocity.y + m_acceleration.y * dt * dt;
    m_acceleration = Vector2 { 0, 0 }; // reset acceleration
}

void Particle::ApplyForce(const Vector2& force) {
    if (!IsFixed()) {
        m_acceleration.x += force.x;
        m_acceleration.y += force.y;
    }
}

void Particle::Draw(const Texture2D* particleTexture) const {
    if (particleTexture != nullptr && particleTexture->id > 0) {
        float radius = GetRadius();
        float diameter = radius * 2;
        Rectangle source = Rectangle {
            0, 0,
            (float)particleTexture->width, (float)particleTexture->height
        };
        Rectangle dest = Rectangle {
            m_position.x, m_position.y,
            diameter, diameter
        };
        Vector2 origin = Vector2 { radius, radius };
        DrawTexturePro(*particleTexture, source, dest, origin, 0.0f, m_color);
    } else {
        DrawCircle(m_position.x, m_position.y, m_radius, m_color);
    }
}

bool Particle::CheckCollision(const Particle& first, const Particle& second) {
    if (first.IsFixed() && second.IsFixed()) {
        return false;
    }
    Vector2 delta = Vector2Subtract(first.GetPosition(), second.GetPosition());
    float minPermissibleDistance = first.GetRadius() + second.GetRadius();
    float actualDistance = Vector2Length(delta);
    // check if distance is less than sum of radius.
    bool areColliding = minPermissibleDistance - actualDistance >= Particle::eps;
    return areColliding;
}

void Particle::ResolveCollision(Particle& first, Particle& second) {
    Vector2 delta = Vector2Subtract(second.GetPosition(), first.GetPosition());
    float distance = Vector2Length(delta);
    if (distance == 0.0f) {
        // avoid divide by zero
        return;
    }
    float minPermissibleDistance = first.GetRadius() + second.GetRadius();

    float overlap = minPermissibleDistance - distance;

    if (overlap >= Particle::eps) {
        // normal direction per unit distance
        Vector2 collisionNormal = Vector2Scale(delta, 1.0f / distance);
        // position change will be half of the overlap if none is fixed
        Vector2 positionChange = Vector2Scale(
            collisionNormal,
            first.IsFixed() || second.IsFixed() ? overlap : overlap * 0.5f
        );

        // push particles apart
        // Apply a basic velocity dampening to avoid energy gain
        if (!first.IsFixed()) {
            first.m_position = Vector2Subtract(first.GetPosition(), positionChange);
            Vector2 velocity = first.GetVelocity();
            first.SetVelocity(Vector2Scale(velocity, Particle::dampening));
        }
        if (!second.IsFixed()) {
            second.m_position = Vector2Add(second.GetPosition(), positionChange);
            Vector2 velocity = second.GetVelocity();
            second.SetVelocity(Vector2Scale(velocity, Particle::dampening));
        }
    }
}
