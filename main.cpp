#include <iostream>
#include "eSNN.h"
#include <windows.h>


using namespace std;

void ReadDirectory(const std::string &name, vector<string> &v) {
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}

void PrintActualParameters() {
    cout << "Actual: NOsize: " << NOsize << " Wsize: " << Wsize << " NIsize: " << NIsize;
    cout << " Beta: " << Beta << " TS: " << TS << " sim: " << sim << " mod: " << mod << " C: " << C;
    cout << " ErrorFactor: " << ErrorFactor << " AnomalyFactor: " << AnomalyFactor << endl;
}

int main() {


    string path = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                  "\\Datasets\\numenta\\";

    string resultsPath = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                         "\\Results\\numenta_Precision_optimization\\";

    vector<string> folders = {
            "artificialWithAnomaly",
            "realAdExchange",
            "realAWSCloudwatch",
            "realKnownCause",
            "realTraffic",
            "realTweets"
    };

    for (int j = 0; j < folders.size(); j++) {
        string folder = folders[j];

        string dirPath = path + "\\" + folder;
        vector<string> files;

        ReadDirectory(dirPath, files); //load all datafiles from a directory

        double o_NOsize, o_Wsize, o_NIsize, o_Beta, o_TS, o_sim, o_mod, o_C, o_ErrorFactor, o_AnomalyFactor; //optimal found parametes of eSNN

        double NOsize_b = 50, NOsize_e = 150, NOsize_s = 100; //parameters for grid search (xxx_b - intial, xxx_e - ending, xxx_s - step)
        double Wsize_b = 100, Wsize_e = 600, Wsize_s = 100;
        double NIsize_b = 10, NIsize_e = 30, NIsize_s = 20;
        double Beta_b = 1.6, Beta_e = 1.6, Beta_s = 0.2;
        double TS_b = 1000, TS_e = 1000, TS_s = 1000;
        double sim_b = 0.15, sim_e = 0.15, sim_s = 0.15;
        double mod_b = 0.6, mod_e = 0.6, mod_s = 0.1;
        double C_b = 0.6, C_e = 0.6, C_s = 0.1;
        double ErrorFactor_b = 0.7, ErrorFactor_e = 0.9, ErrorFactor_s = 0.1;
        double AnomalyFactor_b = 2, AnomalyFactor_e = 7, AnomalyFactor_s = 1;

        //double maxMeanFMeasure = 0.0; //optimize max Mean FMeasure over datasets in each folder
        double maxMeanPrecision = 0.0;

        int Trials = 5; //for each set of eSNN parameters perform num of Trials, average results

        cout << "#####################################################################" << endl;
        cout << folder << endl;
        for (double NOsize_c = NOsize_b; NOsize_c <= NOsize_e; NOsize_c += NOsize_s) {
            for (double Wsize_c = Wsize_b; Wsize_c <= Wsize_e; Wsize_c += Wsize_s) {
                for (double NIsize_c = NIsize_b; NIsize_c <= NIsize_e; NIsize_c += NIsize_s) {
                    for (double Beta_c = Beta_b; Beta_c <= Beta_e; Beta_c += Beta_s) {
                        for (double TS_c = TS_b; TS_c <= TS_e; TS_c += TS_s) {
                            for (double sim_c = sim_b; sim_c <= sim_e; sim_c += sim_s) {
                                for (double mod_c = mod_b; mod_c <= mod_e; mod_c += mod_s) {
                                    for (double C_c = C_b; C_c <= C_e; C_c += C_s) {
                                        for (double ErrorFactor_c = ErrorFactor_b;
                                             ErrorFactor_c <= ErrorFactor_e; ErrorFactor_c += ErrorFactor_s) {
                                            for (double AnomalyFactor_c = AnomalyFactor_b;
                                                 AnomalyFactor_c <= AnomalyFactor_e;
                                                 AnomalyFactor_c += AnomalyFactor_s) {

                                                NOsize = NOsize_c, Wsize = Wsize_c, NIsize = NIsize_c, Beta = Beta_c,
                                                TS = TS_c, sim = sim_c, mod = mod_c, C = C_c, ErrorFactor = ErrorFactor_c,
                                                AnomalyFactor = AnomalyFactor_c;

                                                PrintActualParameters();

                                                double meanFMeasure = 0.0, meanPrecision = 0.0, meanRecall = 0.0, meanAUC = 0.0;

                                                for (int I = 0; I < Trials; I++) {
                                                    double meanFMeasureTrials = 0.0, meanPrecisionTrials = 0.0,
                                                            meanRecallTrials = 0.0, meanAUCTrials = 0.0;
                                                    for (int i = 2; i < files.size(); i++) {
                                                        LoadData(dirPath + "\\" + files[i]); //load dataset

                                                        TraineSNN(); //train eSNN with given set of parameters

                                                        CalculateConfusionMatrix(); //calculate confusion Matrix
                                                        double precision = CalculatePrecision(); //calculate statistics
                                                        double recall = CalculateRecall();
                                                        double fMeasure = CalculateF_Measure(precision, recall);
                                                        double AUC = CalculateAUC();

                                                        meanFMeasureTrials += fMeasure;
                                                        meanPrecisionTrials += precision;
                                                        meanRecallTrials += recall;
                                                        meanAUCTrials += AUC;
                                                        ClearStructures();
                                                    }

                                                    meanFMeasureTrials /= files.size() - 2;
                                                    meanPrecisionTrials /= files.size() - 2;
                                                    meanRecallTrials /= files.size() - 2;
                                                    meanAUCTrials /= files.size() - 2;

                                                    meanFMeasure += meanFMeasureTrials;
                                                    meanPrecision += meanPrecisionTrials;
                                                    meanRecall += meanRecallTrials;
                                                    meanAUC += meanAUCTrials;

                                                }

                                                meanFMeasure /= Trials;
                                                meanPrecision /= Trials;
                                                meanRecall /= Trials;
                                                meanAUC /= Trials;

                                                //cout << "Actual meanFMeasure " << meanFmeasure << endl;

                                                cout << "Actual meanRecall " << meanRecall << endl;
                                                cout << "Actual meanPrecision " << meanPrecision << endl;

                                                string metricsFilePath = resultsPath + "\\" + folder + "\\" +
                                                                         "metr_Overall_OptimizationTrace_" + folder;
                                                SaveMetricsTrace(metricsFilePath, meanPrecision, meanRecall,
                                                            meanFMeasure, meanAUC); //save optimal paramterers and metrics to file

                                                //if (meanFMeasure > maxMeanFMeasure) { //rember parametrs for mzx mean FMeasure
                                                //   maxMeanFMeasure = meanFMeasure;

                                                if(meanPrecision >= maxMeanPrecision)
                                                {
                                                    maxMeanPrecision = meanPrecision;
                                                    o_NOsize = NOsize;
                                                    o_Wsize = Wsize;
                                                    o_NIsize = NIsize;
                                                    o_Beta = Beta;
                                                    o_TS = TS;
                                                    o_sim = sim;
                                                    o_mod = mod;
                                                    o_C = C;
                                                    o_ErrorFactor = ErrorFactor;
                                                    o_AnomalyFactor = AnomalyFactor;

                                                    string metricsFilePath = resultsPath + "\\" + folder + "\\" +
                                                                             "metr_Overall_Optimal_" + folder;
                                                    SaveMetrics(metricsFilePath, meanPrecision, meanRecall,
                                                                meanFMeasure, meanAUC); //save optimal paramterers and metrics to file
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        cout << endl;
        cout << "Optimized calculation" << endl;

        //////////////////////////////////////////////////
        //////////////////////////////////////////////////
        //Below generate example results for optimal found parameters of eSNN for current folder (artificialWithAnomaly, realAdExchange ...)

        NOsize = o_NOsize, Wsize = o_Wsize, NIsize = o_NIsize, Beta = o_Beta, TS = o_TS, sim = o_sim,
        mod = o_mod, C = o_C, ErrorFactor = o_ErrorFactor, AnomalyFactor = o_AnomalyFactor;

        PrintActualParameters();
        //cout << "Optimized meanFMeasure " << maxMeanFMeasure << endl;
        cout << "Optimized mean precision " << maxMeanPrecision<< endl;


        for (int i = 2; i < files.size(); i++) {
            LoadData(dirPath + "\\" + files[i]);

            TraineSNN();

            CalculateConfusionMatrix();
            double precision = CalculatePrecision();
            double recall = CalculateRecall();
            double fMeasure = CalculateF_Measure(precision, recall);
            double AUC = CalculateAUC();


            string resultsFilePath = resultsPath + "\\" + folder + "\\" + folder + "_" + files[i];
            SaveResults(resultsFilePath);
            string metricsFilePath = resultsPath + "\\" + folder + "\\" + "metr_" + folder + "_" + files[i];
            SaveMetrics(metricsFilePath, precision, recall, fMeasure, AUC);
            ClearStructures();
        }
    }

    return 0;

}
