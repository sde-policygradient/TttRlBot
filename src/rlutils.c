#include"rlutils.h"
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

NeuralNetwork* createNetwork(const int size, const int layerSizes[]){
    NeuralNetwork *network = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
    network->size = size;
    network->layers = (Layer*)calloc(size, sizeof(Layer));

    for(int i = 0; i < size; i++){
        Layer *currentLayer = &network->layers[i];

        currentLayer->inputs = !i?layerSizes[i]:layerSizes[i - 1];
        currentLayer->outputs = layerSizes[i];
        currentLayer->weights = (double*)calloc(currentLayer->inputs * currentLayer->outputs, sizeof(double));
        currentLayer->biases = (double*)calloc(currentLayer->outputs, sizeof(double));
    }

    return network;
}

void initNetwork(NeuralNetwork *network){
    for(int i = 0; i < network->size; i++){
        Layer *currentLayer = &network->layers[i];

        for(int j = 0; j < currentLayer->inputs * currentLayer->outputs; j++){
            currentLayer->weights[j] = i?(double)rand() / RAND_MAX:1;
        }

        for(int j = 0; j < currentLayer->outputs; j++){
            currentLayer->biases[j] = i?(double)rand() / RAND_MAX:0;
        }
    }
}

void freeNetwork(NeuralNetwork *network){
    if(!network)return;

    for(int i = 0; i < network->size; i++){
        Layer *currentLayer = &network->layers[i];

        free(currentLayer->weights);
        free(currentLayer->biases);
    }

    free(network->layers);
    free(network);
}

void saveNetwork(NeuralNetwork *network, const char *filename){
    FILE *file = fopen(filename, "wb");

    if(file == NULL){
        perror("Can't save network");
        return;
    }

    fwrite(&network->size, sizeof(int), 1, file);

    for(int i = 0; i < network->size; i++){
        Layer *currentLayer = &network->layers[i];

        fwrite(&currentLayer->inputs, sizeof(int), 1, file);
        fwrite(&currentLayer->outputs, sizeof(int), 1, file);
        fwrite(currentLayer->weights, sizeof(double), currentLayer->inputs * currentLayer->outputs, file);
        fwrite(currentLayer->biases, sizeof(double), currentLayer->outputs, file);
    }

    fclose(file);
}

NeuralNetwork* loadNetwork(const char* filename){
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
        Layer *currentLayer = &network->layers[i];
        int weightSize;
        int biasSize;

        fread(&currentLayer->inputs, sizeof(int), 1, file);
        fread(&currentLayer->outputs, sizeof(int), 1, file);

        weightSize = currentLayer->inputs * currentLayer->outputs;
        biasSize = currentLayer->outputs;

        currentLayer->weights = (double*)calloc(weightSize, sizeof(double));
        currentLayer->biases = (double*)calloc(biasSize, sizeof(double));

        fread(currentLayer->weights, sizeof(double), weightSize, file);
        fread(currentLayer->biases, sizeof(double), biasSize, file);
    }

    fclose(file);

    return network;
}

double* calculateLayerOutput(Layer *layer, const double inputs[], activation_t activationFunc){
    double *outputs = (double*)calloc(layer->outputs, sizeof(double));

    for(int i = 0; i < layer->outputs; i++){
        for(int j = 0; j < layer->inputs; j++){
            outputs[i] += inputs[j] * layer->weights[i * layer->inputs + j];
        }

        outputs[i] = activationFunc(outputs[i]);
    }

    return outputs;
}

double* calculateNetworkOutput(NeuralNetwork *network, const double inputs[], const int networkSize, activation_t activationFunc){
    double *layerOutputs = (double*)calloc(network->layers[0].inputs, sizeof(double));

    for(int i = 0; i < network->layers[0].inputs; i++){
        layerOutputs[i] = inputs[i];
    }

    for(int i = 1; i < network->size; i++){
        layerOutputs = (double*)realloc(layerOutputs, network->layers[i].outputs);
        layerOutputs = calculateLayerOutput(&network->layers[i], layerOutputs, i?activationFunc:none);
    }

    return layerOutputs;
}

void backpropagate(NeuralNetwork *network, const double inputs[], const double outputs[], const double expectedOutputs[], activation_t activationFunc){
    const int size = network->size;
    const int outLayerSize = network->layers[size - 1].outputs;
    const int numOutputs = network->layers[size - 1].outputs;
    double *costActivationGradients = (double*)calloc(outLayerSize, sizeof(double));

    {
        const double gradientCoefficient = 2 / numOutputs;

        for(int i = 0; i < numOutputs; i++){
            costActivationGradients[i] = gradientCoefficient * (outputs[i] - expectedOutputs[i]);
        }
    }

    for(int i = network->size - 1; i > 0; i--){
        Layer *currentLayer = &network->layers[i];
        Layer *prevLayer = &network->layers[i + 1];

        for(int j = 0; j < currentLayer->outputs; j++){
            double costActivationGradient = 0;
            const double rawOutput = calculateNetworkOutput(network, inputs, i + 1, activationFunc)[j];
            const double activationBiasGradient = derivative(activationFunc, rawOutput);
        }
    }
}

int selectRandomOutput(const double numbers[], const int size){
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

double tempDiffError(double reward, double discountFactor, double optimalFutureVal, double currentVal){
    double tempDiffTarget = reward + discountFactor * optimalFutureVal;

    return tempDiffTarget - currentVal;
}

double meanSquareError(double values[], double expectedValues[], int size){
    double squareError = 0;

    for(int i = 0; i < size; i++){
        squareError += (values[i] - expectedValues[i]);
        squareError *= squareError;
    }

    return squareError / size;
}