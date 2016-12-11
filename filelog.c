#include <stdio.h>
#include "filelog.h"

void init_log(Log *log, char *file_name) {
    if (file_name == NULL) {
        log->log_file = NULL;
    } else {
        log->log_file = fopen(file_name, "w");
    }
}

void log_game_info(Log *log, char *player1_name, char *player2_name, int start_tiles, int width, int height) {
    if (log->log_file == NULL)
        return;
    fprintf(log->log_file, "StartGameInfo\n");
    fprintf(log->log_file, "    Boardsize: %dx%d\n", width, height);
    fprintf(log->log_file, "    Starttiles: %d\n", start_tiles);
    fprintf(log->log_file, "    Player1Name: %s\n", player1_name);
    fprintf(log->log_file, "    Player2Name: %s\n", player2_name);
    fprintf(log->log_file, "EndGameInfo\n");
}

void log_start_of_moves(Log *log) {
    if (log->log_file == NULL)
        return;
    fprintf(log->log_file, "StartGame\n");
}

void log_move(Log *log, int x, int y, Tile tile) {
    if (log->log_file == NULL)
        return;
    fprintf(log->log_file, "    UserClick %d,%d %c\n", x, y, tile == WHITE ? 'W' : 'B');
}

void log_settile(Log *log, int x, int y, Tile tile) {
    if (log->log_file == NULL)
        return;
    fprintf(log->log_file, "    SetTile %d,%d %c\n", x, y, tile == WHITE ? 'W' : 'B');
}

void log_end_of_moves(Log *log) {
    if (log->log_file == NULL)
        return;
    fprintf(log->log_file, "EndGame");
}

void log_end_game_statistics(Log *log, int player1_score, int player2_score, int total_moves) {
    if (log->log_file == NULL)
        return;
    fprintf(log->log_file, "StartStatistics\n");
    fprintf(log->log_file, "    Player1Score: %d\n", player1_score);
    fprintf(log->log_file, "    Player2Score: %d\n", player2_score);
    fprintf(log->log_file, "    TotalMoves: %d\n", total_moves);
    fprintf(log->log_file, "EndStatistics\n");
}

void close_log(Log *log) {
    if (log->log_file == NULL)
        return;
    fclose(log->log_file);
}
