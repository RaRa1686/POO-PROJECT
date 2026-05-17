#include "Player.h"
#include <sstream>

int Player::totalPlayers = 0;

namespace PlayerConstants {
    const float HEAD_RADIUS = 38.f;
    const float BODY_W      = 30.f;
    const float BODY_H      = 28.f;
    const float LEG_W       = 12.f;
    const float LEG_H       = 22.f;
    const float GRAVITY_P   = 800.f;
    const float TOTAL_H     = HEAD_RADIUS * 2 + BODY_H + LEG_H;
}

Player::Player(float x, float y, const sf::Color& col,
               const std::string& nm, float gY, float fWidth)
    : GameObject(x, y, PlayerConstants::BODY_W * 2,
                 PlayerConstants::TOTAL_H),
      score(0), name(nm), color(col),
      groundY(gY), fieldWidth(fWidth), onGround(false), facingRight(true)
{
    // Cap mare
    head.setRadius(PlayerConstants::HEAD_RADIUS);
    head.setOrigin({PlayerConstants::HEAD_RADIUS, PlayerConstants::HEAD_RADIUS});
    head.setFillColor(color);
    head.setOutlineColor(sf::Color::Black);
    head.setOutlineThickness(2.f);

    // Ochi albi
    eyeLeft.setRadius(9.f);
    eyeLeft.setOrigin({9.f, 9.f});
    eyeLeft.setFillColor(sf::Color::White);
    eyeLeft.setOutlineColor(sf::Color::Black);
    eyeLeft.setOutlineThickness(1.5f);

    eyeRight.setRadius(9.f);
    eyeRight.setOrigin({9.f, 9.f});
    eyeRight.setFillColor(sf::Color::White);
    eyeRight.setOutlineColor(sf::Color::Black);
    eyeRight.setOutlineThickness(1.5f);

    // Pupile
    pupilLeft.setRadius(5.f);
    pupilLeft.setOrigin({5.f, 5.f});
    pupilLeft.setFillColor(sf::Color::Black);

    pupilRight.setRadius(5.f);
    pupilRight.setOrigin({5.f, 5.f});
    pupilRight.setFillColor(sf::Color::Black);

    // Sprancene negre groase
    browLeft.setSize({18.f, 5.f});
    browLeft.setOrigin({9.f, 2.5f});
    browLeft.setFillColor(sf::Color(30, 20, 10));

    browRight.setSize({18.f, 5.f});
    browRight.setOrigin({9.f, 2.5f});
    browRight.setFillColor(sf::Color(30, 20, 10));

    // Gura
    mouth.setSize({20.f, 5.f});
    mouth.setOrigin({10.f, 2.5f});
    mouth.setFillColor(sf::Color(150, 30, 30));

    // Obraji roz
    cheekLeft.setRadius(7.f);
    cheekLeft.setOrigin({7.f, 7.f});
    cheekLeft.setFillColor(sf::Color(255, 150, 150, 160));

    cheekRight.setRadius(7.f);
    cheekRight.setOrigin({7.f, 7.f});
    cheekRight.setFillColor(sf::Color(255, 150, 150, 160));

    // Corp
    body.setSize({PlayerConstants::BODY_W, PlayerConstants::BODY_H});
    body.setOrigin({PlayerConstants::BODY_W / 2.f, 0.f});
    body.setFillColor(color);
    body.setOutlineColor(sf::Color::Black);
    body.setOutlineThickness(2.f);

    // Picioare
    legLeft.setSize({PlayerConstants::LEG_W, PlayerConstants::LEG_H});
    legLeft.setOrigin({PlayerConstants::LEG_W / 2.f, 0.f});
    legLeft.setFillColor(sf::Color(40, 40, 120));
    legLeft.setOutlineColor(sf::Color::Black);
    legLeft.setOutlineThickness(1.5f);

    legRight.setSize({PlayerConstants::LEG_W, PlayerConstants::LEG_H});
    legRight.setOrigin({PlayerConstants::LEG_W / 2.f, 0.f});
    legRight.setFillColor(sf::Color(40, 40, 120));
    legRight.setOutlineColor(sf::Color::Black);
    legRight.setOutlineThickness(1.5f);

    updateParts();
    totalPlayers++;
}

