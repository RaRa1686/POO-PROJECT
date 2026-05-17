#pragma once
#include "Player.h"
#include "Ball.h"

class AIPlayer : public Player {
private:
    float startX;
    float startY;
    float reactionDelay;
    float reactionTimer;
    int difficulty;        // 1=easy, 2=medium, 3=hard
    const Ball* currentBall;

public:
    AIPlayer(float x, float y, const sf::Color& color,
             const std::string& name, float groundY, float fieldWidth,
             int difficulty = 2);
    AIPlayer(const AIPlayer& other);
    AIPlayer& operator=(const AIPlayer& other);
    ~AIPlayer() override = default;

    void handleInput() override;
    void updateAI(const Ball& ball);
    void reset() override;
    std::string getInfo() const override;
    AIPlayer* clone() const override { return new AIPlayer(*this); }

    int getDifficulty() const { return difficulty; }
    void setBall(const Ball* ball) { currentBall = ball; }
};