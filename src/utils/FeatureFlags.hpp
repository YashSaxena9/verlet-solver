#pragma once
#include <cstdint>

enum class Feature : uint32_t {
    None         = 0,
    Logging      = 1 << 0,
    Motion       = 1 << 1,
    Gravity      = 1 << 2,
    SpatialHash  = 1 << 3,
    SimulateFire = 1 << 4
};

class FeatureFlags {
public:
    // Singleton accessor
    static FeatureFlags& Instance() {
        static FeatureFlags instance;
        return instance;
    }
    inline void Enable(Feature f) {
        m_flags |= static_cast<uint32_t>(f);
    }
    inline void Disable(Feature f) {
        m_flags &= ~static_cast<uint32_t>(f);
    }
    inline void Toggle(Feature f) {
        m_flags ^= static_cast<uint32_t>(f);
    }
    inline bool IsEnabled(Feature f) const {
        return (m_flags & static_cast<uint32_t>(f)) != 0;
    }

    inline void SetAll(uint32_t flags) {
        m_flags = flags;
    }
    inline uint32_t GetAll() const {
        return m_flags;
    }

private:
    // Hidden constructor/destructor to enforce singleton
    FeatureFlags() = default;
    ~FeatureFlags() = default;

    // Prevent copying/moving
    FeatureFlags(const FeatureFlags&) = delete;
    FeatureFlags& operator=(const FeatureFlags&) = delete;

    uint32_t m_flags;
};
