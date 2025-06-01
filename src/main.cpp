#include <iostream>
#include "Game.hpp"
#include "utils/FeatureFlags.hpp"
#include "utils/ThreadPool.hpp"

using namespace std;

int main(int args, char** argv) {
    FeatureFlags& flags = FeatureFlags::Instance();
    flags.Enable(Feature::Motion);
    flags.Enable(Feature::Gravity);
    flags.Enable(Feature::Logging);
    flags.Enable(Feature::SpatialHash);

    int32_t width = Constants::SCREEN_WIDTH;
    int32_t height = Constants::SCREEN_HEIGHT;
    mt::ThreadPool threadPool(20);

    Game game(
        threadPool,
        width,
        height,
        Constants::PREFERRED_FPS
    );
    game.ShouldProcessInput(Constants::SPAWN_ON_CLICK);
    game.ShowFPS(Constants::SHOW_FPS);
    game.SpawnFixedParticles({
        Vector2 { (float)width / 4.0f, (float)height / 2.0f },
        Vector2 { (float)width * 3.0f / 4.0f, (float)height / 2.0f }
    });
    game.SpawnParticles(
        Constants::SPAWN_PROBABLITY,
        Constants::SPAWN_LIMIT
    );
    game.Run();

    return EXIT_SUCCESS;
}
