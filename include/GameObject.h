#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class GameObject {
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float width;
    float height;

public:
    GameObject(float x, float y, float w, float h);
    GameObject(const GameObject& other);
    GameObject& operator=(const GameObject& other);
    virtual ~GameObject() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual void reset() = 0;

    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getVelocity() const { return velocity; }
    void setPosition(const sf::Vector2f& pos) { position = pos; }
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }

    virtual std::string getInfo() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const GameObject& obj);
    friend std::istream& operator>>(std::istream& is, GameObject& obj);

    bool operator==(const GameObject& other) const;
    virtual GameObject* clone() const = 0;
};