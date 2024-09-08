#include"mlstore.h"
#include"mltypes.h"
#include"checkedfunc.h"
#include<stdio.h>
#include<stdlib.h>

void create_network(NeuralNetwork **network, const int size, const int layer_sizes[], int *error_code){
    CHECKED_MALLOC(*network, 1, NeuralNetwork, error_code);

    (*network)->size = size;

    CHECKED_CALLOC((*network)->layers, size, Layer, error_code);

    for(int i = 0; i < size; i++){
        Layer *current_layer = &(*network)->layers[i];

        {
            current_layer->inputs = !i?layer_sizes[i]:layer_sizes[i - 1];
            current_layer->outputs = layer_sizes[i];
        }

        {
            CHECKED_CALLOC(current_layer->weights, current_layer->inputs * current_layer->outputs, double, error_code);
            CHECKED_CALLOC(current_layer->biases, current_layer->outputs, double, error_code);
        }
    }
}

void init_network(NeuralNetwork *network){
    for(int i = 0; i < network->size; i++){
        Layer *current_layer = &network->layers[i];

        for(int j = 0; j < current_layer->inputs * current_layer->outputs; j++){
            current_layer->weights[j] = i?(double)rand() / RAND_MAX:1;
        }

        for(int j = 0; j < current_layer->outputs; j++){
            current_layer->biases[j] = i?(double)rand() / RAND_MAX:0;
        }
    }
}

void free_network(NeuralNetwork *network){
    if(!network)return;

    for(int i = 0; i < network->size; i++){
        Layer *current_layer = &network->layers[i];

        free(current_layer->weights);
        free(current_layer->biases);
    }

    free(network->layers);
    free(network);
}

static void save_layer(const Layer layer, FILE *file, int *error_code){
    CHECKED_FWRITE(&layer.inputs, sizeof(int), 1, file, error_code);
    CHECKED_FWRITE(&layer.outputs, sizeof(int), 1, file, error_code);
    CHECKED_FWRITE(layer.weights, sizeof(double), layer.inputs * layer.outputs, file, error_code);
    CHECKED_FWRITE(layer.biases, sizeof(double), layer.outputs, file, error_code);
}

void save_network(const NeuralNetwork *network, const char *filename, int *error_code){
    FILE *file;

    CHECKED_FOPEN(file, filename, "wb", error_code);

    CHECKED_FWRITE(&network->size, sizeof(int), 1, file, error_code);

    for(int i = 0; i < network->size; i++){
        save_layer(network->layers[i], file, error_code);
    }

    CHECKED_FCLOSE(file, error_code);
}

static void load_layer(Layer *layer, FILE *file, int *error_code){

    CHECKED_FREAD(&layer->inputs, sizeof(int), 1, file, error_code);
    CHECKED_FREAD(&layer->outputs, sizeof(int), 1, file, error_code);

    const int weight_size = layer->inputs * layer->outputs;
    const int bias_size = layer->outputs;

    CHECKED_MALLOC(layer->weights, weight_size, double, error_code);
    CHECKED_MALLOC(layer->biases, bias_size, double, error_code);

    CHECKED_FREAD(layer->weights, sizeof(double), weight_size, file, error_code);
    CHECKED_FREAD(layer->biases, sizeof(double), bias_size, file, error_code);
}

void load_network(NeuralNetwork **network, const char* filename, int *error_code){
    FILE *file;

    CHECKED_FOPEN(file, filename, "rb", error_code);

    CHECKED_MALLOC(*network, 1, NeuralNetwork, error_code);

    int size;

    CHECKED_FREAD(&size, sizeof(int), 1, file, error_code);

    (*network)->size = size;

    CHECKED_MALLOC((*network)->layers, size, Layer, error_code);

    for(int i = 0; i < size; i++){
        Layer *current_layer = &(*network)->layers[i];
        load_layer(current_layer, file, error_code);
    }

    CHECKED_FCLOSE(file, error_code);
}