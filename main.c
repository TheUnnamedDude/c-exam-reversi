#include <stdlib.h>
#include <stdio.h>
//#include "board.h"
#include "gamelogic.h"

int main(void) {
    Board board;
    init_board(&board);

    char player1_name[MAX_PLAYERNAME_SIZE + 1];
    char player2_name[MAX_PLAYERNAME_SIZE + 1];
    prompt_usernames(&board, player1_name, player2_name);

    init_scoreboard(&board, player1_name, player2_name);

    int xPos;
    int yPos;
    Tile current_tile = WHITE;
    int nPoints = 0;
    update_scoreboard(&board, 0, 0, current_tile);
    while (wait_for_input(&board, &xPos, &yPos)) {
        Point points[MAX_TILE_CHANGE];
        nPoints = check_add(&board, xPos, yPos, current_tile, points);
        if (nPoints > 0) {
            update_tile(&board, xPos, yPos, current_tile);
            for (int i = 0; i < nPoints; i++) {
                update_tile(&board, points[i].x, points[i].y, current_tile);
            }
            if (current_tile == WHITE) {
                current_tile = BLACK;
            } else {
                current_tile = WHITE;
            }
        }
        update_scoreboard(&board, 0, 0, current_tile);
    }
    close_board(&board);
    printf("User1: %s, User2: %s\n", player1_name, player2_name);
    return 0;
}
