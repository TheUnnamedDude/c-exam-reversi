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

void color_on(Board *board, WINDOW *window, int color) {
    if (board->has_color) {
        wattron(window, COLOR_PAIR(color));
    }
}

void color_off(Board *board, WINDOW *window, int color) {
    if (board->has_color) {
        wattroff(window, COLOR_PAIR(color));
    }
}

void init_board(Board *board) {
    initscr();
    curs_set(false);
    noecho();
    board->main_window = newwin(Y_END + 1, X_END + 1, 0, 0);
    board->has_color = true;

    if (board->has_color) {
        start_color();
        init_pair(WHITE_TILE_COLOR, COLOR_WHITE, COLOR_WHITE);
        init_pair(BLACK_TILE_COLOR, COLOR_BLACK, COLOR_BLACK);
        init_pair(NONE_TILE_COLOR, COLOR_BLUE, COLOR_BLUE);
        init_pair(BORDER_COLOR, COLOR_WHITE, COLOR_BLUE);
    }

    keypad(board->main_window, true);
    mousemask(BUTTON1_CLICKED, NULL);

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_WIDTH; y++) {
            board->tiles[x][y] = NONE;
        }
    }

    repaint_board(board);
}

void print_tile(Board *board, int x, int y, Tile tile, WINDOW *window) {
    int color = tile == WHITE ? WHITE_TILE_COLOR : tile == BLACK ? BLACK_TILE_COLOR : NONE_TILE_COLOR;
    color_on(board, window, color);
    mvwprintw(window, y, x-1, " %c ", tile);
    color_off(board, window, color);
}

void set_tile(Board *board, int x, int y, Tile tile) {
    print_tile(board, 2 + x * 4, 1 + y * 2, tile, board->main_window);
    board->tiles[x][y] = tile;
    update_player_score(board);
}

void update_tile(Board *board, int x, int y, Tile tile) {
    set_tile(board, x, y, tile);
    wrefresh(board->main_window);
}

void update_player_score(Board *board) {
    int player1_score = 0;
    int player2_score = 0;
    int tiles_left = 0;

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            if (board->tiles[x][y] == WHITE) {
                player1_score++;
            } else if (board->tiles[x][y] == BLACK) {
                player2_score++;
            } else {
                tiles_left++;
            }
        }
    }

    board->player1_score = player1_score;
    board->player2_score = player2_score;
    board->tiles_left = tiles_left;
}

void repaint_board(Board *board) {
    color_on(board, board->main_window, BORDER_COLOR);

    box(board->main_window, 0, 0);

    for (int x = 1; x < X_END; x++) {
        for (int y = 1; y < Y_END; y++) {
            if (x % 4 == 0 && y % 2 == 0) {
                mvwaddch(board->main_window, y, x, ACS_PLUS);
            } else if (x % 4 == 0) {
                mvwaddch(board->main_window, y, x, ACS_VLINE);
            } else if (y % 2 == 0) {
                mvwaddch(board->main_window, y, x, ACS_HLINE);
            }
        }
    }

    for (int x = 4; x < X_END; x+=4) {
        mvwaddch(board->main_window, 0, x, ACS_TTEE);
        mvwaddch(board->main_window, Y_END, x, ACS_BTEE);
    }

    for (int y = 2; y < Y_END; y+=2) {
        mvwaddch(board->main_window, y, 0, ACS_LTEE);
        mvwaddch(board->main_window, y, X_END, ACS_RTEE);
    }

    color_off(board, board->main_window, BORDER_COLOR);

    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            set_tile(board, x, y, board->tiles[x][y]);
        }
    }

    wrefresh(board->main_window);
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
    int ch = wgetch(board->main_window);

    if (ch == KEY_MOUSE && x != NULL && y != NULL) {
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
    int max_x = getmaxx(w);
    mvwhline(w, y - 1, 1, 0, max_x - 2);
    mvwaddch(w, y - 1, 0, ACS_LTEE);
    mvwaddch(w, y - 1, max_x - 1, ACS_RTEE);
    mvwaddch(w, y, 4, ACS_VLINE);
    print_tile(board, 2, y, prompt, w);
}

void prompt_usernames(Board *board) {
    WINDOW *prompt_window = newwin(7, 24, 3, 3);
    curs_set(true);
    echo();

    box(prompt_window, 0, 0);
    mvwprintw(prompt_window, 1, 1, "Please enter usernames");
    print_user_prompt(board, prompt_window, 3, WHITE);
    print_user_prompt(board, prompt_window, 5, BLACK);

    mvwaddch(prompt_window, 2, 4, ACS_TTEE);
    mvwaddch(prompt_window, 4, 4, ACS_PLUS);
    mvwaddch(prompt_window, 6, 4, ACS_BTEE);

    mvwgetnstr(prompt_window, 3, 6, board->player1_name, MAX_PLAYERNAME_SIZE);
    mvwgetnstr(prompt_window, 5, 6, board->player2_name, MAX_PLAYERNAME_SIZE);

    wrefresh(prompt_window);
    delwin(prompt_window);
    curs_set(false);
    noecho();

    repaint_board(board);
}

void init_scoreboard(Board *board) {
    board->scoreboard_window = newwin(5, 15 + MAX_PLAYERNAME_SIZE, 0, X_END + 1 + 5);
}

