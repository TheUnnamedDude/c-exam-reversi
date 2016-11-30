#include <stdlib.h>
#include "board.h"

int main(int argc, char **argv) {
    Board board;
    init_board(&board);
    //sleep(10);
    close_board(&board);
    return 0;
}
