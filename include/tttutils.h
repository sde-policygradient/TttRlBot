#ifndef TTTUTILS_H
#define TTTUTILS_H

#include<stdio.h>
#include<stdbool.h>

void clearBoard(char board[3][3]);
void drawBoard(char board[3][3]);
bool playMove(char board[3][3], int row, int column, char symbol);
char evaluate(char board[3][3]);

#endif