#include <stdlib.h>
#include <stdio.h>
//#include "board.h"
#include "gamelogic.h"

int main(void) {
    Board board;
    init_board(&board);

    prompt_usernames(&board);

    init_scoreboard(&board);

    int xPos;
    int yPos;

    Tile current_tile = WHITE;

    int nPoints = 0;

    update_scoreboard(&board, current_tile);

    while ((can_move(&board, WHITE) || can_move(&board, BLACK)) 
        && board.tiles_left > 0 && wait_for_input(&board, &xPos, &yPos)) { 
        // The tripple can_move call is a little bit inefficient, but it should
        // be so little overhead that its not worth messing around with
        if (!can_move(&board, current_tile)) {
            current_tile = current_tile == WHITE ? BLACK : WHITE;
            update_scoreboard(&board, current_tile);
            continue;
        }

        Point points[MAX_TILE_CHANGE];
        nPoints = check_add(&board, xPos, yPos, current_tile, points);

        if (nPoints > 0) {
            update_tile(&board, xPos, yPos, current_tile);
            for (int i = 0; i < nPoints; i++) {
                update_tile(&board, points[i].x, points[i].y, current_tile);
            }
            current_tile = current_tile == WHITE ? BLACK : WHITE;
        }

        update_scoreboard(&board, current_tile);
    }

    close_scoreboard(&board);
    show_game_over_screen(&board);
    close_board(&board);

    return 0;
}
