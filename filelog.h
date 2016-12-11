#include <stdio.h>
#include "board.h"

typedef struct LogType {
    FILE *log_file;
} Log;

void init_log(Log *log, char *file_name);
void log_game_info(Log *log, char *player1_name, char *player2_name, int start_tiles, int width, int height);
void log_start_of_moves(Log *log);
void log_move(Log *log, int x, int y, Tile tile);
void log_settile(Log *log, int x, int y, Tile tile);
void log_end_of_moves(Log *log);
void log_end_game_statistics(Log *log, int player1_score, int player2_score, int total_moves);
void close_log(Log *log);
