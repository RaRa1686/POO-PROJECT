#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <memory>
#include <optional>
#include "Player.h"
#include "HumanPlayer.h"
#include "AIPlayer.h"
#include "Ball.h"
#include "ScoreBoard.h"
#include "Observer.h"

class Game : public ISubject, public IObserver {
private:
    static Game* instance;

    sf::RenderWindow window;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    std::unique_ptr<Ball> ball;

    sf::Font font;
    std::optional<sf::Text> timerText;

    float groundY;
    float timeLeft;
    bool gameOver;
    bool vsAI;

    ScoreBoard<int>   scoreBoard;
    ScoreBoard<float> statsBoard;

    std::list<std::string> eventLog;
    std::vector<IObserver*> observers;

    enum class GameState { MENU, PLAYING, GAMEOVER };
    GameState state;

    Game();

public:
    static Game* getInstance();
    ~Game() override;
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void run();
    void addObserver(IObserver* obs) override;
    void notifyObservers(const std::string& event) override;
    void onEvent(const std::string& event) override;

private:
    void handleEvents();
    void update(float dt);
    void draw();
    void drawMenu();
    void drawGame();
    void drawGameOver();
    void drawDarkStadium();
    void drawGoal(bool leftGoal);
    void drawScoreboard();
    void checkBallPlayerCollision(Player& player);
    void checkPlayerPlayerCollision();
    void checkGoal();
    void initGame(bool ai);
    void demonstratePolymorphism();
};