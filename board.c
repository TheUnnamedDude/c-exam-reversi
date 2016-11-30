#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include "board.h"

#define WHITE_TILE_COLOR 1
#define BLACK_TILE_COLOR 2
#define NONE_TILE_COLOR 3
#define BORDER_COLOR 4

int BOARD_WIDTH = 8;
int BOARD_HEIGHT = 8;

void color_char(int x, int y, char c, int color, Board *board) {
    if (board->has_color) {
        attron(COLOR_PAIR(color));
    }
    mvaddch(y, x, c);
    if (board->has_color) {
        attroff(COLOR_PAIR(color));
    }
}

void init_board(Board *board) {
    initscr();
    curs_set(FALSE);
    board->has_color = true;
    if (board->has_color) {
        start_color();
        init_pair(WHITE_TILE_COLOR, COLOR_WHITE, COLOR_WHITE);
        init_pair(BLACK_TILE_COLOR, COLOR_BLACK, COLOR_BLACK);
        init_pair(NONE_TILE_COLOR, COLOR_BLUE, COLOR_BLUE);
        init_pair(BORDER_COLOR, COLOR_WHITE, COLOR_BLUE);
    }
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_WIDTH; y++) {
            board->tiles[x][y] = NONE;
        }
    }
    board->tiles[3][3] = WHITE;
    board->tiles[4][3] = BLACK;
    board->tiles[3][4] = BLACK;
    board->tiles[4][4] = WHITE;
    repaint_board(board);
}

void set_tile(Board *board, int x, int y, Tile tile) {
    int attr;
    if (board->has_color) {
        switch (tile) {
            case WHITE:
                attr = COLOR_PAIR(WHITE_TILE_COLOR);
                break;
          case BLACK:
                attr = COLOR_PAIR(BLACK_TILE_COLOR);
                break;
          case NONE:
                attr = COLOR_PAIR(NONE_TILE_COLOR);
                break;
        }
        attron(attr);
        mvaddch(1 + y * 2, 1 + x * 4, tile);
        mvaddch(1 + y * 2, 3 + x * 4, tile);
    }
    mvaddch(1 + y * 2, 2 + x * 4, tile);
    if (board->has_color) {
        attroff(attr);
    }
    board->tiles[x][y] = tile;
}

void update_tile(Board *board, int x, int y, Tile tile) {
    set_tile(board, x, y, tile);
    refresh();
}

void repaint_board(Board *board) {
    int x_end = BOARD_WIDTH * 4;
    int y_end = BOARD_HEIGHT * 2;
    for (int x = 0; x <= x_end; x++) {
        for (int y = 0; y <= y_end; y++) {
            if (x % 4 == 0 && y % 2 == 0) {
                color_char(x, y, '+', BORDER_COLOR, board);
            } else if (x % 4 == 0) {
                color_char(x, y, '|', BORDER_COLOR, board);
            } else if (y % 2 == 0) {
                color_char(x, y, '-', BORDER_COLOR, board);
            }
        }
    }
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            set_tile(board, x, y, board->tiles[x][y]);
        }
    }
    refresh();
}

void close_board(Board *board) {
    if (board != NULL) {
        getch();
        endwin();
    }
}
