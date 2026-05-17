#include "Game.h"

int main() {
    Game* game = Game::getInstance();
    game->run();
    delete game;
    return 0;
}