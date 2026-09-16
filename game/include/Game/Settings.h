#pragma once

/// @brief Target framerate cap for the game.
/// NOTE: Currently set to 50 FPS as a test tuning value to evaluate whether slowing the
/// entire game uniformly resolves occasional double-step movement on short taps (giving
/// ~160 ms per 8-frame grid step instead of ~125 ms at the original 64 FPS cap).
constexpr unsigned int GAME_FRAMERATE_LIMIT = 50u;

/// @brief The game settings.
struct GameSettings {
    bool audio = true;
    float audioVolume = 100.f;
    bool joystickControl = false;
    bool fixedColours = false;
    bool setRefreshRateOnStart = false;
    unsigned int framerateLimit = GAME_FRAMERATE_LIMIT;
};
