#ifndef MLSTORE_H
#define MLSTORE_H

#include"mltypes.h"
#include<stdio.h>
#include<stdlib.h>

NeuralNetwork* create_network(const int size, const int layer_sizes[]);

void init_network(NeuralNetwork *network);

void free_network(NeuralNetwork *network);

void save_network(NeuralNetwork *network, const char *filename);

NeuralNetwork* load_network(const char* filename);

#endif