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

typedef struct BoardInfo {
    WINDOW *main_window;
    WINDOW *scoreboard_window;
} BoardInfo;

void color_char(int x, int y, char c, int color, Board *board, WINDOW *window) {
    if (board->has_color) {
        wattron(window, COLOR_PAIR(color));
    }
    mvwaddch(window, y, x, c);
    if (board->has_color) {
        wattroff(window, COLOR_PAIR(color));
    }
}

void init_board(Board *board) {
    initscr();
    curs_set(FALSE);
    noecho();
    BoardInfo *board_info = malloc(sizeof(BoardInfo));
    board_info->main_window = newwin(Y_END + 1, X_END + 1, 0, 0);
    board->internal = board_info;
    board->has_color = TRUE;
    if (board->has_color) {
        start_color();
        init_pair(WHITE_TILE_COLOR, COLOR_WHITE, COLOR_WHITE);
        init_pair(BLACK_TILE_COLOR, COLOR_BLACK, COLOR_BLACK);
        init_pair(NONE_TILE_COLOR, COLOR_BLUE, COLOR_BLUE);
        init_pair(BORDER_COLOR, COLOR_WHITE, COLOR_BLUE);
    }
    keypad(board_info->main_window, TRUE);
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
    BoardInfo *board_info = board->internal;
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
        wattron(board_info->main_window, attr);
        mvwaddch(board_info->main_window, 1 + y * 2, 1 + x * 4, tile);
        mvwaddch(board_info->main_window, 1 + y * 2, 3 + x * 4, tile);
    }
    mvwaddch(board_info->main_window, 1 + y * 2, 2 + x * 4, tile);
    if (board->has_color) {
        wattroff(board_info->main_window, attr);
    }
    board->tiles[x][y] = tile;
}

void update_tile(Board *board, int x, int y, Tile tile) {
    BoardInfo *board_info = board->internal;
    set_tile(board, x, y, tile);
    wrefresh(board_info->main_window);
}

void repaint_board(Board *board) {
    BoardInfo *board_info = board->internal;
    for (int x = 0; x <= X_END; x++) {
        for (int y = 0; y <= Y_END; y++) {
            if (x % 4 == 0 && y % 2 == 0) {
                color_char(x, y, '+', BORDER_COLOR, board, board_info->main_window);
            } else if (x % 4 == 0) {
                color_char(x, y, '|', BORDER_COLOR, board, board_info->main_window);
            } else if (y % 2 == 0) {
                color_char(x, y, '-', BORDER_COLOR, board, board_info->main_window);
            }
        }
    }
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            set_tile(board, x, y, board->tiles[x][y]);
        }
    }
    wrefresh(board_info->main_window);
}

void close_board(Board *board) {
    if (board != NULL) {
        endwin();
        free(board->internal);
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
    BoardInfo *board_info = board->internal;
    MEVENT event;
    int ch = wgetch(board_info->main_window);
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

void print_user_prompt(WINDOW *w, int y, Tile prompt) {
    mvwprintw(w, y, 0, "|");
    mvwprintw(w, y, 1, " %c ", prompt);
    mvwprintw(w, y, 4, "|                  |");
    mvwprintw(w, y+1, 0, "+---+------------------+");
}

void prompt_usernames(Board *board, char player1_name[MAX_PLAYERNAME_SIZE + 1], char player2_name[MAX_PLAYERNAME_SIZE + 1]) {
    WINDOW *prompt_window = newwin(5, 24, 3, 3);
    curs_set(true);
    echo();
    mvwprintw(prompt_window, 0, 0, "+---+------------------+");
    print_user_prompt(prompt_window, 1, WHITE);
    print_user_prompt(prompt_window, 3, BLACK);
    mvwgetnstr(prompt_window, 1, 6, player1_name, 16);
    mvwgetnstr(prompt_window, 3, 6, player2_name, 16);
    wrefresh(prompt_window);
    delwin(prompt_window);
    curs_set(true);
    noecho();
    repaint_board(board);
}

void init_scoreboard(Board *board, char player1_name[MAX_PLAYERNAME_SIZE + 1], char player2_name[MAX_PLAYERNAME_SIZE + 1]) {

}

void update_scoreboard(Board *board, int player1_score, int player2_score, Tile turn) {

}
