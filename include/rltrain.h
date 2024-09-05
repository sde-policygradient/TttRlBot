#ifndef RLTRAIN_H
#define RLTRAIN_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"mlstore.h"

typedef double activation_t(double);

double ReLu(double in);

double sigmoid(double in);

double swish(double in);

double* calculate_layer_output(Layer *layer, const double inputs[], activation_t activation_func);

double* calculate_network_output(NeuralNetwork *network, const double inputs[], const int network_size, activation_t activation_func);

void backpropagate(NeuralNetwork *network, const double inputs[], const double outputs[], const double expected_outputs[], activation_t activation_func);

int select_random_output(const double numbers[], const int size);

double temp_diff_error(double reward, double discount_factor, double optimal_future_val, double current_val);

double mean_square_error(double values[], double expected_values[], int size);

#endif