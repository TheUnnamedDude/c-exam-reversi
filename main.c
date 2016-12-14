#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "gamelogic.h"
#include "filelog.h"

int main(int argc, char **argv) {
    Board board;
    Log log;

    init_board(&board);

    if (argc > 1) {
        init_log(&log, argv[1]);
    } else {
        init_log(&log, NULL);
    }

    prompt_usernames(&board);
    init_scoreboard(&board);

    log_game_info(&log, &board.player1_name[0], &board.player2_name[0], 4, BOARD_WIDTH, BOARD_HEIGHT);

    int xPos;
    int yPos;

    update_tile(&board, 3, 3, WHITE);
    log_settile(&log, 3, 3, WHITE);
    update_tile(&board, 4, 3, BLACK);
    log_settile(&log, 4, 3, BLACK);
    update_tile(&board, 3, 4, BLACK);
    log_settile(&log, 3, 4, BLACK);
    update_tile(&board, 4, 4, WHITE);
    log_settile(&log, 4, 4, WHITE);

    Tile current_tile = WHITE;

    int nPoints = 0;
    int moves = 0;

    update_scoreboard(&board, current_tile);

    while ((can_move(&board, WHITE) || can_move(&board, BLACK))
        && board.tiles_left > 0 && wait_for_input(&board, &xPos, &yPos)) { 
        // The tripple can_move call is a little bit inefficient as the method itself
        // isnt really that optimal, but since its so small datasets its should be fine
        if (!can_move(&board, current_tile)) {
            current_tile = current_tile == WHITE ? BLACK : WHITE;
            update_scoreboard(&board, current_tile);
            continue;
        }

        Point points[MAX_TILE_CHANGE];
        nPoints = check_add(&board, xPos, yPos, current_tile, points);

        if (nPoints > 0) {
            update_tile(&board, xPos, yPos, current_tile);
            log_move(&log, xPos, yPos, current_tile);
            log_settile(&log, xPos, yPos, current_tile);
            for (int i = 0; i < nPoints; i++) {
                update_tile(&board, points[i].x, points[i].y, current_tile);
                log_settile(&log, points[i].x, points[i].y, current_tile);
            }
            current_tile = current_tile == WHITE ? BLACK : WHITE;
            moves ++;
        }

        update_scoreboard(&board, current_tile);
    }

    log_end_of_moves(&log);
    log_end_game_statistics(&log, board.player1_score, board.player2_score, moves);
    close_log(&log);

    close_scoreboard(&board);
    show_game_over_screen(&board);
    close_board(&board);

    return 0;
}
