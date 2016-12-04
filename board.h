#include <stdbool.h>

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8
#define MAX_PLAYERNAME_SIZE 16

typedef enum TileType {
    WHITE = '+',
    BLACK = '@',
    NONE = ' '
} Tile;

typedef struct BoardEventType {
    int x;
    int y;
} BoardEvent;

typedef struct BoardType {
    bool has_color;
    Tile tiles[BOARD_WIDTH][BOARD_HEIGHT];
    void *internal;
} Board;

void init_board(Board *board);
void update_tile(Board *board, int x, int y, Tile tile);
void repaint_board(Board *board);
void close_board(Board *board);
void prompt_usernames(Board *board, char player1_name[MAX_PLAYERNAME_SIZE + 1], char player2_name[MAX_PLAYERNAME_SIZE + 1]);
void init_scoreboard(Board *board, char player1_name[MAX_PLAYERNAME_SIZE + 1], char player2_name[MAX_PLAYERNAME_SIZE + 1]);
void update_scoreboard(Board *board, int player1_score, int player2_score, Tile turn);
bool wait_for_input(Board *board, int *x, int *y);
