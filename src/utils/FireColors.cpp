#include "FireColors.hpp"
#include <raymath.h>

namespace sim {

Color ColorFromFireTemperature(float normalizedTemp) {
    float tempC = Clamp(normalizedTemp, 0.0f, 100.0f) * 20.0f; // 0–2000 °C

    if (tempC < 300) return { 30, 0, 0, 255 }; // dark red
    else if (tempC < 800) {
        float t = (tempC - 300) / 500.0f;
        return Color {
            (unsigned char)(30 + t * (255 - 30)),
            (unsigned char)(t * 100),
            0,
            255
        }; // red → orange
    } else if (tempC < 1200) {
        float t = (tempC - 800) / 400.0f;
        return Color {
            255,
            (unsigned char)(100 + t * (155)),
            (unsigned char)(t * 100),
            255
        }; // orange → yellow
    } else if (tempC < 1600) {
        float t = (tempC - 1200) / 400.0f;
        return Color {
            (unsigned char)(255 - t * 55),
            255,
            (unsigned char)(100 + t * 155),
            255
        }; // yellow → white
    } else {
        float t = (tempC - 1600) / 400.0f;
        return Color {
            (unsigned char)(200 - t * 200),
            (unsigned char)(255 - t * 255),
            255,
            255
        }; // white → blue
    }
}

}