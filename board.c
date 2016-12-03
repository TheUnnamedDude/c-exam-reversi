#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include "board.h"

#define WHITE_TILE_COLOR 1
#define BLACK_TILE_COLOR 2
#define NONE_TILE_COLOR 3
#define BORDER_COLOR 4

#define X_END BOARD_WIDTH * 4
#define Y_END BOARD_HEIGHT * 2

void color_char(int x, int y, char c, int color, Board *board) {
    if (board->has_color) {
        wattron(board->internal, COLOR_PAIR(color));
    }
    mvwaddch(board->internal, y, x, c);
    if (board->has_color) {
        wattroff(board->internal, COLOR_PAIR(color));
    }
}

void init_board(Board *board) {
    initscr();
    curs_set(FALSE);
    noecho();
    board->internal = newwin(Y_END + 1, X_END + 1, 0, 0);
    board->has_color = TRUE;
    if (board->has_color) {
        start_color();
        init_pair(WHITE_TILE_COLOR, COLOR_WHITE, COLOR_WHITE);
        init_pair(BLACK_TILE_COLOR, COLOR_BLACK, COLOR_BLACK);
        init_pair(NONE_TILE_COLOR, COLOR_BLUE, COLOR_BLUE);
        init_pair(BORDER_COLOR, COLOR_WHITE, COLOR_BLUE);
    }
    keypad(board->internal, TRUE);
    mousemask(BUTTON1_CLICKED, NULL);
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
        wattron(board->internal, attr);
        mvwaddch(board->internal, 1 + y * 2, 1 + x * 4, tile);
        mvwaddch(board->internal, 1 + y * 2, 3 + x * 4, tile);
    }
    mvwaddch(board->internal, 1 + y * 2, 2 + x * 4, tile);
    if (board->has_color) {
        wattroff(board->internal, attr);
    }
    board->tiles[x][y] = tile;
}

void update_tile(Board *board, int x, int y, Tile tile) {
    set_tile(board, x, y, tile);
    wrefresh(board->internal);
}

void repaint_board(Board *board) {
    for (int x = 0; x <= X_END; x++) {
        for (int y = 0; y <= Y_END; y++) {
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
    wrefresh(board->internal);
}

void close_board(Board *board) {
    if (board != NULL) {
        endwin();
    }
}

int translate_position(int absoluteX, int absoluteY, int *relativeX, int *relativeY) {
    if (absoluteX % 4 == 0 || absoluteY % 2 == 0) {
        return 0;
    }
    if (absoluteX >= X_END || absoluteY >= Y_END) {
        return 0;
    }
    *relativeX = absoluteX / 4;
    *relativeY = absoluteY / 2;
    return 1;
}

bool wait_for_input(Board *board, int *x, int *y) {
    MEVENT event;
    int ch = wgetch(board->internal);
    if (ch == KEY_MOUSE) {
        if (getmouse(&event) == OK) {
            if (translate_position(event.x, event.y, x, y)) {
                return true; // We got input, lets just give that back to the user
            }
        }
    } else if (ch == 'q' || ch == 'Q') {
        return false;
    }
    return wait_for_input(board, x, y);
}
