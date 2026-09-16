#include "Input/Input.h"
#include <iostream>
#include <cstdlib>

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

int main() {
    std::cout << "Running Movement Input Ordering Tests..." << std::endl;

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

    std::cout << "All movement ordering tests passed successfully!" << std::endl;
    return 0;
}
