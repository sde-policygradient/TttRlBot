#include"tttutils.h"
#include<stdio.h>
#include<stdbool.h>

void clear_board(char board[3][3]){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            board[i][j] = ' ';
        }
    }
}

void draw_board(char board[3][3]){
    for(int i = 0; i < 7; i++){
        if(!(i % 2)){
            printf("-------\n");
            continue;
        }

        int index = (i - 1) * 0.5f;

        printf("|%c|%c|%c|\n", board[index][0], board[index][1], board[index][2]);
    }
}

bool play_move(char board[3][3], int row, int column, char symbol){
    char *target_sqare = &board[row][column];

    if(*target_sqare != ' ')return false;

    *target_sqare = symbol;
    return true;
}

static char check_win(char board[3][3]){
    char symbol;
    int symbol_count = 0;

    for(int i = 0; i < 3; i++){
        symbol = board[i][0];

        if(symbol == ' ')continue;

        for(int j = 1; j < 3; j++){
            if(board[i][j] != symbol){
                symbol_count = 0;
                break;
            }

            symbol_count++;
        }

        if(symbol_count >= 2)goto win;
    }

    for(int i = 0; i < 3; i++){
        symbol = board[0][i];

        if(symbol == ' ')continue;

        for(int j = 1; j < 3; j++){
            if(board[j][i] != symbol){
                symbol_count = 0;
                break;
            }

            symbol_count++;
        }

        if(symbol_count >= 2)goto win;
    }

    symbol = board[0][0];

    for(int i = 1; i < 3; i++){
        if(board[i][i] != symbol || symbol == ' '){
            symbol_count = 0;
            break;
        }

        symbol_count++;
    }

    if(symbol_count >= 2)goto win;

    symbol = board[2][0];

    for(int i = 1; i < 3; i++){
        if(board[2 - i][i] != symbol || symbol == ' '){
            symbol_count = 0;
            break;
        }

        symbol_count++;
    }

    if(symbol_count >= 2)goto win;

    return ' ';

    win:
        return symbol;
}

static bool check_draw(char board[3][3]){
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
    char win = check_win(board);

    if(win != ' ')return win;

    if(check_draw(board))return '=';

    return ' ';
}