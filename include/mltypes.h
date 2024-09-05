#ifndef MLTYPES_H
#define MLTYPES_H

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

#endif