#include "Ball.h"
#include <cmath>
#include <sstream>

int Ball::totalBounces = 0;
float Ball::maxSpeed = 0.f;

namespace GameConstants {
    const float GRAVITY     = 800.f;
    const float BOUNCE_FACTOR = 0.6f;
}

Ball::Ball(float x, float y, float gY, float fWidth)
    : GameObject(x, y, 40.f, 40.f),
      radius(20.f), startX(x), startY(y),
      groundY(gY), fieldWidth(fWidth)
{
    shape.setRadius(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition({x, y});
    shape.setFillColor(sf::Color::White);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.f);
    velocity = {200.f, 0.f};
}

Ball::Ball(const Ball& other)
    : GameObject(other),
      shape(other.shape), radius(other.radius),
      startX(other.startX), startY(other.startY),
      groundY(other.groundY), fieldWidth(other.fieldWidth) {}

Ball& Ball::operator=(const Ball& other) {
    if (this != &other) {
        GameObject::operator=(other);
        shape = other.shape;
        radius = other.radius;
        startX = other.startX;
        startY = other.startY;
        groundY = other.groundY;
        fieldWidth = other.fieldWidth;
    }
    return *this;
}

void Ball::update(float dt) {
    velocity.y += GameConstants::GRAVITY * dt;
    position += velocity * dt;
    shape.setPosition(position);

    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > maxSpeed) maxSpeed = speed;

    if (position.x - radius < 0.f) {
        position.x = radius;
        velocity.x = std::abs(velocity.x);
        totalBounces++;
    }
    if (position.x + radius > fieldWidth) {
        position.x = fieldWidth - radius;
        velocity.x = -std::abs(velocity.x);
        totalBounces++;
    }
    if (position.y + radius > groundY) {
        position.y = groundY - radius;
        velocity.y = -std::abs(velocity.y) * GameConstants::BOUNCE_FACTOR;
        velocity.x *= 0.98f;
        totalBounces++;
    }
    shape.setPosition(position);
}

void Ball::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

void Ball::reset() {
    position = {startX, startY};
    velocity = {200.f, 0.f};
    shape.setPosition(position);
}

void Ball::applyImpulse(float nx, float ny, float speed) {
    if (speed < 0.f)
        throw InvalidMoveException("Viteza impulsului nu poate fi negativa!");
    velocity.x = nx * speed;
    velocity.y = ny * speed;
    if (velocity.y > 0) velocity.y *= -1.f;
}

std::string Ball::getInfo() const {
    std::ostringstream oss;
    oss << "Ball[pos=(" << position.x << "," << position.y << ") "
        << "vel=(" << velocity.x << "," << velocity.y << ")]";
    return oss.str();
}

Ball Ball::operator+(const sf::Vector2f& offset) const {
    Ball result(*this);
    result.position += offset;
    result.shape.setPosition(result.position);
    return result;
}

Ball operator*(const Ball& ball, float scalar) {
    Ball result(ball);
    sf::Vector2f vel = result.getVelocity();
    result.setVelocity({vel.x * scalar, vel.y * scalar});
    return result;
}