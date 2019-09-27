//
// Created by Piotr on 14.09.2019.
//

#include "eSNN.h"


int datasetSize;

int CNOsize;
int Wsize;
int NOsize;
double Beta;
int NIsize;
double TS;
double sim;
double C;
double mod;
double ErrorFactor;
double AnomalyFactor;

vector<neuron *> OutputNeurons;
vector<inputValue> X;
vector<double> Y;
vector<bool> U;
vector<double> E;
vector<GRFstruct> GRFs;
vector<vector<inputNeuron>> spikeOrder;
vector<int> NeuronFired;
vector<double> psps;
vector<double> gammas;

ConfusionMatrixStruct ConfusionMatrix;
vector<data_auc_struct> dataSorted;

int neuronAge = 0;

void InitializeGRFs(vector<double> Window) { //intialize each GRF with current Window
    double I_max = *max_element(Window.begin(), Window.end());
    double I_min = *min_element(Window.begin(), Window.end());

    for (int i = 0; i < GRFs.size(); i++) {

        double mu = I_min + ((2.0 * i - 3.0) / 2.0) * ((I_max - I_min) / (double(NIsize) - 2));
        double sigma = (1.0 / Beta) * (((I_max - I_min) / (double(NIsize) - 2)));

        GRFs[i].mu = mu;
        GRFs[i].sigma = sigma;

    }
}

bool compFiringTime(const inputNeuron &nI1, const inputNeuron &nI2) { //comparator of firing times
    return nI1.firingTime < nI2.firingTime;
}

void CalculateSpikeOrder(vector<double> Window) { //calculate firing times of input neurons for current X[t]

    spikeOrder.clear();
    //for (int k = Window.size() - 1; k >= Window.size() - 1; k--) {
    vector<inputNeuron> inputNeurons;
    for (int j = 0; j < GRFs.size(); j++) {
        if (GRFs[j].sigma == 0.0) {
            GRFs[j].sigma = 1.0;
        }
        double exc = (exp(-0.5 * pow(((Window[Window.size() - 1] - GRFs[j].mu) / GRFs[j].sigma), 2)));
        double firingTime = /*floor*/(TS * (1 - exc));
        inputNeuron newIN = {j, firingTime};
        inputNeurons.push_back(newIN);

    }

    sort(inputNeurons.begin(), inputNeurons.end(), compFiringTime);
    spikeOrder.push_back(inputNeurons);
    //}
}

double CalculateAvgW(vector<double> vec) { //average of values in vec
    double avg = 0.0;

    for (int k = 0; k < vec.size(); k++) {
        avg += vec[k];
    }

    return avg / vec.size();
}

double CalculateStdW(vector<double> vec, double avg) { //standard deviation of values in vec
    double sqSum = 0.0;

    for (int k = 0; k < vec.size(); k++) {
        sqSum += pow(vec[k] - avg, 2);
    }

    if (vec.size() > 1) {
        return sqrt((sqSum) / (vec.size() - 1));
    } else {
        return sqrt((sqSum) / (vec.size()));
    }
}

void InitializeNeuron(neuron *n_i, const vector<double> &Window) { //Initalize new neron n_i
    unsigned seed_w = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator_w(seed_w);
    uniform_real_distribution<double> distribution_w(0.0, 0.001);

    for (int i = 0; i < NIsize; i++) {
        n_i->s_weights.push_back(0.0);
    }

    int order = 0;
    for (int k = 0; k < /*spikeOrder.size()*/ 1; k++) {
        for (int j = 0; j < spikeOrder[k].size(); j++) {
            n_i->s_weights[spikeOrder[k][j].id] += pow(mod, order);
            order++;
        }
    }

    order = 0;
    for (int k = 0; k < /*spikeOrder.size()*/ 1; k++) {
        for (int j = 0; j < spikeOrder[k].size(); j++) {
            n_i->PSP_max += n_i->s_weights[spikeOrder[k][j].id] * pow(mod, order);
            order++;
        }
    }
    n_i->gamma = n_i->PSP_max * C;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    double avgW = CalculateAvgW(Window);
    double stdW = CalculateStdW(Window, avgW);
    normal_distribution<double> distribution(avgW, stdW);
    n_i->outputValue = distribution(generator);
    n_i->M += 1;
    n_i->age = neuronAge;
    neuronAge++;
}

