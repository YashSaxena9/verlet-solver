#pragma once

#include <raylib.h>
#include "utils/FireColors.hpp"

class Particle {
public:
    // a small epsilon value to account for floating-point imprecision.
    static constexpr float eps = 0.0001f;
    static constexpr float dampening = 0.98f;

    Particle(const Vector2& pos, float radius, bool isFixed = false);
    Particle(Particle&& particle) noexcept;

    Particle& operator=(Particle&& particle) noexcept;

    void Update(float dt);
    void ApplyForce(const Vector2& force);

    inline Vector2 GetPosition() const {
        return m_position;
    }

    inline void SetPosition(const Vector2& pos) {
        m_position = Vector2 { pos.x, pos.y };
        m_oldPosition = Vector2 { pos.x, pos.y };
    }

    inline void SetPositionWithSameVelocity(const Vector2& pos) {
        Vector2 velocity = GetVelocity();
        SetPosition(pos);
        SetVelocity(velocity);
    }

    inline Vector2 GetVelocity() const {
        return Vector2 {
            m_position.x - m_oldPosition.x,
            m_position.y - m_oldPosition.y,
        };
    }

    inline void SetVelocity(const Vector2 &velocity) {
        m_oldPosition.x = m_position.x - velocity.x;
        m_oldPosition.y = m_position.y - velocity.y;
    }

    inline float GetRadius() const {
        return m_radius;
    }

    inline int32_t GetTemperature() const {
        return m_temperature;
    }

    inline Color GetColor() const {
        return sim::ColorFromFireTemperature(GetTemperature());
    }

    inline void IncrementTemperature(int32_t tempInc) {
        m_temperature += tempInc;
    }

    inline void DecrementTemperature(int32_t tempInc) {
        m_temperature = std::min(m_temperature - tempInc, 0);
    }

    inline bool IsFixed() const {
        return m_isFixed;
    }

    inline void MakeFixed(bool fixed = true) {
        m_isFixed = fixed;
    }

    void Draw(const Texture2D* particleTexture = nullptr) const;

    static bool CheckCollision(const Particle& first, const Particle& second);

    static void ResolveCollision(Particle& first, Particle& second);

private:
    Vector2 m_position;
    Vector2 m_oldPosition;
    Vector2 m_acceleration;
    float m_radius;
    bool m_isFixed;

    // for fire simulation
    int32_t m_temperature;
};