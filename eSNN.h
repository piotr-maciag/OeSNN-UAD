//
// Created by Piotr on 14.09.2019.
//

#ifndef ESNN_RTAD_ESNN_H
#define ESNN_RTAD_ESNN_H

#include "iostream"
#include "algorithm"
#include "vector"
#include "fstream"
#include "sstream"
#include "math.h"
#include "chrono"
#include "random"
#include "iomanip"

using namespace std;

struct neuron
{
    int ID;
    vector<double> s_weights;
    double PSP_max = 0.0;
    double gamma;
    double outputValue;
    double M;
    double PSP;
    int additionTime;
}; //output neuron structure

struct inputValue
{
    string timeStamp;
    double value;
    bool r_label;
};  //input value read from input file

struct GRFstruct
{
    double mu, sigma, exc;
}; //struct of GRF

struct inputNeuron
{
    int id;
    double firingTime;
}; //input neuron structure

extern int CNOsize;
extern int Wsize;
extern int NOsize;
extern double Beta;
extern int NIsize;
extern double TS;
extern double sim;
extern double C;
extern double mod;
extern double ErrorFactor;
extern double AnomalyFactor;

extern vector<neuron *> OutputNeurons; //Pointers to output neurons (outputneuron repository)
extern vector<inputValue> X; //input dataset
extern vector<double> Y; //predicted values of eSNN
extern vector<bool> U; //classification of each X[t] input value
extern vector<double> E; //error between predicted Y[t] and X[t]
extern vector<GRFstruct> GRFs; //input GRFs
extern vector<vector<inputNeuron>> spikeOrder; //firing order of input neurons for current X[t]


void LoadData(string fileName);
int CountInstances(string fileName);

void InitializeGRFs(vector<double> Window);
void InitializeNeuron(neuron* n_i, const vector<double> &Window);
void UpdateNeuron(neuron* n_i, neuron* n_s);
void ErrorCorrection(neuron* n_f, double x);
bool ClassifyAnomaly(double x, double y);
void TraineSNN(); //main procedure of eSNN-RTAD

void SaveResults(string filePath);
void ClearStructures();
void SaveMetrics(string, double, double, double, double);
void SaveMetricsOverall(string, double, double, double);

struct ConfusionMatrixStruct
{
    int TP = 0;
    int FP = 0;
    int FN = 0;
    int TN = 0;
} ;

extern ConfusionMatrixStruct ConfusionMatrix;

void CalculateConfusionMatrix();
double CalculatePrecision();
double CalculateRecall();
double CalculateF_Measure(double, double);

struct data_auc_struct
{
    bool label;
    bool value;
};

double CalculateAUC();


double CalculateAvgW(vector<double> vec); //average of values in vec
double CalculateStdW(vector<double> vec, double avg); //standard deviation of values in vec


#endif //ESNN_RTAD_ESNN_H
