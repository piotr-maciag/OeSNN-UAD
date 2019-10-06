#include <iostream>
#include "eSNN.h"
#include <windows.h>


using namespace std;

void ReadDirectory(const std::string &name, vector<string> &v) {
    v.clear();
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

/*
    string path = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                  "\\Datasets\\numenta\\";


    vector<string> folders = {
      //      "artificialNoAnomaly",
            "artificialWithAnomaly",
            "realAdExchange",
            "realAWSCloudwatch",
    //        "realKnownCause",
            "realTraffic",
            "realTweets"
    };

*/
    string path = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                  "\\Datasets\\Yahoo\\";

    //string resultsPath = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
     //                    "\\Results\\yahoo_fMeasure_optimization\\train\\";

    vector<string> folders = {
            "A1Benchmark",
            "A2Benchmark",
            "A3Benchmark",
            "A4Benchmark",
    };



    for (int j = 0; j < folders.size(); j++) {
        string folder = folders[j];

        string path = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                      "\\Datasets\\Yahoo\\";

        string dirPath = path + "\\" + folder + "\\train";
        vector<string> files;

        ReadDirectory(dirPath, files); //load all datafiles from a directory

        double o_NOsize, o_Wsize, o_NIsize, o_Beta, o_TS, o_sim, o_mod, o_C, o_ErrorFactor, o_AnomalyFactor; //optimal found parametes of eSNN

        double NOsize_b = 50, NOsize_e = 50, NOsize_s = 100; //parameters for grid search (xxx_b - intial, xxx_e - ending, xxx_s - step)
        double Wsize_b = 300, Wsize_e = 300, Wsize_s = 100;
        double NIsize_b = 10, NIsize_e = 10, NIsize_s = 20;
        double Beta_b = 1.6, Beta_e = 1.6, Beta_s = 0.2;
        double TS_b = 1000, TS_e = 1000, TS_s = 1000;
        double sim_b = 0.15, sim_e = 0.15, sim_s = 0.15;
        double mod_b = 0.6, mod_e = 0.6, mod_s = 0.1;
        double C_b = 0.6, C_e = 0.6, C_s = 0.1;
        double ErrorFactor_b = 0.9, ErrorFactor_e = 0.9, ErrorFactor_s = 0.1;
        double AnomalyFactor_b = 10, AnomalyFactor_e = 10, AnomalyFactor_s = 1;

/*
       double NOsize_b = 50, NOsize_e = 50, NOsize_s = 100; //parameters for grid search (xxx_b - intial, xxx_e - ending, xxx_s - step)
        double Wsize_b = 20, Wsize_e = 500, Wsize_s = 20;
        double NIsize_b = 10, NIsize_e = 10, NIsize_s = 20;
        double Beta_b = 1.6, Beta_e = 1.6, Beta_s = 0.2;
        double TS_b = 1000, TS_e = 1000, TS_s = 1000;
        double sim_b = 0.15, sim_e = 0.15, sim_s = 0.15;
        double mod_b = 0.6, mod_e = 0.6, mod_s = 0.1;
        double C_b = 0.8, C_e = 0.8, C_s = 0.1;
        double ErrorFactor_b = 0.9, ErrorFactor_e = 0.9, ErrorFactor_s = 0.1;
        double AnomalyFactor_b = 2, AnomalyFactor_e = 18, AnomalyFactor_s = 1;

*/

        string resultsPath = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                             "\\Results\\yahoo_fMeasure_optimization\\" + folder + "\\train\\";

        double avgWin = 0.0;
        double avgerrFact = 0.0;

        for (int i = 2; i < files.size(); i++) {

            double maxFMeasure = 0.0, maxPrecision = 0.0, maxRecall = 0.0;

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
                                                        maxFMeasure) { //rember parametrs for mzx mean FMeasure
                                                        maxFMeasure = fMeasure;
                                                        maxPrecision = precision;
                                                        maxRecall = recall;

                                                        o_NOsize = NOsize, o_Wsize = Wsize, o_NIsize = NIsize, o_Beta = Beta,
                                                        o_TS = TS, o_sim = sim, o_mod = mod, o_C = C, o_ErrorFactor = ErrorFactor,
                                                        o_AnomalyFactor = AnomalyFactor;

                                                        string resultsFilePath =
                                                                resultsPath + folder + "_" +
                                                                files[i];
                                                        SaveResults(resultsFilePath);
                                                        string metricsFilePath =
                                                                resultsPath + "metr_" + folder +
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

            avgWin += o_Wsize;
            avgerrFact += o_AnomalyFactor;

        }

        avgWin /= (files.size() - 2);
        avgerrFact /= (files.size() - 2);

        cout << "xxx" << endl;
        path = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                      "\\Datasets\\Yahoo\\";

        dirPath = path + "\\" + folder + "\\test";
        ReadDirectory(dirPath, files); //load all datafiles from a directory

        resultsPath = "F:\\artykuly\\Anomaly detection eSNN\\Software\\eSNN-RTAD"
                      "\\Results\\yahoo_fMeasure_optimization\\" + folder + "\\test\\";

        vector<double> dataFile_Precision, dataFile_Recall, dataFile_FMeasure, dataFile_AUC;

        for (int i = 2; i < files.size(); i++) {

            NOsize = o_NOsize, Wsize = floor(avgWin), NIsize = o_NIsize, Beta = o_Beta,
            TS = o_TS, sim = o_sim, mod = o_mod, C = o_C, ErrorFactor = o_ErrorFactor,
            AnomalyFactor = floor(avgerrFact);

            LoadData(dirPath + "\\" + files[i]); //load dataset

            TraineSNN(); //train eSNN with given set of parameters

            CalculateConfusionMatrix(); //calculate confusion Matrix
            double precisionTest = CalculatePrecision(); //calculate statistics
            double recallTest = CalculateRecall();
            double fMeasureTest = CalculateF_Measure(precisionTest, recallTest);


            cout << "Precision " << precisionTest << " Recall " << recallTest << " fMeasure "
                 << fMeasureTest << endl;
            dataFile_Precision.push_back(precisionTest);
            dataFile_Recall.push_back(recallTest);
            dataFile_FMeasure.push_back(fMeasureTest);

            string resultsFilePath =
                    resultsPath + folder + "_" +
                    files[i];
            SaveResults(resultsFilePath);
            string metricsFilePath =
                    resultsPath  + "metr_" + folder +
                    "_" + files[i];
            SaveMetrics(metricsFilePath, precisionTest,
                        recallTest, fMeasureTest, 0);

            ClearStructures();

        }

        double precisionOverall = CalculateAvgW(dataFile_Precision);
        double recallOverall = CalculateAvgW(dataFile_Recall);
        double fMeasureOverall = CalculateAvgW(dataFile_FMeasure);

        cout << "Precision overall " << precisionOverall << " Recall overall " << recallOverall
             << " fMeasure overall "
             << fMeasureOverall << endl;

        string metricsFilePath = resultsPath  +
                                 "metr_Overall_Optimal_" + folder;
        SaveMetricsOverall(metricsFilePath, precisionOverall, recallOverall,
                           fMeasureOverall);

    }

    return 0;

}
