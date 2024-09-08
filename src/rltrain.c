#include"rltrain.h"
#include"checkedfunc.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define NUDGE 1.0e-8

static double derivative(double (*function)(const double), const double in){
    double out = 1 / NUDGE;

    out *= function(in + NUDGE) - function(in);

    return out;
}

static double none(double in){
    return in;
}

double ReLu(double in){
    return fmax(in, 0.00);
}

double sigmoid(double in){
    return 1 / (1 + pow(2.718, -in));
}

double swish(double in){
    return in * sigmoid(in);
}

double* calculate_layer_output(Layer *layer, const double inputs[], activation_t activation_func, int *error_code){
    double *outputs;

    CHECKED_CALLOC(outputs, layer->outputs, double, error_code);

    for(int i = 0; i < layer->outputs; i++){
        for(int j = 0; j < layer->inputs; j++){
            outputs[i] += inputs[j] * layer->weights[i * layer->inputs + j];
        }

        outputs[i] = activation_func(outputs[i]);
    }

    return outputs;
}

double* calculate_network_output(NeuralNetwork *network, const double inputs[], const int network_size, activation_t activation_func, int *error_code){
    double *layer_outputs;

    CHECKED_CALLOC(layer_outputs, network->layers[0].inputs, double, error_code);

    for(int i = 0; i < network->layers[0].inputs; i++){
        layer_outputs[i] = inputs[i];
    }

    for(int i = 1; i < network->size; i++){
        CHECKED_REALLOC(layer_outputs, network->layers[i].outputs, double, error_code);
        
        layer_outputs = calculate_layer_output(&network->layers[i], layer_outputs, i?activation_func:none, error_code);
    }

    return layer_outputs;
}

void backpropagate(NeuralNetwork *network, const double inputs[], const double outputs[], const double expected_outputs[], activation_t activation_func){
    return;
}

int select_random_output(const double numbers[], const int size){
    double selector = (double)rand() / RAND_MAX;
    double divisor = 0;

    for(int i = 0; i < size; i++){
        divisor += numbers[i];
    }

    divisor = 1 / divisor;

    for(int i = 0; i < size; i++){
        selector -= numbers[i] * divisor;

        if(selector <= 0)return i;
    }

    return 0;
}

double temp_diff_error(double reward, double discount_factor, double optimal_future_val, double current_val){
    double temp_diff_target = reward + discount_factor * optimal_future_val;

    return temp_diff_target - current_val;
}

double mean_square_error(double values[], double expected_values[], int size){
    double square_error = 0;

    for(int i = 0; i < size; i++){
        square_error += (values[i] - expected_values[i]);
        square_error *= square_error;
    }

    return square_error / size;
}