#pragma once
#include "GameObject.h"
#include "GameException.h"

class Ball : public GameObject {
private:
    sf::CircleShape shape;
    float radius;
    float startX;
    float startY;
    float groundY;
    float fieldWidth;

    static int totalBounces;
    static float maxSpeed;

public:
    Ball(float x, float y, float groundY, float fieldWidth);
    Ball(const Ball& other);
    Ball& operator=(const Ball& other);
    ~Ball() override = default;

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    void reset() override;

    float getRadius() const { return radius; }
    const sf::CircleShape& getShape() const { return shape; }

    void applyImpulse(float nx, float ny, float speed);

    static int getTotalBounces() { return totalBounces; }
    static float getMaxSpeed() { return maxSpeed; }
    static void resetStats() { totalBounces = 0; maxSpeed = 0.f; }

    std::string getInfo() const override;
    Ball* clone() const override { return new Ball(*this); }

    // Operator membru: deplasare pozitie
    Ball operator+(const sf::Vector2f& offset) const;
};

// Operator non-membru: scalare viteza
Ball operator*(const Ball& ball, float scalar);