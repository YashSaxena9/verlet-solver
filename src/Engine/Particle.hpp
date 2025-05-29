#pragma once

#include <raylib.h>

class Particle {
public:
    Particle(const Vector2& pos, float radius, Color color, bool isFixed = false);
    Particle(Particle&& particle) noexcept;

    Particle& operator=(Particle&& particle) noexcept;

    void Update(float dt);
    void ApplyForce(const Vector2& force);

    Vector2 GetPosition() const;
    void SetPosition(Vector2 pos);
    void SetPositionWithSameVelocity(Vector2 pos);

    Vector2 GetVelocity() const;
    void SetVelocity(const Vector2& velocity);

    float GetRadius() const;

    Color GetColor() const;

    bool IsFixed() const;

    void MakeFixed(bool fixed = true);

    void Draw(const Texture2D* particleTexture) const;

    static bool CheckCollision(const Particle& first, const Particle& second);

    static void ResolveCollision(Particle& first, Particle& second);

private:
    Vector2 m_position;
    Vector2 m_oldPosition;
    Vector2 m_acceleration;
    float m_radius;
    Color m_color;
    bool m_isFixed;

    // friend class VerletEngine; // Engine can access private members
};