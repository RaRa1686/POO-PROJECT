#pragma once
#include "Player.h"

class HumanPlayer : public Player {
private:
    sf::Keyboard::Key keyLeft;
    sf::Keyboard::Key keyRight;
    sf::Keyboard::Key keyJump;
    float startX;
    float startY;

public:
    HumanPlayer(float x, float y, const sf::Color& color,
                const std::string& name, float groundY, float fieldWidth,
                sf::Keyboard::Key kLeft, sf::Keyboard::Key kRight, sf::Keyboard::Key kJump);
    HumanPlayer(const HumanPlayer& other);
    HumanPlayer& operator=(const HumanPlayer& other);
    ~HumanPlayer() override = default;

    void handleInput() override;
    void reset() override;
    std::string getInfo() const override;
    HumanPlayer* clone() const override { return new HumanPlayer(*this); }
};