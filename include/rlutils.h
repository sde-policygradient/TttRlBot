#ifndef RLUTILS_H
#define RLUTILS_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct{
    double *weights;
    double *biases;
    int inputs;
    int outputs;
}Layer;

typedef struct{
    Layer *layers;
    int size;
}NeuralNetwork;

typedef double activation_t(double);

double ReLu(double in);

double sigmoid(double in);

double swish(double in);

NeuralNetwork* createNetwork(const int size, const int layerSizes[]);

void initNetwork(NeuralNetwork *network);

void freeNetwork(NeuralNetwork *network);

void saveNetwork(NeuralNetwork *network, const char *filename);

NeuralNetwork* loadNetwork(const char* filename);

double* calculateLayerOutput(Layer *layer, const double inputs[], activation_t activationFunc);

double* calculateNetworkOutput(NeuralNetwork *network, const double inputs[], const int networkSize, activation_t activationFunc);

void backpropagate(NeuralNetwork *network, const double inputs[], const double outputs[], const double expectedOutputs[], activation_t activationFunc);

int selectRandomOutput(const double numbers[], const int size);

double tempDiffError(double reward, double discountFactor, double optimalFutureVal, double currentVal);

double meanSquareError(double values[], double expectedValues[], int size);

#endif