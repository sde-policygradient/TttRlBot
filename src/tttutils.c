#include"tttutils.h"
#include<stdio.h>
#include<stdbool.h>

void clearBoard(char board[3][3]){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            board[i][j] = ' ';
        }
    }
}

void drawBoard(char board[3][3]){
    for(int i = 0; i < 7; i++){
        if(!(i % 2)){
            printf("-------\n");
            continue;
        }

        int index = (i - 1) * 0.5f;

        printf("|%c|%c|%c|\n", board[index][0], board[index][1], board[index][2]);
    }
}

bool playMove(char board[3][3], int row, int column, char symbol){
    char *targetSqare = &board[row][column];

    if(*targetSqare != ' ')return false;

    *targetSqare = symbol;
    return true;
}

char checkWin(char board[3][3]){
    char symbol;
    int symbolCount = 0;

    for(int i = 0; i < 3; i++){
        symbol = board[i][0];

        if(symbol == ' ')continue;

        for(int j = 1; j < 3; j++){
            if(board[i][j] != symbol){
                symbolCount = 0;
                break;
            }

            symbolCount++;
        }

        if(symbolCount >= 2)goto win;
    }

    for(int i = 0; i < 3; i++){
        symbol = board[0][i];

        if(symbol == ' ')continue;

        for(int j = 1; j < 3; j++){
            if(board[j][i] != symbol){
                symbolCount = 0;
                break;
            }

            symbolCount++;
        }

        if(symbolCount >= 2)goto win;
    }

    symbol = board[0][0];

    for(int i = 1; i < 3; i++){
        if(board[i][i] != symbol || symbol == ' '){
            symbolCount = 0;
            break;
        }

        symbolCount++;
    }

    if(symbolCount >= 2)goto win;

    symbol = board[2][0];

    for(int i = 1; i < 3; i++){
        if(board[2 - i][i] != symbol || symbol == ' '){
            symbolCount = 0;
            break;
        }

        symbolCount++;
    }

    if(symbolCount >= 2)goto win;

    return ' ';

    win:
        return symbol;
}

bool checkDraw(char board[3][3]){
    bool draw = true;

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board[i][j] == ' '){
                draw = false;
                break;
            }
        }
    }

    return draw;
}

char evaluate(char board[3][3]){
    char win = checkWin(board);

    if(win != ' ')return win;

    if(checkDraw(board))return '=';

    return ' ';
}