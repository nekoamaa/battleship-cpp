#include "header.h"

int main() {
    // declare player1 and player2's boards
    Player player1, player2;

    // selects the game mode
    int gameMode = chooseGameMode();

    // start the game
    play(player1, player2, gameMode);
    return 0;
}
