#include <stdbool.h>

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8

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
} Board;

void init_board(Board *board);
void update_tile(Board *board, int x, int y, Tile tile);
void repaint_board(Board *board);
void close_board(Board *board);
bool wait_for_input(int *x, int *y);
