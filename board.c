#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
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
    char player1_name[MAX_PLAYERNAME_SIZE + 1];
    char player2_name[MAX_PLAYERNAME_SIZE + 1];
} BoardInfo;

void color_char(Board *board, int x, int y, char c, int color, WINDOW *window) {
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
    curs_set(false);
    noecho();
    BoardInfo *board_info = malloc(sizeof(BoardInfo));
    board_info->main_window = newwin(Y_END + 1, X_END + 1, 0, 0);
    board->internal = board_info;
    board->has_color = true;
    if (board->has_color) {
        start_color();
        init_pair(WHITE_TILE_COLOR, COLOR_WHITE, COLOR_WHITE);
        init_pair(BLACK_TILE_COLOR, COLOR_BLACK, COLOR_BLACK);
        init_pair(NONE_TILE_COLOR, COLOR_BLUE, COLOR_BLUE);
        init_pair(BORDER_COLOR, COLOR_WHITE, COLOR_BLUE);
    }
    keypad(board_info->main_window, true);
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

void print_tile(Board *board, int x, int y, Tile tile, WINDOW *window) {
    int color = tile == WHITE ? WHITE_TILE_COLOR : tile == BLACK ? BLACK_TILE_COLOR : NONE_TILE_COLOR;
    color_char(board, x-1, y, ' ', color, window);
    color_char(board, x, y, tile, color, window);
    color_char(board, x+1, y, ' ', color, window);
}

void set_tile(Board *board, int x, int y, Tile tile) {
    BoardInfo *board_info = board->internal;
    print_tile(board, 2 + x * 4, 1 + y * 2, tile, board_info->main_window);
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
                color_char(board, x, y, '+', BORDER_COLOR, board_info->main_window);
            } else if (x % 4 == 0) {
                color_char(board, x, y, '|', BORDER_COLOR, board_info->main_window);
            } else if (y % 2 == 0) {
                color_char(board, x, y, '-', BORDER_COLOR, board_info->main_window);
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

void print_user_prompt(Board *board, WINDOW *w, int y, Tile prompt) {
    mvwprintw(w, y, 0, "|");
    print_tile(board, 2, y, prompt, w);
    mvwprintw(w, y, 4, "|                  |");
    mvwprintw(w, y+1, 0, "+---+------------------+");
}

void prompt_usernames(Board *board, char player1_name[MAX_PLAYERNAME_SIZE + 1], char player2_name[MAX_PLAYERNAME_SIZE + 1]) {
    WINDOW *prompt_window = newwin(7, 24, 3, 3);
    curs_set(true);
    echo();
    mvwprintw(prompt_window, 0, 0, "+----------------------+");
    mvwprintw(prompt_window, 1, 0, "|Please enter usernames|");
    mvwprintw(prompt_window, 2, 0, "+---+------------------+");
    print_user_prompt(board, prompt_window, 3, WHITE);
    print_user_prompt(board, prompt_window, 5, BLACK);
    mvwgetnstr(prompt_window, 3, 6, player1_name, 16);
    mvwgetnstr(prompt_window, 5, 6, player2_name, 16);
    wrefresh(prompt_window);
    delwin(prompt_window);
    curs_set(false);
    noecho();
    repaint_board(board);
}

void init_scoreboard(Board *board, char player1_name[MAX_PLAYERNAME_SIZE + 1], char player2_name[MAX_PLAYERNAME_SIZE + 1]) {
    BoardInfo *board_info = board->internal;
    memcpy(board_info->player1_name, player1_name, sizeof(board_info->player1_name));
    memcpy(board_info->player2_name, player2_name, sizeof(board_info->player2_name));
    board_info->scoreboard_window = newwin(5, 15 + MAX_PLAYERNAME_SIZE, 0, X_END + 1 + 5);
}

void update_scoreboard(Board *board, int player1_score, int player2_score, Tile turn) {
    BoardInfo *board_info = board->internal;
    WINDOW *scoreboard_window = board_info->scoreboard_window;
    char username_dashes[MAX_PLAYERNAME_SIZE + 1];
    memset(username_dashes, '-', sizeof(username_dashes));
    username_dashes[MAX_PLAYERNAME_SIZE] = '\0';
    char username_spaces[MAX_PLAYERNAME_SIZE + 1];
    memset(username_spaces, ' ', sizeof(username_spaces));
    username_spaces[MAX_PLAYERNAME_SIZE] = '\0';
    mvwprintw(scoreboard_window, 0, 0, "+---+----+-%s---+", username_dashes);
    mvwprintw(scoreboard_window, 1, 0, "|   | %-2d | %s %c |", player1_score, username_spaces, turn == WHITE ? '<' : ' ');
    mvwprintw(scoreboard_window, 2, 0, "+---+----+-%s---+", username_dashes);
    mvwprintw(scoreboard_window, 3, 0, "|   | %-2d | %s %c |", player2_score, username_spaces, turn == BLACK ? '<' : ' ');
    mvwprintw(scoreboard_window, 4, 0, "+---+----+-%s---+", username_dashes);
    mvwprintw(scoreboard_window, 1, 11, board_info->player1_name);
    mvwprintw(scoreboard_window, 3, 11, board_info->player2_name);
    print_tile(board, 2, 1, WHITE, scoreboard_window);
    print_tile(board, 2, 3, BLACK, scoreboard_window);
    wrefresh(scoreboard_window);
}
