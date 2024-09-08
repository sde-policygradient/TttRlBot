#ifndef MLSTORE_H
#define MLSTORE_H

#include"mltypes.h"
#include"checkedfunc.h"
#include<stdio.h>
#include<stdlib.h>

void create_network(NeuralNetwork **network, const int size, const int layer_sizes[], int *error_code);

void init_network(NeuralNetwork *network);

void free_network(NeuralNetwork *network);

void save_network(const NeuralNetwork *network, const char *filename, int *error_code);

void load_network(NeuralNetwork **network, const char* filename, int *error_code);

#endif