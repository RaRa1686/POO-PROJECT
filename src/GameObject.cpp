#include "GameObject.h"

GameObject::GameObject(float x, float y, float w, float h)
    : position(x, y), velocity(0.f, 0.f), width(w), height(h) {}

GameObject::GameObject(const GameObject& other)
    : position(other.position), velocity(other.velocity),
      width(other.width), height(other.height) {}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this != &other) {
        position = other.position;
        velocity = other.velocity;
        width = other.width;
        height = other.height;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const GameObject& obj) {
    os << obj.getInfo();
    return os;
}

std::istream& operator>>(std::istream& is, GameObject& obj) {
    float x, y;
    is >> x >> y;
    obj.position = {x, y};
    return is;
}

bool GameObject::operator==(const GameObject& other) const {
    return position.x == other.position.x && position.y == other.position.y;
}