void UpdateNeuron(neuron *n_i, neuron *n_s) { //Update neuron n_s in output repository
    for (int j = 0; j < n_s->s_weights.size(); j++) {
        n_s->s_weights[j] = (n_i->s_weights[j] + n_i->s_weights[j] * n_s->M) / (n_s->M + 1);
    }
    n_s->gamma = (n_i->gamma + n_s->gamma * n_s->M) / (n_s->M + 1);
    n_s->outputValue = (n_i->outputValue + n_s->outputValue * n_s->M) / (n_s->M + 1);
    n_s->M += 1;
    delete n_i;
}

void ErrorCorrection(neuron *n_f, double x) { //error correction function
    n_f->outputValue += (x - n_f->outputValue) * ErrorFactor;
}

bool ClassifyAnomaly(double x, double y) { //anomaly classification function
    vector<double> eVec;

    if (E.size() >= Wsize) {
        for (int k = E.size() - 2; k > E.size() - 2 - (Wsize - 1); k--) {
            if (U[k] == false) {
                eVec.push_back(E[k]);
            }
        }
    } else {
        for (int k = 0; k < E.size() - 1; k++) {
            if (U[k] == false) {
                eVec.push_back(E[k]);
            }
        }
    }

    double avgE = CalculateAvgW(eVec);
    double stdE = CalculateStdW(eVec, avgE);
    //abs
    return (E[E.size() - 1] - avgE) > AnomalyFactor * stdE;
}

double CalculateDistance(const vector<double> &v1, const vector<double> &v2) { //calculate distance between two weights vectors
    double diffSq = 0.0;

    for (int j = 0; j < v1.size(); j++) {
        diffSq += pow(v1[j] - v2[j], 2);
    }

    return sqrt(diffSq);
}

neuron *FindMostSimilar(neuron *n_i) { //find mos similar neurons in terms of synaptic weights
    double minDist = CalculateDistance(n_i->s_weights, OutputNeurons[0]->s_weights);
    double minIdx = 0;

    if (OutputNeurons.size() > 1) {
        for (int k = 1; k < OutputNeurons.size(); k++) {
            double dist = CalculateDistance(n_i->s_weights, OutputNeurons[k]->s_weights);
            if (dist < minDist) {
                minDist = dist;
                minIdx = k;
            }
        }
    }
    return OutputNeurons[minIdx];
}

void ReplaceOldest(neuron *n_i) { //replace the oldets neuron in output repostiory
    int oldest = OutputNeurons[0]->age;
    int oldestIdx = 0;


    for (int l = 1; l < OutputNeurons.size(); l++) {
        if (oldest > OutputNeurons[l]->age) {
            oldest = OutputNeurons[l]->age;
            oldestIdx = l;
        }
    }

    delete OutputNeurons[oldestIdx];
    OutputNeurons[oldestIdx] = n_i;
}

void ZeroizePSP() { //zeroize current PSP ouf output neurons
    for (int i = 0; i < OutputNeurons.size(); i++) {
        OutputNeurons[i]->PSP = 0.0;
    }
}

