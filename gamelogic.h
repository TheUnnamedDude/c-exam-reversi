#include "board.h"

#define MAX_TILE_CHANGE BOARD_WIDTH + BOARD_HEIGHT + (BOARD_WIDTH < BOARD_HEIGHT ? BOARD_WIDTH : BOARD_HEIGHT) * 2 - 16

typedef struct Point {
    int x;
    int y;
} Point;

int check_add(Board *board, int xPos, int yPos, Tile tile, Point points[MAX_TILE_CHANGE]);
