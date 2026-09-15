#include "Input/Input.h"
#include <algorithm>

bool Input::System::isMovementAction(Input::Action action) {
    return action == Input::Action::MoveUp ||
           action == Input::Action::MoveDown ||
           action == Input::Action::MoveLeft ||
           action == Input::Action::MoveRight;
}

void Input::System::pushMovementAction(Input::Action action) {
    if (!isMovementAction(action)) return;
    auto it = std::find(m_movementOrder.begin(), m_movementOrder.end(), action);
    if (it != m_movementOrder.end()) {
        m_movementOrder.erase(it);
    }
    m_movementOrder.push_back(action);
}

void Input::System::popMovementAction(Input::Action action) {
    m_movementOrder.erase(
        std::remove(m_movementOrder.begin(), m_movementOrder.end(), action),
        m_movementOrder.end()
    );
}

std::optional<Input::Action> Input::System::getMovementDirection() const {
    for (auto it = m_movementOrder.rbegin(); it != m_movementOrder.rend(); ++it) {
        if (isPressed(*it)) {
            return *it;
        }
    }
    return std::nullopt;
}

void Input::System::handleEvent(const sf::Event& event) {

    if (const auto keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        m_heldScancodes.insert(keyPressed->scancode);
        for (auto& [key, action] : m_keyMap) {
            if (keyPressed->scancode == key) {
                const bool wasHeld = m_heldActions.count(action) > 0;
                m_pressedActions.insert(action);
                m_heldActions.insert(action);
                if (isMovementAction(action) && !wasHeld) {
                    pushMovementAction(action);
                }
                m_keyboardEventOccurred = true;
            }
        }
    }
    if (const auto keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        m_heldScancodes.erase(keyReleased->scancode);
        for (auto& [key, action] : m_keyMap) {
            if (keyReleased->scancode == key) {
                bool stillHeld = false;
                for (const auto& [otherKey, otherAction] : m_keyMap) {
                    if (otherAction == action && m_heldScancodes.count(otherKey)) {
                        stillHeld = true;
                        break;
                    }
                }
                if (!stillHeld) {
                    m_heldActions.erase(action);
                    if (isMovementAction(action)) {
                        popMovementAction(action);
                    }
                }
                m_keyboardEventOccurred = true;
            }
        }
    }
    if (const auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        for (const auto& [button, action] : m_mouseMap) {
            if (mousePressed->button == button) {
                m_pressedActions.insert(action);
                m_heldActions.insert(action);
                m_keyboardEventOccurred = true;
            }
        }
    }
    if (const auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        for (const auto& [button, action] : m_mouseMap) {
            if (mouseReleased->button == button) {
                m_heldActions.erase(action);
                m_keyboardEventOccurred = true;
            }
        }
    }
}

void Input::System::handleJoystick() {
    if (!m_joystick || !sf::Joystick::isConnected(m_joystickId) || m_keyboardEventOccurred) return;

    float x = sf::Joystick::getAxisPosition(m_joystickId, m_horizontalAxis);
    float y = sf::Joystick::getAxisPosition(m_joystickId, m_verticalAxis);

    auto updateJoyDir = [this](Action dir, bool active) {
        if (active) {
            if (!isPressed(dir)) {
                m_pressedActions.insert(dir);
                m_heldActions.insert(dir);
                pushMovementAction(dir);
            }
        }
        else {
            if (isPressed(dir)) {
                m_heldActions.erase(dir);
                popMovementAction(dir);
            }
        }
    };

    updateJoyDir(Action::MoveLeft, x < -m_deadzone);
    updateJoyDir(Action::MoveRight, x > m_deadzone);
    updateJoyDir(Action::MoveUp, y < -m_deadzone);
    updateJoyDir(Action::MoveDown, y > m_deadzone);

    for (auto& [button, action] : m_joystickButtonMap) {
        if (sf::Joystick::isButtonPressed(m_joystickId, button)) {
            if (!isPressed(action)) {
                m_pressedActions.insert(action);
            }
            m_heldActions.insert(action);
        }
        else {
            m_heldActions.erase(action);
        }
    }
}

void Input::System::update() {
    m_movementOrder.erase(
        std::remove_if(m_movementOrder.begin(), m_movementOrder.end(),
            [this](Input::Action a) { return !isPressed(a); }),
        m_movementOrder.end()
    );

    bool movementPressed = (isPressed(Input::Action::MoveLeft)
        || isPressed(Input::Action::MoveRight)
        || isPressed(Input::Action::MoveUp)
        || isPressed(Input::Action::MoveDown)
        );
    if (!movementPressed) {
        pushTimer = 0;
    }
    m_pressedActions.clear();
    m_keyboardEventOccurred = false;
}

bool Input::System::isPressed(Input::Action action) const {
    return m_heldActions.count(action);
}

bool Input::System::wasPressed(Input::Action action) const {
    return m_pressedActions.count(action);
}

void Input::System::increasePushTimer() {
    pushTimer++;
}

bool Input::System::registerPush() const {
    if (pushTimer < 8) {
        return false;
    }
    return pushTimer % 8 == 0;
}

void Input::System::setJoystick(const bool& toggle) {
    m_joystick = toggle;
    if (m_joystick) detectJoystick();
}

void Input::System::detectJoystick() {
    m_joystickId = -1;
    for (unsigned int id = 0; id < sf::Joystick::Count; ++id) {
        if (sf::Joystick::isConnected(id)) {
            m_joystickId = static_cast<int>(id);
            break;
        }
    }
}