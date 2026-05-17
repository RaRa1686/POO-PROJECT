#pragma once
#include "GameObject.h"
#include <string>

class Player : public GameObject {
protected:
    // Cap mare
    sf::CircleShape head;
    // Ochi
    sf::CircleShape eyeLeft;
    sf::CircleShape eyeRight;
    // Pupile
    sf::CircleShape pupilLeft;
    sf::CircleShape pupilRight;
    // Sprancene
    sf::RectangleShape browLeft;
    sf::RectangleShape browRight;
    // Gura
    sf::RectangleShape mouth;
    // Obraz (rosu)
    sf::CircleShape cheekLeft;
    sf::CircleShape cheekRight;
    // Picioare
    sf::RectangleShape legLeft;
    sf::RectangleShape legRight;
    // Corp mic
    sf::RectangleShape body;

    int score;
    std::string name;
    sf::Color color;
    float groundY;
    float fieldWidth;
    bool onGround;
    bool facingRight;

    static int totalPlayers;

    void updateParts();

public:
    Player(float x, float y, const sf::Color& color,
           const std::string& name, float groundY, float fieldWidth);
    Player(const Player& other);
    Player& operator=(const Player& other);
    virtual ~Player() = default;

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    void reset() override;

    virtual void handleInput() = 0;

    int getScore() const { return score; }
    void addScore() { score++; }
    const std::string& getName() const { return name; }
    const sf::CircleShape& getHead() const { return head; }

    static int getTotalPlayers() { return totalPlayers; }

    std::string getInfo() const override;
    virtual Player* clone() const override = 0;

    bool operator<(const Player& other) const { return score < other.score; }
    Player& operator+=(int points);

    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};