#include <iostream>
#include "eSNN.h"
#include <windows.h>


using namespace std;

//Read all data files in a given directory
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


    //comment or uncoment lines 31-63 base on benchmark you want to test
/*
    string path = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                  "\\Datasets\\numenta\\";

    string resultsPath = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                         "\\Results\\numenta_fMeasure_optimization\\";

    vector<string> folders = {
            "artificialNoAnomaly",
           "artificialWithAnomaly",
            "realAdExchange",
            "realAWSCloudwatch",
            "realKnownCause",
            "realTraffic",
            "realTweets"
    };

*/

    string path = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                  "\\Datasets\\Yahoo\\";
    string resultsPath = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                         "\\Results\\yahoo_fMeasure_optimization\\";
    vector<string> folders = {
            "A1Benchmark",
            "A2Benchmark",
            "A3Benchmark",
            "A4Benchmark",
    };


    for (int j = 0; j < folders.size(); j++) {
        string folder = folders[j];

        string dirPath = path + "\\" + folder;
        vector<string> files;

        ReadDirectory(dirPath, files); //load all datafiles from a directory
        

        //below set parameters for the selected benchmark
/*  //Numenta benchmark parameters set
        double NOsize_b = 50, NOsize_e = 50, NOsize_s = 100; //parameters for grid search (xxx_b - intial, xxx_e - ending, xxx_s - step)
        double Wsize_b = 100, Wsize_e = 600, Wsize_s = 100;
        double NIsize_b = 10, NIsize_e = 10, NIsize_s = 20;
        double Beta_b = 1.0, Beta_e = 1.0, Beta_s = 0.2;
        double TS_b = 1000, TS_e = 1000, TS_s = 1000;
        double sim_b = 0.1, sim_e = 0.1, sim_s = 0.1;
        double mod_b = 0.6, mod_e = 0.6, mod_s = 0.1;
        double C_b = 0.6, C_e = 0.6, C_s = 0.1;
        double ErrorFactor_b = 0.9, ErrorFactor_e = 0.9, ErrorFactor_s = 0.1;
        double AnomalyFactor_b = 2, AnomalyFactor_e = 7, AnomalyFactor_s = 1;
*/

    //Yahoo benchmark parameters set
       double NOsize_b = 50, NOsize_e = 50, NOsize_s = 100; //parameters for grid search (xxx_b - intial, xxx_e - ending, xxx_s - step)
        double Wsize_b = 20, Wsize_e = 500, Wsize_s = 20;
        double NIsize_b = 10, NIsize_e = 10, NIsize_s = 20;
        double Beta_b = 1.0, Beta_e = 1.0, Beta_s = 0.2;
        double TS_b = 1000, TS_e = 1000, TS_s = 1000;
        double sim_b = 0.1, sim_e = 0.1, sim_s = 0.1;
        double mod_b = 0.6, mod_e = 0.6, mod_s = 0.1;
        double C_b = 0.8, C_e = 0.8, C_s = 0.1;
        double ErrorFactor_b = 0.9, ErrorFactor_e = 0.9, ErrorFactor_s = 0.1;
        double AnomalyFactor_b = 2, AnomalyFactor_e = 17, AnomalyFactor_s = 1;

        vector<double> dataFile_Precision, dataFile_Recall, dataFile_FMeasure, dataFile_AUC;

        for (int i = 2; i < files.size(); i++) {

            double maxFMeasure = 0.0, maxPrecision = 0.0, maxRecall = 0.0;
            double threshold = 0.0;

            cout << "#####################################################################" << endl;
            cout << folder << "/" << files[i] << endl;

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

                                                    LoadData(dirPath + "\\" + files[i]); //load dataset

                                                    TraineSNN(); //train eSNN with given set of parameters

                                                    CalculateConfusionMatrix(); //calculate confusion Matrix
                                                    double precision = CalculatePrecision(); //calculate statistics
                                                    double recall = CalculateRecall();
                                                    double fMeasure = CalculateF_Measure(precision, recall);


                                                    if (fMeasure >
                                                        threshold) { //rember parametrs for mzx mean FMeasure
                                                        threshold = fMeasure;
                                                        maxFMeasure = fMeasure;
                                                        maxPrecision = precision;
                                                        maxRecall = recall;

                                                        string resultsFilePath =
                                                                resultsPath + "\\" + folder + "\\" + folder + "_" +
                                                                files[i];
                                                        SaveResults(resultsFilePath);
                                                        string metricsFilePath =
                                                                resultsPath + "\\" + folder + "\\" + "metr_" + folder +
                                                                "_" + files[i];
                                                        SaveMetrics(metricsFilePath, precision,
                                                                    recall, fMeasure, 0);

                                                    }
                                                    ClearStructures();
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

            cout << "Precision " << maxPrecision << " Recall " << maxRecall << " fMeasure "
                 << maxFMeasure << endl;
            dataFile_Precision.push_back(maxPrecision);
            dataFile_Recall.push_back(maxRecall);
            dataFile_FMeasure.push_back(maxFMeasure);

        }

        double precisionOverall = CalculateAvgW(dataFile_Precision);
        double recallOverall = CalculateAvgW(dataFile_Recall);
        double fMeasureOverall = CalculateAvgW(dataFile_FMeasure);

        cout << "Precision overall " << precisionOverall << " Recall overall " << recallOverall << " fMeasure overall "
             << fMeasureOverall << endl;

        string metricsFilePath = resultsPath + "\\" + folder + "\\" +
                                 "metr_Overall_Optimal_" + folder;
        SaveMetricsOverall(metricsFilePath, precisionOverall, recallOverall,
                           fMeasureOverall);



    }

    return 0;

}