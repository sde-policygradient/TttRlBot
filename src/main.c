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

void symbols_to_values(const char board[3][3], double out[9]){
    #pragma omp parallel for
    
    for(int i = 0; i < 9; i++){
        switch(board[i / 3][i % 3]){
            case ' ':
                out[i] = 1.0;
                break;
            case 'X':
                out[i] = 0.0;
                break;
            case 'O':
                out[i] = 0.5;
                break;
        }
    }
}

int log_to_file(const char *path, const char *message){
    FILE *log_file = fopen(path, "a");

    if(!log_file)return 1;

    fprintf(log_file, "%s\n", message);

    fclose(log_file);

    return 0;
}

int check_options(const int argc, char **argv, char **options_to_check, char **contains_options, int num_options_to_check){
    int prefix_lengths[num_options_to_check];

    #pragma omp parallel for

    for(int i = 0; i < num_options_to_check; i++){
        prefix_lengths[i] = 0;

        while(prefix_lengths[i] < strlen(options_to_check[i])){
            if(options_to_check[i][prefix_lengths[i]] == '$')break;

            prefix_lengths[i]++;
        }

        contains_options[i] = NULL;

        #pragma omp parallel for

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
    double board_state[9];
    char *save_dir;
    bool draw = false;
    NeuralNetwork *network;
    char save[MAX_LENGTH];
    char log[MAX_LENGTH];
    char evaluation;
    unsigned long iterations;
    unsigned long delay;
    bool move_success;
    int network_move;
    int row;
    int column;

    {
        check_options(argc, argv, options_to_check, contains_options, sizeof(options_to_check) / sizeof(options_to_check[0]));

        if(!contains_options[0] || !contains_options[1]){
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

        save_dir = (char*)malloc((strlen(contains_options[0]) + 1) * sizeof(char));
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
        log_to_file(log, "Directory already exists");
    }else if(!MKDIR(save_dir)){
        log_to_file(log, "Created directory");
    }else{
        log_to_file(log, "Failed to create directory");
        goto free_save_dir;
    }

    if(file_exists(save)){
        network = load_network(save);
        log_to_file(log, "Loaded network");
    }else{
        network = create_network(DEFAULT_SIZE, default_layer_sizes);
        init_network(network);
        log_to_file(log, "Created network");
    }

    clear_board(board);

    for(int i = 0;; i++){
        if(draw){
            system(CLEAR_COMMAND);
            draw_board(board);
        }

        evaluation = evaluate(board);
        move_success = false;

        if(evaluation != ' ')break;

        SLEEP(delay);

        for(int j = 1; !move_success; j++){
            if(j > MAX_MOVE_INPUT){
                log_to_file(log, "Network exceeded MAX_MOVE_INPUT");
                goto free_save_dir;
            }

            symbols_to_values(board, board_state);

            network_move = select_random_output(calculate_network_output(network, board_state, network->size, swish), 9);
            row = network_move % 3;
            column = network_move / 3;

            move_success = play_move(board, row, column, i % 2?'O':'X');
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
            log_to_file(log, "???");
    }

    log_to_file(log, "Network played normally");

    save_network(network, save);
    free_network(network);

    log_to_file(log, "Saved network");

    free_save_dir:
        free(save_dir);
    
    free_options_array:
        free_array_elements((void**)contains_options, sizeof(contains_options) / sizeof(contains_options[0]));

    return return_value;
}