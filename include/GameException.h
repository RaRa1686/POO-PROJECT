#pragma once
#include <exception>
#include <string>

// Clasa de baza pentru exceptii - pornita de la std::exception
class GameException : public std::exception {
protected:
    std::string message;
public:
    explicit GameException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
    virtual ~GameException() = default;
};

// Exceptie speciala pentru goluri - demonstreaza propagarea si upcasting-ul
class GoalException : public GameException {
private:
    int scoringPlayer;
public:
    GoalException(int player, const std::string& msg)
        : GameException(msg), scoringPlayer(player) {}
    int getScoringPlayer() const { return scoringPlayer; }
};

// Exceptie pentru mutari invalide - ilustreaza blocurile catch cu upcast
class InvalidMoveException : public GameException {
public:
    explicit InvalidMoveException(const std::string& msg) : GameException(msg) {}
};