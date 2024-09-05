#ifndef TTTUTILS_H
#define TTTUTILS_H

#include<stdio.h>
#include<stdbool.h>

void clear_board(char board[3][3]);
void draw_board(char board[3][3]);
bool play_move(char board[3][3], int row, int column, char symbol);
char evaluate(char board[3][3]);

#endif