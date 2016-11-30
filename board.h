#include <stdbool.h>

#define WIDTH 8
#define HEIGHT 8

typedef enum TileType {
    WHITE = '+',
    BLACK = '@',
    NONE = ' '
} Tile;

typedef struct BoardType {
    bool has_color;
    Tile tiles[WIDTH][HEIGHT];
} Board;

void init_board(Board *board);
void update_tile(Board *board, int x, int y, Tile tile);
void repaint_board(Board *board);
void close_board(Board *board);

extern int BOARD_WIDTH;
extern int BOARD_HEIGHT;
