# eSNN-AD

## Source code files

### eSNN.h

Contains declarations of variables, structures and functions of the proposed eSNN architecture. Imported by main.cpp. No changes should be made in this file.

### eSNN.cpp

Definitions of functions and variables used by the proposed eSNN architecture. 

### main.cpp

Main file of the project. 




## Input Data Format

The input file should contain three columns: the first one bing series of timestamps, the second one input values to be classified and the third one should contain...


## Results for Numenta Anomaly Benchmark and Yahoo Anomaly Dataset

To reproduce the results reported in our paper you need to include data files grouped into categories in the folder Dataset/Numenta or Dataset/Yahoo. Each data file should contain three time series: timestamp, input value and anomaly label. For Numenta Anomaly Benchmark it is neccessary to include anomaly labels of input values in each data file as originally only timestamps and input values time series are provided in each data file. To do that you can either copy labels from one of the results file 

For Yahoo Anomaly Dataset all columns besides timestamp, input value and anomaly label should be removed from data files.  

The eSNN prameters grid search optimization for Numenta and Yahoo datasets are given in text files Numenta_opt_parameters and Yahoo_opt_parameters, respectively. 

The results should be included in the Results folder. 

## Contact

For questions contact Piotr Maciąg: piotr.maciag@pw.edu.pl
