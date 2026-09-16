#include "Input/Input.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iomanip>

static void pressKey(Input::System& input, sf::Keyboard::Scan scan) {
    sf::Event::KeyPressed kp{};
    kp.scancode = scan;
    input.handleEvent(kp);
}

static void releaseKey(Input::System& input, sf::Keyboard::Scan scan) {
    sf::Event::KeyReleased kr{};
    kr.scancode = scan;
    input.handleEvent(kr);
}

#define TEST_ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            std::cerr << "FAIL: " << msg << " (" #expr ") at line " << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while (0)

static int simulateTapSteps(int phaseFrame, int tapMs) {
    Input::System input;
    const double frameMs = 20.0; // 50 FPS
    const double pressMs = phaseFrame * frameMs;
    const double releaseMs = pressMs + tapMs;
    bool isDown = false;
    int stepsStarted = 0;

    for (int f = 0; f < 32; ++f) {
        double curMs = f * frameMs;

        if (!isDown && curMs >= pressMs && curMs < releaseMs) {
            isDown = true;
            pressKey(input, sf::Keyboard::Scan::Right);
        }

        if (isDown && curMs >= releaseMs) {
            isDown = false;
            releaseKey(input, sf::Keyboard::Scan::Right);
        }

        if (f % 8 == 0) {
            auto dir = input.getMovementDirection();
            if (dir.has_value()) {
                stepsStarted++;
            }
        }

        input.update();
    }

    return stepsStarted;
}

