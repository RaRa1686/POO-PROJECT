#include "AIPlayer.h"
#include <cmath>
#include <sstream>

namespace AIConstants {
    const float SPEED  = 250.f;
    const float JUMP   = -620.f;
}

AIPlayer::AIPlayer(float x, float y, const sf::Color& color,
                   const std::string& name, float groundY, float fieldWidth,
                   int diff)
    : Player(x, y, color, name, groundY, fieldWidth),
      startX(x), startY(y),
      reactionDelay(diff == 1 ? 0.3f : diff == 2 ? 0.15f : 0.05f),
      reactionTimer(0.f), difficulty(diff), currentBall(nullptr) {}

AIPlayer::AIPlayer(const AIPlayer& other)
    : Player(other),
      startX(other.startX), startY(other.startY),
      reactionDelay(other.reactionDelay), reactionTimer(other.reactionTimer),
      difficulty(other.difficulty), currentBall(other.currentBall) {}

AIPlayer& AIPlayer::operator=(const AIPlayer& other) {
    if (this != &other) {
        Player::operator=(other);
        startX        = other.startX;
        startY        = other.startY;
        reactionDelay = other.reactionDelay;
        reactionTimer = other.reactionTimer;
        difficulty    = other.difficulty;
        currentBall   = other.currentBall;
    }
    return *this;
}

void AIPlayer::handleInput() {
    if (!currentBall) return;
    updateAI(*currentBall);
}

void AIPlayer::updateAI(const Ball& ball) {
    sf::Vector2f ballPos = ball.getPosition();
    float dx = ballPos.x - position.x;
    float speed = AIConstants::SPEED * (difficulty / 3.f + 0.5f);

    if (std::abs(dx) > 10.f)
        velocity.x = (dx > 0) ? speed : -speed;
    else
        velocity.x = 0.f;

    if (ballPos.y < position.y - 100.f && onGround && std::abs(dx) < 150.f) {
        velocity.y = AIConstants::JUMP;
        onGround = false;
    }
}

void AIPlayer::reset() {
    Player::reset();
    position = {startX, startY};
    updateParts();
}

std::string AIPlayer::getInfo() const {
    std::ostringstream oss;
    oss << "AI(diff=" << difficulty << ")" << Player::getInfo();
    return oss.str();
}