neuron *NeuronSpikeFirst() { //obtain neuron from output repository, which spike first
    int order = 0;

    double maxCurrentPSPDiff;
    int maxCurrentPSPDiffIndex;
    ZeroizePSP();

    vector<int> toFire;

    for (int j = 0; j < /*spikeOrder.size()*/1; j++) {
        for (int l = 0; l < spikeOrder[j].size(); l++) {
            for (int i = 0; i < OutputNeurons.size(); i++) {
                OutputNeurons[i]->PSP += pow(mod, order) * OutputNeurons[i]->s_weights[spikeOrder[j][l].id];
            }
            for (int i = 0; i < OutputNeurons.size(); i++) {
                if (OutputNeurons[i]->PSP > OutputNeurons[i]->gamma) {
                    toFire.push_back(i);
                }
            }
            maxCurrentPSPDiff = 0.0;
            maxCurrentPSPDiffIndex = -1;
            if (toFire.empty() != true) {
                for (int i = 0; i < toFire.size(); i++) {
                    if ((OutputNeurons[toFire[i]]->PSP - OutputNeurons[toFire[i]]->gamma) > maxCurrentPSPDiff) {
                        maxCurrentPSPDiff = OutputNeurons[toFire[i]]->PSP - OutputNeurons[toFire[i]]->gamma;
                        maxCurrentPSPDiffIndex = toFire[i];
                    }
                }
                toFire.clear();
                return (OutputNeurons[maxCurrentPSPDiffIndex]);
            }
            order++;
        }
    }
}

void TraineSNN() { //main RTAD-eSNN procedure

    CNOsize = 0;
    for (int j = 0; j < NIsize; j++) {
        GRFstruct newGRF;
        GRFs.push_back(newGRF);
    }

    vector<double> Window;

    for (int k = 0; k < Wsize; k++)
        Window.push_back(X[k].value);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    double avgW = CalculateAvgW(Window);
    double stdW = CalculateStdW(Window, avgW);
    normal_distribution<double> distribution(avgW, stdW);

    for (int k = 0; k < Wsize; k++)
        Y.push_back(distribution(generator));

    for (int k = 0; k < Wsize; k++) {
        E.push_back(abs(X[k].value - Y[k]));
        NeuronFired.push_back(-1);
    }


    for (int k = 0; k < Wsize; k++)
        U.push_back(false);


    for (int t = Wsize; t < X.size(); t++) {

        if (Window.size() == Wsize - 1) {
            Window.push_back(X[t].value);
        } else {
            Window.erase(Window.begin());
            Window.push_back(X[t].value);
        }

        InitializeGRFs(Window);

        CalculateSpikeOrder(Window);

        neuron *n_i = new neuron;
        InitializeNeuron(n_i, Window);


        neuron *n_s;
        if (CNOsize > 0) {
            n_s = FindMostSimilar(n_i);
        }


        if (CNOsize > 0 && CalculateDistance(n_i->s_weights, n_s->s_weights) <= sim) {
            UpdateNeuron(n_i, n_s);
        } else if (CNOsize < NOsize) {
            OutputNeurons.push_back(n_i);
            CNOsize++;
        } else {
            ReplaceOldest(n_i);
        }


        ///////////////////////////////

        neuron *n_f = NeuronSpikeFirst();

        NeuronFired.push_back(n_f->age);
        psps.push_back(n_f->PSP);
        gammas.push_back(n_f->gamma);
        double y_t_1 = n_f->outputValue;

        Y.push_back(y_t_1);
        double x_t_1 = X[t].value;
        double e_t_1 = abs(x_t_1 - y_t_1);
        E.push_back(e_t_1);

        bool u_t_1 = ClassifyAnomaly(x_t_1, y_t_1);

        U.push_back(u_t_1);
        if (u_t_1 == false) {
            ErrorCorrection(n_f, x_t_1);
        }

    }
}

///////////////////////////////////////////////////////
//Print Results Procedures

void SaveResults(string filePath) {
    fstream handler;
    handler.open(filePath, iostream::out);

    handler << "timestamp,value,predicted_value,error,p_label,fired,r_label" << endl;
    for (int t = 0; t < X.size(); t++) {
        handler << setprecision(12) << X[t].timeStamp << "," << X[t].value << "," << Y[t] << "," << E[t] << "," << U[t]
                << ",";
        handler << NeuronFired[t] << "," << X[t].r_label;
        handler << endl;
    }
    handler.close();
}


