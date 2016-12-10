#include <stdbool.h>
#include "gamelogic.h"

#define NUM_DIRECTIONS 8

const Point DIRECTIONS[NUM_DIRECTIONS] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };

int check_add(Board *board, int x_pos, int y_pos, Tile tile, Point points[MAX_TILE_CHANGE]) {
    if (board->tiles[x_pos][y_pos] != NONE) {
        return 0;
    }

    int n_points = 0;

    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        int tiles_found = n_points;
        int x = x_pos;
        int y = y_pos;

        while ((x+=DIRECTIONS[i].x) >= 0 && x < BOARD_WIDTH 
            && (y+=DIRECTIONS[i].y) >= 0 && y < BOARD_HEIGHT)  {

            if (tiles_found >= MAX_TILE_CHANGE + 1) {
                return n_points;
            }

            if (board->tiles[x][y] == tile) {
                n_points = tiles_found;
            } else if (board->tiles[x][y] != tile && board->tiles[x][y] != NONE) {
                tiles_found ++;
                if (points == NULL)
                    continue;

                Point *point = &points[tiles_found-1];
                point->x = x;
                point->y = y;
                continue;
            }// Found NONE or the tile, we should stop looking

            break;
        }
    }

    return n_points;
}

bool can_move(Board *board, Tile tile) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            if (check_add(board, x, y, tile, NULL) > 0) {
                return true;
            }
        }
    }

    return false;
}
