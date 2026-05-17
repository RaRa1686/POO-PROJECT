#include "HumanPlayer.h"
#include "GameException.h"
#include <sstream>

namespace HumanConstants {
    const float SPEED      = 300.f;
    const float JUMP_FORCE = -620.f;
    const float LEG_H      = 22.f;
    const float BODY_H     = 28.f;
    const float HEAD_R     = 38.f;
}

HumanPlayer::HumanPlayer(float x, float y, const sf::Color& color,
                         const std::string& name, float groundY, float fieldWidth,
                         sf::Keyboard::Key kLeft, sf::Keyboard::Key kRight, sf::Keyboard::Key kJump)
    : Player(x, y, color, name, groundY, fieldWidth),
      keyLeft(kLeft), keyRight(kRight), keyJump(kJump),
      startX(x), startY(y) {}

HumanPlayer::HumanPlayer(const HumanPlayer& other)
    : Player(other),
      keyLeft(other.keyLeft), keyRight(other.keyRight), keyJump(other.keyJump),
      startX(other.startX), startY(other.startY) {}

HumanPlayer& HumanPlayer::operator=(const HumanPlayer& other) {
    if (this != &other) {
        Player::operator=(other);
        keyLeft  = other.keyLeft;
        keyRight = other.keyRight;
        keyJump  = other.keyJump;
        startX   = other.startX;
        startY   = other.startY;
    }
    return *this;
}

void HumanPlayer::handleInput() {
    velocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(keyLeft))  velocity.x = -HumanConstants::SPEED;
    if (sf::Keyboard::isKeyPressed(keyRight)) velocity.x =  HumanConstants::SPEED;
    if (sf::Keyboard::isKeyPressed(keyJump) && onGround) {
        velocity.y = HumanConstants::JUMP_FORCE;
        onGround = false;
    }
}

void HumanPlayer::reset() {
    Player::reset();
    position = {startX, startY};
    updateParts();
}

std::string HumanPlayer::getInfo() const {
    std::ostringstream oss;
    oss << "Human" << Player::getInfo();
    return oss.str();
}