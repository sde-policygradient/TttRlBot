#include<string.h>
#include<limits.h>
#include<time.h>
#include<errno.h>
#include<omp.h>
#include"tttutils.h"
#include"rltrain.h"
#include"filecheck.h"
#include"filecreate.h"

#define MAX_LENGTH 64
#define DEFAULT_SIZE 6
#define MAX_MOVE_INPUT 1024
#define LEARN_RATE 0.01

#if defined(_WIN32) || defined(_WIN64)

#define CLEAR_COMMAND "cls"
#define SLEEP(x) Sleep(x)

#else

#define CLEAR_COMMAND "clear"
#define SLEEP(x) usleep(x * 1000)

#endif

const int default_layer_sizes[] = {9, 12, 16, 16, 12, 9};

float symbol_to_value(const char symbol){
    const char symbols_to_check[] = {'X', 'O', ' '};
    
    for(int i = 0; i < sizeof(symbols_to_check); i++){
        if(symbol != symbols_to_check[i])continue;

        return i;
    }

    return -1;
}

void symbols_to_values(const char board[3][3], double out[9]){
    for(int i = 0; i < 9; i++){
        out[i] = symbol_to_value(board[i / 3][i % 3]) * 0.5;
    }
}

int log_to_file(const char *path, const char *message, int *error_code){
    FILE *log_file;

    CHECKED_FOPEN(log_file, path, "a", error_code);

    fprintf(log_file, "%s\n", message);

    CHECKED_FCLOSE(log_file, error_code);

    return 0;
}

int check_options(const int argc, char **argv, char **options_to_check, char **contains_options, int num_options_to_check){
    int prefix_lengths[num_options_to_check];

    for(int i = 0; i < num_options_to_check; i++){
        prefix_lengths[i] = 0;

        while(prefix_lengths[i] < strlen(options_to_check[i])){
            if(options_to_check[i][prefix_lengths[i]] == '$')break;

            prefix_lengths[i]++;
        }

        contains_options[i] = NULL;

        for(int j = 1; j < argc; j++){
            if(strncmp(argv[j], options_to_check[i], prefix_lengths[i]) != 0)continue;

            char value_str[MAX_LENGTH];
            int value_length;

            if(prefix_lengths[i] == strlen(options_to_check[i])){
                snprintf(value_str, MAX_LENGTH, "%d", INT_MIN);
            }else{
                snprintf(value_str, MAX_LENGTH, "%s", argv[j] + prefix_lengths[i]);
            }

            value_length = strlen(value_str);
            contains_options[i] = (char*)malloc((value_length + 1) * sizeof(char));

            if(!contains_options[i])continue;

            strcpy(contains_options[i], value_str);
        }
    }

    return 0;
}

void free_array_elements(void **array, int array_size){
    for(int i = 0; i < array_size; i++){
        if(array[i] == NULL)continue;

        free(array[i]);
    }
}

int main(int argc, char **argv){
    srand(time(NULL));

    char *options_to_check[] = {"-s$", "-i$", "-d$", "--draw"};
    char *contains_options[sizeof(options_to_check) / sizeof(options_to_check[0])];
    char *end_ptr;

    int return_value = 0;

    char board[3][3];
    char *save_dir;
    bool draw = false;
    NeuralNetwork *network;
    char save[MAX_LENGTH];
    char log[MAX_LENGTH];
    char evaluation;
    unsigned long iterations;
    unsigned long delay;
    
    {
        check_options(argc, argv, options_to_check, contains_options, sizeof(options_to_check) / sizeof(options_to_check[0]));

        if((!contains_options[0] || !contains_options[1]) || (strlen(contains_options[0]) == 0 || strlen(contains_options[1]) == 0)){
            printf("<name> <-s<save>> <-i<iterations>> <-d<delay>> [--draw]\n");
            goto free_options_array;
        }

        iterations = strtol(contains_options[1], &end_ptr, 10);

        if(*end_ptr != '\0'){
            printf("Bad value for iterations: %c\n", *end_ptr);
            goto free_options_array;
        }

        if(contains_options[2] == NULL){
            delay = 100;
        }else
            delay = strtol(contains_options[2], &end_ptr, 10);

        if(*end_ptr != '\0'){
            printf("Bad value for delay: %c\n", *end_ptr);
            goto free_options_array;
        }

        CHECKED_MALLOC(save_dir, strlen(contains_options[0]) + 1, char, &return_value);

        strcpy(save_dir, contains_options[0]);

        if(contains_options[3]){
            draw = true;
        }
    }

    {
        strcpy(save, save_dir);
        strcpy(log, save_dir);

        strcat(save, "/save.bin");
        strcat(log, "/log.txt");
    }

    if(dir_exists(save_dir)){
        log_to_file(log, "Directory already exists", &return_value);
    }else if(!MKDIR(save_dir)){
        log_to_file(log, "Created directory", &return_value);
    }else{
        log_to_file(log, "Failed to create directory", &return_value);
        goto free_save_dir;
    }

    if(file_exists(save)){
        load_network(&network, save, &return_value);
        log_to_file(log, "Loaded network", &return_value);
    }else{
        create_network(&network, DEFAULT_SIZE, default_layer_sizes, &return_value);
        init_network(network);
        log_to_file(log, "Created network", &return_value);
    }

    int network_move;
    bool move_success;
    int row;
    int column;

    clear_board(board);

    for(int i = 0;; i++){
        if(draw){
            system(CLEAR_COMMAND);
            draw_board(board);
        }

        evaluation = evaluate(board);

        if(evaluation != ' ')break;

        SLEEP(delay);

        double board_state[9];
        move_success = false;

        symbols_to_values(board, board_state);

        for(int j = 1; !move_success; j++){
            if(j > MAX_MOVE_INPUT){
                log_to_file(log, "Network exceeded MAX_MOVE_INPUT", &return_value);
                goto free_save_dir;
            }

            network_move = select_random_output(calculate_network_output(network, board_state, network->size, swish, &return_value), 9);
            row = network_move % 3;
            column = network_move / 3;

            move_success = play_move(board, row, column, i % 2? 'O' : 'X');
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
            printf("???\n");
    }

    log_to_file(log, "Network played normally", &return_value);

    save_network(network, save, &return_value);
    free_network(network);

    log_to_file(log, "Saved network", &return_value);

    free_save_dir:
        free(save_dir);
    
    free_options_array:
        free_array_elements((void**)contains_options, sizeof(contains_options) / sizeof(contains_options[0]));

    return return_value;
}