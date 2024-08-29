#include<string.h>
#include<time.h>
#include<errno.h>
#include<conio.h>
#include"tttutils.h"
#include"rlutils.h"
#include"filebase.h"

#define MAX_NAME_LENGTH 64
#define DEFAULT_SIZE 6
#define MAX_MOVE_INPUT 1024
#define LEARN_RATE 0.01

#if defined(_WIN32) || defined(_WIN64)
    #include<windows.h>
    #include<direct.h>

    #define CLEAR_COMMAND "cls"
    #define SLEEP(x) Sleep(x)
#else
    #include<unistd.h>
    #include<sys/stat.h>

    #define CLEAR_COMMAND "clear"
    #define SLEEP(X) sleep(x)
#endif

const int default_layer_sizes[] = {9, 12, 16, 16, 12, 9};

void symbolsToValues(const char board[3][3], double out[9]){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            switch(board[i][j]){
                case ' ':
                    out[i * 3 + j] = 1.0;
                    break;
                case 'X':
                    out[i * 3 + j] = 0.0;
                    break;
                case 'O':
                    out[i * 3 + j] = 0.5;
                    break;
            }
        }
    }
}

void logToFile(const char *path, const char *message){
    FILE *logFile = fopen(path, "a");

    fprintf(logFile, "%s\n", message);

    fclose(logFile);
}

int main(int argc, char **argv){
    srand(time(NULL));

    if(argc < 3){
        printf("main [dir] [iterations] (--draw)");
        return 0;
    }

    int saveDirLength = strlen(argv[1]);

    char board[3][3];
    double boardState[9];
    char saveDir[MAX_NAME_LENGTH];
    bool draw = false;
    NeuralNetwork *network;
    char save[MAX_NAME_LENGTH];
    char log[MAX_NAME_LENGTH];
    char evaluation;
    const unsigned long iterations = strtoul(argv[2], NULL, 10);
    bool moveSuccess;
    int networkMove;
    int row;
    int column;

    strcpy(saveDir, argv[1]);
    strcpy(save, argv[1]);
    strcpy(log, argv[1]);

    strcat(save, "\\save.bin");
    strcat(log, "\\log.txt");

    if(argc >= 4){
        if(strcmp(argv[3], "--draw")){
            perror("Invalid options");
            return 1;
        }

        draw = true;
    }

    if(dirExists(saveDir)){
        logToFile(log, "Directory already exists");
    }else if(!MKDIR(saveDir)){
        logToFile(log, "Created directory");
    }else{
        logToFile(log, "Failed to create directory");
        goto Failure;
    }

    if(fileExists(save)){
        network = loadNetwork(save);
        logToFile(log, "Loaded network");
    }else{
        network = createNetwork(DEFAULT_SIZE, default_layer_sizes);
        initNetwork(network);
        logToFile(log, "Created network");
    }

    clearBoard(board);

    for(int i = 0;; i++){
        if(draw){
            system(CLEAR_COMMAND);
            drawBoard(board);
        }

        evaluation = evaluate(board);
        moveSuccess = false;

        if(evaluation != ' ')break;

        SLEEP(100);

        for(int j = 1; !moveSuccess; j++){
            if(j > MAX_MOVE_INPUT){
                logToFile(log, "Network exceeded MAX_MOVE_INPUT");
                goto Failure;
            }

            symbolsToValues(board, boardState);

            networkMove = selectRandomOutput(calculateNetworkOutput(network, boardState, network->size, swish), 9);
            row = networkMove % 3;
            column = networkMove / 3;

            moveSuccess = playMove(board, row, column, i % 2?'O':'X');
        };
    }

    switch(evaluation){
        case 'X':
        case 'O':
            printf("%c wins!\n", evaluation);
            break;
        case '=':
            printf("Game drawn!\n");
            break;
        default:
            logToFile(log, "???");
    }

    logToFile(log, "Network played normally");

    saveNetwork(network, save);
    freeNetwork(network);

    logToFile(log, "Saved network");

    return 0;
    
    Failure:
        return 1;
}