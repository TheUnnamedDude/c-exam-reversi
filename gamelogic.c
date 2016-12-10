#include <stdbool.h>
#include "gamelogic.h"

#define NUM_DIRECTIONS 8

int check_add(Board *board, int x_pos, int y_pos, Tile tile, Point points[MAX_TILE_CHANGE]) {
    if (board->tiles[x_pos][y_pos] != NONE) {
        return 0;
    }
    int n_points = 0;
    const Point directions[NUM_DIRECTIONS] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        int tiles_found = n_points;
        int x = x_pos;
        int y = y_pos;
        while (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT)  {
            if (tiles_found >= MAX_TILE_CHANGE + 1) {
                return n_points;
            }
            x += directions[i].x;
            y += directions[i].y;
            if (board->tiles[x][y] == tile) {
                n_points = tiles_found;
            } else if (board->tiles[x][y] != tile && board->tiles[x][y] != NONE) {
                if (points == NULL)
                    continue;
                Point *point = &points[tiles_found++];
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
            if (check_add(board, x, y, tile, NULL)) {
                return true;
            }
        }
    }
    return false;
}