int main() {
    std::cout << "==================================================================" << std::endl;
    std::cout << "  RUNNING COMPREHENSIVE ONE-SHOT MOVEMENT BUFFER TESTS (50 FPS)  " << std::endl;
    std::cout << "==================================================================" << std::endl;

    // -------------------------------------------------------------------------
    // Priority and Ordering Tests
    // -------------------------------------------------------------------------
    // Test 1: Down -> Right -> release Right => Down resumes
    {
        Input::System input;
        pressKey(input, sf::Keyboard::Scan::Down);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveDown, "Press Down -> MoveDown");

        pressKey(input, sf::Keyboard::Scan::Right);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveRight, "Press Right while Down held -> MoveRight");

        releaseKey(input, sf::Keyboard::Scan::Right);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveDown, "Release Right while Down held -> resume MoveDown");

        releaseKey(input, sf::Keyboard::Scan::Down);
        TEST_ASSERT(!input.getMovementDirection().has_value(), "Release Down -> idle");
        std::cout << "[PASS] Test 1: Down -> Right -> release Right => Down resumes" << std::endl;
    }

    // Test 2: Up -> Left -> release Left => Up resumes
    {
        Input::System input;
        pressKey(input, sf::Keyboard::Scan::Up);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveUp, "Press Up -> MoveUp");

        pressKey(input, sf::Keyboard::Scan::Left);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveLeft, "Press Left while Up held -> MoveLeft");

        releaseKey(input, sf::Keyboard::Scan::Left);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveUp, "Release Left while Up held -> resume MoveUp");

        releaseKey(input, sf::Keyboard::Scan::Up);
        TEST_ASSERT(!input.getMovementDirection().has_value(), "Release Up -> idle");
        std::cout << "[PASS] Test 2: Up -> Left -> release Left => Up resumes" << std::endl;
    }

    // Test 3: Right -> Down -> release Down => Right resumes
    {
        Input::System input;
        pressKey(input, sf::Keyboard::Scan::Right);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveRight, "Press Right -> MoveRight");

        pressKey(input, sf::Keyboard::Scan::Down);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveDown, "Press Down while Right held -> MoveDown");

        releaseKey(input, sf::Keyboard::Scan::Down);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveRight, "Release Down while Right held -> resume MoveRight");

        releaseKey(input, sf::Keyboard::Scan::Right);
        TEST_ASSERT(!input.getMovementDirection().has_value(), "Release Right -> idle");
        std::cout << "[PASS] Test 3: Right -> Down -> release Down => Right resumes" << std::endl;
    }

    // Test 4: WASD keys (S -> D -> release D => S resumes)
    {
        Input::System input;
        pressKey(input, sf::Keyboard::Scan::S);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveDown, "Press S -> MoveDown");

        pressKey(input, sf::Keyboard::Scan::D);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveRight, "Press D while S held -> MoveRight");

        releaseKey(input, sf::Keyboard::Scan::D);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveDown, "Release D while S held -> resume MoveDown");

        releaseKey(input, sf::Keyboard::Scan::S);
        TEST_ASSERT(!input.getMovementDirection().has_value(), "Release S -> idle");
        std::cout << "[PASS] Test 4: WASD S -> D -> release D => S resumes" << std::endl;
    }

    // Test 5: Three keys in sequence (Left -> Up -> Right -> release Right -> release Up)
    {
        Input::System input;
        pressKey(input, sf::Keyboard::Scan::Left);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveLeft, "Press Left -> MoveLeft");

        pressKey(input, sf::Keyboard::Scan::Up);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveUp, "Press Up -> MoveUp");

        pressKey(input, sf::Keyboard::Scan::Right);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveRight, "Press Right -> MoveRight");

        releaseKey(input, sf::Keyboard::Scan::Right);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveUp, "Release Right -> resume MoveUp");

        releaseKey(input, sf::Keyboard::Scan::Up);
        TEST_ASSERT(input.getMovementDirection() == Input::Action::MoveLeft, "Release Up -> resume MoveLeft");

        releaseKey(input, sf::Keyboard::Scan::Left);
        TEST_ASSERT(!input.getMovementDirection().has_value(), "Release Left -> idle");
        std::cout << "[PASS] Test 5: Three keys sequence (Left -> Up -> Right)" << std::endl;
    }

    // -------------------------------------------------------------------------
    // Cardinal Single Tap Tests (Up, Down, Left, Right)
    // -------------------------------------------------------------------------
    {
        struct TapTestEntry {
            sf::Keyboard::Scan scan;
            Input::Action expected;
            const char* name;
        };

        TapTestEntry entries[] = {
            { sf::Keyboard::Scan::Up,    Input::Action::MoveUp,    "Up" },
            { sf::Keyboard::Scan::Down,  Input::Action::MoveDown,  "Down" },
            { sf::Keyboard::Scan::Left,  Input::Action::MoveLeft,  "Left" },
            { sf::Keyboard::Scan::Right, Input::Action::MoveRight, "Right" },
        };

        for (const auto& entry : entries) {
            Input::System input;
            // Key pressed at frame 2, released at frame 6 (short tap between ticks)
            pressKey(input, entry.scan);
            releaseKey(input, entry.scan);

            // Decision tick at frame 8: consumes pending direction
            auto dir1 = input.getMovementDirection();
            TEST_ASSERT(dir1 == entry.expected, std::string("Tap ") + entry.name + " yields exactly 1 buffered step");

            // Decision tick at frame 16: buffer was consumed, key is not held -> stops
            auto dir2 = input.getMovementDirection();
            TEST_ASSERT(!dir2.has_value(), std::string("Tap ") + entry.name + " does not take a second step");

            std::cout << "[PASS] Single tap " << entry.name << " -> exactly 1 buffered step" << std::endl;
        }
    }

    // -------------------------------------------------------------------------
    // Continuous Hold Test (500 ms at 50 FPS)
    // -------------------------------------------------------------------------
    {
        Input::System input;
        int stepsTaken = 0;
        // Hold Right from frame 0 to frame 25 (500 ms)
        pressKey(input, sf::Keyboard::Scan::Right);

        for (int f = 0; f < 32; ++f) {
            if (f == 25) {
                releaseKey(input, sf::Keyboard::Scan::Right);
            }
            if (f % 8 == 0) {
                auto dir = input.getMovementDirection();
                if (dir.has_value()) {
                    TEST_ASSERT(dir == Input::Action::MoveRight, "Continuous hold moves Right");
                    stepsTaken++;
                }
            }
            input.update();
        }

        TEST_ASSERT(stepsTaken == 4, "Hold Right produced 4 continuous steps (Frames 0, 8, 16, 24)");
        std::cout << "[PASS] Hold Right -> 4 continuous steps over 500ms with no pause" << std::endl;
    }

    // -------------------------------------------------------------------------
    // Multiple Taps Before Next Tick (Last-Key-Wins)
    // -------------------------------------------------------------------------
    {
        Input::System input;
        // Frame 2: tap Up
        pressKey(input, sf::Keyboard::Scan::Up);
        releaseKey(input, sf::Keyboard::Scan::Up);

        // Frame 5: tap Left (replaces Up in buffer)
        pressKey(input, sf::Keyboard::Scan::Left);
        releaseKey(input, sf::Keyboard::Scan::Left);

        // Frame 8 (decision tick): Left must win
        auto dir = input.getMovementDirection();
        TEST_ASSERT(dir == Input::Action::MoveLeft, "Last-key-wins: Left replaces Up in one-shot buffer");

        // Frame 16: consumed, idle
        TEST_ASSERT(!input.getMovementDirection().has_value(), "Idle after last-key-wins single step");
        std::cout << "[PASS] Multiple taps before next tick -> most recent direction wins" << std::endl;
    }

    // -------------------------------------------------------------------------
    // Blocked Wall Safety Test
    // -------------------------------------------------------------------------
    {
        Input::System input;
        // Tap Right
        pressKey(input, sf::Keyboard::Scan::Right);
        releaseKey(input, sf::Keyboard::Scan::Right);

        // Tick 0: Jim queries direction (faces wall, cannot move)
        auto dir0 = input.getMovementDirection();
        TEST_ASSERT(dir0 == Input::Action::MoveRight, "Direction consumed on blocked tick");

        // Tick 8: Buffer was consumed on tick 0, key is not held -> must NOT repeat!
        auto dir8 = input.getMovementDirection();
        TEST_ASSERT(!dir8.has_value(), "Blocked tap is safely consumed and does not persist forever");
        std::cout << "[PASS] Pending direction against blocked wall is consumed safely" << std::endl;
    }

    // -------------------------------------------------------------------------
    // State Transition / Cave Reset Clear Test
    // -------------------------------------------------------------------------
    {
        Input::System input;
        pressKey(input, sf::Keyboard::Scan::Right);
        releaseKey(input, sf::Keyboard::Scan::Right);

        // Cave reset / player death occurs before next tick
        input.clearPendingMovement();

        // Next tick: buffer was cleared
        auto dir = input.getMovementDirection();
        TEST_ASSERT(!dir.has_value(), "clearPendingMovement() clears pending direction");
        std::cout << "[PASS] Pending direction is cleared on cave reset / death / restart" << std::endl;
    }

    // -------------------------------------------------------------------------
    // No Diagonal Movement Test
    // -------------------------------------------------------------------------
    {
        Input::System input;
        pressKey(input, sf::Keyboard::Scan::Up);
        pressKey(input, sf::Keyboard::Scan::Right);

        auto dir = input.getMovementDirection();
        TEST_ASSERT(dir == Input::Action::MoveRight, "Cardinal only: Right wins over Up");
        TEST_ASSERT(dir != Input::Action::MoveUp, "No diagonal: returns single cardinal direction");

        releaseKey(input, sf::Keyboard::Scan::Right);
        releaseKey(input, sf::Keyboard::Scan::Up);
        std::cout << "[PASS] No diagonal movement" << std::endl;
    }

    // -------------------------------------------------------------------------
    // Extended Timing Matrix Validation (Across All 8 Phases)
    // -------------------------------------------------------------------------
    {
        std::cout << "\n=== EXTENDED TIMING MATRIX: 50 FPS TAP DURATIONS ACROSS ALL 8 PHASES ===" << std::endl;
        std::cout << "Tap (ms)   | Missed Rate  | Single-Step Rate | Double-Step Rate" << std::endl;
        std::cout << "---------------------------------------------------------------" << std::endl;

        const int durations[] = { 50, 70, 100, 120, 140, 150, 160, 170, 180, 200 };

        for (int d : durations) {
            int missed = 0;
            int single = 0;
            int doubleStep = 0;

            for (int p = 0; p < 8; ++p) {
                int steps = simulateTapSteps(p, d);
                if (steps == 0) missed++;
                else if (steps == 1) single++;
                else if (steps == 2) doubleStep++;
            }

            double missedPct = missed * 100.0 / 8.0;
            double singlePct = single * 100.0 / 8.0;
            double doublePct = doubleStep * 100.0 / 8.0;

            std::cout << std::setw(6) << d << " ms   | "
                      << std::setw(9) << std::fixed << std::setprecision(1) << missedPct << "%  | "
                      << std::setw(13) << singlePct << "%  | "
                      << std::setw(13) << doublePct << "%" << std::endl;

            // Assertions for each duration
            TEST_ASSERT(missed == 0, "Missed rate must be 0% for all tap durations");

            if (d <= 160) {
                TEST_ASSERT(single == 8, "Single-step rate must be 100% for tap duration <= 160ms");
                TEST_ASSERT(doubleStep == 0, "Double-step rate must be 0% for tap duration <= 160ms");
            } else if (d <= 180) {
                TEST_ASSERT(single == 7, "Single-step rate is 87.5% for 170-180ms (Phase 0 double-steps)");
                TEST_ASSERT(doubleStep == 1, "Double-step rate is 12.5% for 170-180ms (Phase 0 only)");
            } else if (d == 200) {
                TEST_ASSERT(single == 6, "Single-step rate is 75% for 200ms");
                TEST_ASSERT(doubleStep == 2, "Double-step rate is 25% for 200ms");
            }
        }

        std::cout << "\nExact continuous-hold threshold: > 160.0 ms" << std::endl;
        std::cout << "All extended timing matrix tests passed successfully!" << std::endl;
    }

    std::cout << "\n==================================================================" << std::endl;
    std::cout << "  ALL ONE-SHOT MOVEMENT BUFFER TESTS PASSED SUCCESSFULLY!         " << std::endl;
    std::cout << "==================================================================" << std::endl;
    return 0;
}