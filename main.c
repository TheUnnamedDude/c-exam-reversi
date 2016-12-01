#include <stdlib.h>
#include <stdio.h>
#include "board.h"

int main(int argc, char **argv) {
    Board board;
    init_board(&board);

    int xPos;
    int yPos;

    while (wait_for_input(&xPos, &yPos)) {
        update_tile(&board, xPos, yPos, WHITE);
    }
    close_board(&board);
    return 0;
}
