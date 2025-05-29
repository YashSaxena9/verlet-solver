#include <iostream>
#include "Particle.hpp"
#include <raymath.h>

Particle::Particle(const Vector2& pos, float radius, Color color, bool isFixed)
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
    Vector2 velocity = Vector2 {
        m_position.x - m_oldPosition.x,
        m_position.y - m_oldPosition.y
    };
    m_oldPosition = m_position;
    m_position.x += velocity.x + m_acceleration.x * dt * dt;
    m_position.y += velocity.y + m_acceleration.y * dt * dt;
    m_acceleration = Vector2 { 0, 0 }; // reset acceleration
}

void Particle::ApplyForce(const Vector2& force) {
    if (m_isFixed) {
        return;
    }
    m_acceleration.x += force.x;
    m_acceleration.y += force.y;
}

Vector2 Particle::GetPosition() const {
    return m_position;
}

void Particle::SetPosition(Vector2 pos) {
    m_position = pos;
    m_oldPosition = pos;
}

void Particle::SetPositionWithSameVelocity(Vector2 pos) {
    const Vector2 velocity = GetVelocity();
    SetPosition(pos);
    SetVelocity(velocity);
}

Vector2 Particle::GetVelocity() const {
    return Vector2 {
        m_position.x - m_oldPosition.x,
        m_position.y - m_oldPosition.y,
    };
}

void Particle::SetVelocity(const Vector2& velocity) {
    m_oldPosition.x = m_position.x - velocity.x;
    m_oldPosition.y = m_position.y - velocity.y;
}

float Particle::GetRadius() const {
    return m_radius;
}

Color Particle::GetColor() const {
    return m_color;
}

bool Particle::IsFixed() const {
    return m_isFixed;
}

void Particle::MakeFixed(bool fixed) {
    m_isFixed = fixed;
}

void Particle::Draw(const Texture2D* particleTexture) const {
    if (particleTexture && particleTexture->id > 0) {
        const float diameter = m_radius + m_radius;
        Rectangle source = Rectangle {
            0, 0,
            (float)particleTexture->width, (float)particleTexture->height
        };
        Rectangle dest = Rectangle {
            m_position.x, m_position.y,
            diameter, diameter
        };
        Vector2 origin = Vector2 { diameter / 2, diameter / 2 };
        DrawTexturePro(*particleTexture, source, dest, origin, 0.0f, m_color);
    } else {
        DrawCircle(m_position.x, m_position.y, m_radius, m_color);
    }
}

bool Particle::CheckCollision(const Particle& first, const Particle& second) {
    static float eps = 0.001;
    Vector2 delta = Vector2Subtract(first.m_position, second.m_position);
    float minPermissibleDistance = first.m_radius + second.m_radius;
    float actualDistance = Vector2Length(delta);
    // check if distance is less than sum of radius.
    bool areColliding = minPermissibleDistance - actualDistance >= eps;
    return areColliding;
}

void Particle::ResolveCollision(Particle& first, Particle& second) {
    static float damping = 0.98f;
    Vector2 delta = Vector2Subtract(second.m_position, first.m_position);
    float distance = Vector2Length(delta);
    if (distance == 0.0f) {
        // avoid divide by zero
        return;
    }
    float minDistance = first.m_radius + second.m_radius;

    float overlap = minDistance - distance;

    if (overlap > 0.0f) {
        // normal direction per unit distance
        Vector2 collisionNormal = Vector2Scale(delta, 1.0f / distance);
        // position change will be half of the overlap
        Vector2 positionChange = Vector2Scale(collisionNormal, overlap * 0.5f);

        // push particles apart
        // Apply a basic velocity dampening to avoid energy gain
        if (!first.m_isFixed) {
            first.m_position = Vector2Subtract(first.m_position, positionChange);
            Vector2 velocity1 = first.GetVelocity();
            first.SetVelocity(Vector2Scale(velocity1, damping));
        }
        if (!second.m_isFixed) {
            second.m_position = Vector2Add(second.m_position, positionChange);
            Vector2 velocity2 = second.GetVelocity();
            second.SetVelocity(Vector2Scale(velocity2, damping));
        }
    }
}
