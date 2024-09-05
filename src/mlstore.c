#include"mlstore.h"
#include"mltypes.h"
#include<stdio.h>
#include<stdlib.h>

NeuralNetwork* create_network(const int size, const int layer_sizes[]){
    NeuralNetwork *network = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
    network->size = size;
    network->layers = (Layer*)calloc(size, sizeof(Layer));

    for(int i = 0; i < size; i++){
        Layer *current_layer = &network->layers[i];

        {
            current_layer->inputs = !i?layer_sizes[i]:layer_sizes[i - 1];
            current_layer->outputs = layer_sizes[i];
        }

        {
            current_layer->weights = (double*)calloc(current_layer->inputs * current_layer->outputs, sizeof(double));
            current_layer->biases = (double*)calloc(current_layer->outputs, sizeof(double));
        }
    }

    return network;
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

static void save_layer(const Layer layer, FILE *file){
    fwrite(&layer.inputs, sizeof(int), 1, file);
    fwrite(&layer.outputs, sizeof(int), 1, file);
    fwrite(layer.weights, sizeof(double), layer.inputs * layer.outputs, file);
    fwrite(layer.biases, sizeof(double), layer.outputs, file);
}

void save_network(NeuralNetwork *network, const char *filename){
    FILE *file = fopen(filename, "wb");

    if(file == NULL){
        perror("Can't save network");
        return;
    }

    fwrite(&network->size, sizeof(int), 1, file);

    for(int i = 0; i < network->size; i++){
        save_layer(network->layers[i], file);
    }

    fclose(file);
}

static void load_layer(Layer *layer, const int weight_size, const int bias_size, FILE *file){
    layer->weights = (double*)calloc(weight_size, sizeof(double));
    layer->biases = (double*)calloc(bias_size, sizeof(double));

    fread(layer->weights, sizeof(double), weight_size, file);
    fread(layer->biases, sizeof(double), bias_size, file);
}

NeuralNetwork* load_network(const char* filename){
    FILE *file = fopen(filename, "rb");

    if(file == NULL){
        perror("Can't load network");
        return NULL;
    }

    NeuralNetwork *network = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
    int size;

    fread(&size, sizeof(int), 1, file);

    network->size = size;
    network->layers = (Layer*)calloc(size, sizeof(Layer));

    for(int i = 0; i < size; i++){
        Layer *current_layer = &network->layers[i];

        {
            fread(&current_layer->inputs, sizeof(int), 1, file);
            fread(&current_layer->outputs, sizeof(int), 1, file);
        }

        {
            const int weight_size = current_layer->inputs * current_layer->outputs;
            const int bias_size = current_layer->outputs;

            load_layer(current_layer, weight_size, bias_size, file);
        }
    }

    fclose(file);

    return network;
}