void SaveMetrics(string filePath, double precision, double recall, double fMeasure, double Auc) {
    fstream handler;
    handler.open(filePath, iostream::out);

    handler << "eSNN Parameters:" << endl;
    handler << "NOsize: " << NOsize << " Wsize: " << Wsize << " NIsize: " << NIsize;
    handler << " Beta: " << Beta << " TS: " << TS << " sim: " << sim << " mod: " << mod << " C: " << C;
    handler << " ErrorFactor: " << ErrorFactor << " AnomalyFactor: " << AnomalyFactor << endl;
    handler << "Metrics: " << endl;
    handler << "Precision " << precision << " Recall " << recall << " fMeasure " << fMeasure << " AUC " << Auc;

    handler.close();
}
///////////////////////////////////////////////////////
//Load Data Procedures

int CountInstances(string fileName) {
    fstream handler;
    handler.open(fileName);
    string line;

    int numInstances = 0;

    while (handler.eof() != true) {
        getline(handler, line);

        if (line != "") {
            numInstances++;
        }
    }
    handler.close();

    return numInstances;
}

bool compValDataSort(const data_auc_struct &d1, const data_auc_struct &d2) {
    return d1.value > d2.value;
}

void LoadData(string fileName) {
    fstream handler;

    datasetSize = CountInstances(fileName); // zlicz l. instancji w pliku

    handler.open(fileName);
    for (int i = 0; i < datasetSize; i++) {
        string line;
        getline(handler, line);
        stringstream linestream(line);
        string dataPortion;

        if (line != "") {
            getline(linestream, dataPortion, ',');
            string timeStamp = dataPortion;
            getline(linestream, dataPortion, ',');
            double value = stod(dataPortion);
            getline(linestream, dataPortion, ' ');
            int r_label = stoi(dataPortion);

            inputValue newValue = {timeStamp, value, (r_label == 0 ? false : true)};

            X.push_back(newValue);
        }
    }
    handler.close();


}

void ClearStructures() {
    for (int i = 0; i < OutputNeurons.size(); i++) {
        delete OutputNeurons[i];
    }
    OutputNeurons.clear();
    X.clear();
    Y.clear();
    E.clear();
    U.clear();
    GRFs.clear();
    spikeOrder.clear();
    NeuronFired.clear();
    neuronAge = 0;
    ConfusionMatrix.TP = 0, ConfusionMatrix.FP = 0, ConfusionMatrix.FN = 0, ConfusionMatrix.TN = 0;
}

///////////////////
//Calculate metrics procedures

void CalculateConfusionMatrix() {
    for (int t = 0; t < X.size(); t++) {
        if (X[t].r_label == U[t]) {
            if (X[t].r_label == true && U[t] == true) {
                ConfusionMatrix.TP++;
            } else if (X[t].r_label == false && U[t] == false) {
                ConfusionMatrix.TN++;
            }
        } else {
            if (X[t].r_label == true && U[t] == false) {
                ConfusionMatrix.FN++;
            } else if (X[t].r_label == false && U[t] == true) {
                ConfusionMatrix.FP++;
            }
        }
    }
}

double CalculatePrecision()
{
    if(ConfusionMatrix.TP == 0)
    {
        return (0.0000001);
    }
    else
    {
        return (double(ConfusionMatrix.TP)/double((ConfusionMatrix.TP+ConfusionMatrix.FP)));
    }
}
double CalculateRecall()
{
    if(ConfusionMatrix.TP == 0)
    {
        return (0.0000001);
    }
    else
    {
        return (double(ConfusionMatrix.TP)/double((ConfusionMatrix.TP+ConfusionMatrix.FN)));
    }
}
double CalculateF_Measure(double precision, double recall)
{
    return (2*precision*recall)/(precision + recall);
}

double CalculateAUC() {//not used! only approximation of AUC for binary classiier

    double AUC = 0.0;

    double sse = (double(ConfusionMatrix.TP))/(double(ConfusionMatrix.TP + ConfusionMatrix.FN))/2.0;
    double spe = (double(ConfusionMatrix.TN))/(double(ConfusionMatrix.TN + ConfusionMatrix.FP))/2.0;

    return (sse+spe);
}