void update_scoreboard(Board *board, Tile turn) {
    WINDOW *scoreboard_window = board->scoreboard_window;
    int max_x = getmaxx(scoreboard_window);

    mvwprintw(scoreboard_window, 1, 11, board->player1_name);
    mvwprintw(scoreboard_window, 3, 11, board->player2_name);

    print_tile(board, 2, 1, WHITE, scoreboard_window);
    mvwprintw(scoreboard_window, 1, 6, "%-2d", board->player1_score);
    mvwaddch(scoreboard_window, 1, 12 + MAX_PLAYERNAME_SIZE, turn == WHITE ? '<' : ' ');
    print_tile(board, 2, 3, BLACK, scoreboard_window);
    mvwprintw(scoreboard_window, 3, 6, "%-2d", board->player2_score);
    mvwaddch(scoreboard_window, 3, 12 + MAX_PLAYERNAME_SIZE, turn == BLACK ? '<' : ' ');

    box(scoreboard_window, 0, 0);


    // Add TTEE over player 1 to avoid gaps
    mvwaddch(scoreboard_window, 0, 4, ACS_TTEE);
    mvwaddch(scoreboard_window, 0, 9, ACS_TTEE);
    // Add chars between columns for player 1
    mvwaddch(scoreboard_window, 1, 4, ACS_VLINE);
    mvwaddch(scoreboard_window, 1, 9, ACS_VLINE);

    // Add a line between players and a plus to avoid gaps
    mvwaddch(scoreboard_window, 2, 0, ACS_LTEE);
    mvwhline(scoreboard_window, 2, 1, 0, max_x - 2);
    mvwaddch(scoreboard_window, 2, 4, ACS_PLUS);
    mvwaddch(scoreboard_window, 2, 9, ACS_PLUS);
    mvwaddch(scoreboard_window, 2, max_x - 1, ACS_RTEE);

    // Add chars between columns for player 2
    mvwaddch(scoreboard_window, 3, 4, ACS_VLINE);
    mvwaddch(scoreboard_window, 3, 9, ACS_VLINE);

    // Add BTEE under player 2 to avoid gaps
    mvwaddch(scoreboard_window, 4, 4, ACS_BTEE);
    mvwaddch(scoreboard_window, 4, 9, ACS_BTEE);

    wrefresh(scoreboard_window);
}

void close_scoreboard(Board *board) {
    if (board->scoreboard_window != NULL) {
        wclear(board->scoreboard_window);
        wrefresh(board->scoreboard_window);
        delwin(board->scoreboard_window);
    }
}

void show_game_over_screen(Board *board)  {
    int width = 27 + MAX_PLAYERNAME_SIZE;
    WINDOW *go_screen = newwin(7, width, 5, 2);

    char *winner_name = NULL;
    char *loser_name = NULL;
    int winner_score = 0;
    int loser_score = 0;

    // Ugly if else to set the variables for the winner and loser info
    if (board->player1_score > board->player2_score) {
        winner_name = &board->player1_name[0];
        loser_name = &board->player2_name[0];
        winner_score = board->player1_score;
        loser_score = board->player2_score;
    } else {
        winner_name = &board->player2_name[0];
        loser_name = &board->player1_name[0];
        winner_score = board->player2_score;
        loser_score = board->player1_score;
    }
    box(go_screen, 0, 0);
    
    // Nothing should ever print over the winner and loser names anyway
    mvwprintw(go_screen, 1, 12, winner_name);
    mvwprintw(go_screen, 1, 14 + MAX_PLAYERNAME_SIZE, "%3d points", winner_score);
    mvwprintw(go_screen, 3, 12, loser_name);
    mvwprintw(go_screen, 3, 14 + MAX_PLAYERNAME_SIZE, "%3d points", loser_score);

    // Add TTEE's to the top for no gaps
    mvwaddch(go_screen, 0, 10, ACS_TTEE);
    mvwaddch(go_screen, 0, 12 + MAX_PLAYERNAME_SIZE, ACS_TTEE);

    // Print winner collumn
    mvwprintw(go_screen, 1, 2, "Winner");
    mvwaddch(go_screen, 1, 10, ACS_VLINE);
    mvwaddch(go_screen, 1, 12 + MAX_PLAYERNAME_SIZE, ACS_VLINE);

    // Print line between winner and loser + add chars for no gaps
    mvwaddch(go_screen, 2, 0, ACS_LTEE);
    mvwhline(go_screen, 2, 1, 0, width - 2);
    mvwaddch(go_screen, 2, 10, ACS_PLUS);
    mvwaddch(go_screen, 2, 12 + MAX_PLAYERNAME_SIZE, ACS_PLUS);
    mvwaddch(go_screen, 2, width - 1, ACS_RTEE);

    // Print the loser collumn + add chars for no gaps
    mvwprintw(go_screen, 3, 2, "Loser");
    mvwaddch(go_screen, 3, 10, ACS_VLINE);
    mvwaddch(go_screen, 3, 12 + MAX_PLAYERNAME_SIZE, ACS_VLINE);

    // Print the line between loser and info box + add chars for no gaps
    mvwaddch(go_screen, 4, 0, ACS_LTEE);
    mvwhline(go_screen, 4, 1, 0, width - 2);
    mvwaddch(go_screen, 4, 10, ACS_BTEE);
    mvwaddch(go_screen, 4, 12 + MAX_PLAYERNAME_SIZE, ACS_BTEE);
    mvwaddch(go_screen, 4, width - 1, ACS_RTEE);

    // Print the content of the info box
    mvwprintw(go_screen, 5, 1, "Press q to quit");

    wrefresh(go_screen);
    while (wait_for_input(board, NULL, NULL)); // Wait for the  user to press the exit button
}
