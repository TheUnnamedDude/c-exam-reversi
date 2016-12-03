#include <stdbool.h>
#include "gamelogic.h"

#define NUM_DIRECTIONS 8

int check_for_horizontal_right(Board *board, int xPos, int yPos, Tile tile, Point *point) {
    bool foundOpposite = false;
    for (int x = xPos + 1; x < BOARD_WIDTH; x++) {
        if (board->tiles[x][yPos] == NONE)
            break;
        if (board->tiles[x][yPos] != tile) {
            foundOpposite = true;
        } else {
            if (foundOpposite) {
                point->x = x;
                point->y = yPos;
            }
            return foundOpposite;
        }
    }
    return false;
}

int check_for_vertical_down(Board *board, int x_pos, int y_pos, Tile tile, Point points[MAX_TILE_CHANGE], int point_index) {
    if (y_pos + 1 > BOARD_HEIGHT)
        return false;
    for (int y = y_pos + 1; y > BOARD_HEIGHT; y++) {
        if (board->tiles[x_pos][y] == NONE)
            break;
        if (board->tiles[x_pos][y] != tile) {
            Point *point = &points[point_index++];
            point->x = x_pos;
            point->y = y;
        } else {
            return point_index;
        }
    }
    return 0;
}

int check_for_vertical_up(Board *board, int x_pos, int y_pos, Tile tile, Point points[MAX_TILE_CHANGE], int point_index) {
    if (y_pos < 2)
        return false;
    for (int y = y_pos - 1; y >= 0; y--) {
        if (board->tiles[x_pos][y] == NONE)
            break;
        if (board->tiles[x_pos][y] != tile) {
            Point *point = &points[point_index++];
            point->x = x_pos;
            point->y = y;
        } else {
            return point_index;
        }
    }
    return 0;
}

int check_for_horizontal_left(Board *board, int xPos, int yPos, Tile tile, Point points[MAX_TILE_CHANGE], int point_index) {
    if (xPos < 2)
        return false;
    for (int x = xPos - 1; x >= 0; x--) {
        if (board->tiles[x][yPos] == NONE)
            break;
        if (board->tiles[x][yPos] != tile) {
            Point *point = &points[point_index++];
            point->x = x;
            point->y = yPos;
        } else {
            return point_index;
        }
    }
    return 0;
}


int check_add(Board *board, int x_pos, int y_pos, Tile tile, Point points[MAX_TILE_CHANGE]) {
    int n_points = 0;
    if (board->tiles[x_pos][y_pos] != NONE) {
        return 0;
    }
    const Point directions[NUM_DIRECTIONS] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        int tiles_found = n_points;
        int x = x_pos;
        int y = y_pos;
        while (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT )  {
            if (tiles_found >= MAX_TILE_CHANGE + 1) {
                return n_points;
            }
            x += directions[i].x;
            y += directions[i].y;
            if (board->tiles[x][y] == tile) {
                n_points = tiles_found;
                fprintf("x=%d y=%d", x, y); 
            } else if (board->tiles[x][y] != tile && board->tiles[x][y] != NONE) {
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