Player::Player(const Player& other)
    : GameObject(other),
      head(other.head), eyeLeft(other.eyeLeft), eyeRight(other.eyeRight),
      pupilLeft(other.pupilLeft), pupilRight(other.pupilRight),
      browLeft(other.browLeft), browRight(other.browRight),
      mouth(other.mouth), cheekLeft(other.cheekLeft), cheekRight(other.cheekRight),
      legLeft(other.legLeft), legRight(other.legRight), body(other.body),
      score(other.score), name(other.name), color(other.color),
      groundY(other.groundY), fieldWidth(other.fieldWidth),
      onGround(other.onGround), facingRight(other.facingRight)
{
    totalPlayers++;
}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        GameObject::operator=(other);
        head = other.head;
        eyeLeft = other.eyeLeft; eyeRight = other.eyeRight;
        pupilLeft = other.pupilLeft; pupilRight = other.pupilRight;
        browLeft = other.browLeft; browRight = other.browRight;
        mouth = other.mouth;
        cheekLeft = other.cheekLeft; cheekRight = other.cheekRight;
        legLeft = other.legLeft; legRight = other.legRight;
        body = other.body;
        score = other.score; name = other.name; color = other.color;
        groundY = other.groundY; fieldWidth = other.fieldWidth;
        onGround = other.onGround; facingRight = other.facingRight;
    }
    return *this;
}

void Player::updateParts() {
    float px = position.x;
    float py = position.y;

    // Picioare jos
    legLeft.setPosition ({px - 9.f,  py - PlayerConstants::LEG_H});
    legRight.setPosition({px + 9.f,  py - PlayerConstants::LEG_H});

    // Corp deasupra picioarelor
    float bodyTop = py - PlayerConstants::LEG_H - PlayerConstants::BODY_H;
    body.setPosition({px, bodyTop});

    // Cap deasupra corpului
    float headY = bodyTop - PlayerConstants::HEAD_RADIUS;
    head.setPosition({px, headY});

    // Directia privirii
    float eyeOffsetX = facingRight ? 10.f : -10.f;

    eyeLeft.setPosition ({px - 13.f, headY - 8.f});
    eyeRight.setPosition({px + 13.f, headY - 8.f});

    pupilLeft.setPosition ({px - 13.f + eyeOffsetX * 0.3f, headY - 7.f});
    pupilRight.setPosition({px + 13.f + eyeOffsetX * 0.3f, headY - 7.f});

    browLeft.setPosition ({px - 13.f, headY - 19.f});
    browRight.setPosition({px + 13.f, headY - 19.f});

    // Sprancene inclinate in functie de directie (expresie)
    browLeft.setRotation (sf::degrees(facingRight ? -10.f : 10.f));
    browRight.setRotation(sf::degrees(facingRight ? 10.f : -10.f));

    mouth.setPosition({px, headY + 18.f});

    cheekLeft.setPosition ({px - 22.f, headY + 5.f});
    cheekRight.setPosition({px + 22.f, headY + 5.f});
}

void Player::update(float dt) {
    velocity.y += PlayerConstants::GRAVITY_P * dt;
    position += velocity * dt;

    if (position.y >= groundY) {
        position.y = groundY;
        velocity.y = 0.f;
        onGround = true;
    }
    if (position.x < PlayerConstants::BODY_W) {
        position.x = PlayerConstants::BODY_W;
        velocity.x = 0.f;
    }
    if (position.x > fieldWidth - PlayerConstants::BODY_W) {
        position.x = fieldWidth - PlayerConstants::BODY_W;
        velocity.x = 0.f;
    }

    if (velocity.x > 0.f)       facingRight = true;
    else if (velocity.x < 0.f)  facingRight = false;

    updateParts();
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(legLeft);
    window.draw(legRight);
    window.draw(body);
    window.draw(head);
    window.draw(cheekLeft);
    window.draw(cheekRight);
    window.draw(eyeLeft);
    window.draw(eyeRight);
    window.draw(pupilLeft);
    window.draw(pupilRight);
    window.draw(browLeft);
    window.draw(browRight);
    window.draw(mouth);
}

void Player::reset() {
    score = 0;
    onGround = false;
    velocity = {0.f, 0.f};
}

Player& Player::operator+=(int points) {
    score += points;
    return *this;
}

std::string Player::getInfo() const {
    std::ostringstream oss;
    oss << "Player[" << name << " score=" << score
        << " pos=(" << position.x << "," << position.y << ")]";
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << p.getInfo();
    return os;
}