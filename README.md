# eSNN-AD

## Source code files

### eSNN.h

Contains declarations of variables, structures and functions of the proposed eSNN architecture. Imported by main.cpp. No changes should be made in this file.

### eSNN.cpp

Definitions of functions and variables used by the proposed eSNN architecture. In particular, *TraineSNN()* is the main function called in *main.cpp* file.

### main.cpp

Main file of the project. To produce anomaly classification results on your data files or data file in anomaly detection benchmarks you need change paths at the begining of *main* function. 


## Input Data Format

Each input data file should contain three columns: the first one being series of timestamps, the second one being real input values to be classified and the third one should contain labels denoting presence or absence of anomaly for each input value. The presence of anomaly should be indicated by 1, while absence by 0. The types of these three columns should be as follows:
  * timestamp: string,
  * input value: real (double, float),
  * anomaly label: Boolean (1 or 0);
  
Input data files should not contain column headers. Rows in data files should not be numbered. 

Generally you should include your data files in *Dataset* folder and results will be produced to *Results* folder. Please note that the file listing *ReadDirectory(const std::string &name, vector<string> &v)* procedure given in *main.cpp* is written for Windows. In a case of other platforms you need to replace it with your own procedure for listing all files in a directory.

## Results Data Files

The proposed eSNN architecture for each input data file will produce two results files: the first one being csv file, which contain the same three columns as input data file and also predicted value *y* of each input value, error between input value and predicted value, the age of the neuron responsible for providing output value *y* and classification of input value as anomaly or not. The second produced results file starts with prefix *metr_* and contains optimal learning parameters of eSNN for the respective input data file.

The results are produced to folder *Results*.

## Results for Numenta Anomaly Benchmark and Yahoo Anomaly Dataset

To reproduce the results reported in our paper on Numenta and Yahoo benchmarks you need to include input data files of these two benchmarks in folders *Dataset/numenta* or *Dataset/Yahoo*, respectively. Each data file should contain three time series: timestamp, input value and anomaly label. Please assure that column headers are not provided in data files.

For example, data files of *realTweets* category of Numenta benchmark should be contained in the folder:
  * *Datasets/numenta/realTweets*

For Numenta Anomaly Benchmark it is neccessary to include anomaly labels of input values in each data file as originally only timestamps and input values time series are provided in data files. To do that you can either copy anomaly labels column from one of the results file of the anomaly detection algorithms implemented in Numenta benchmark or use file *combined_labels.json* contained in *labels* folder of Numenta benchmark. 

For Yahoo Anomaly Dataset all columns besides timestamp, input value and anomaly label should be removed from data files.  

The eSNN prameters for grid search optimization on Numenta and Yahoo datasets are given in text files *Numenta_opt_parameters* and *Yahoo_opt_parameters*, respectively. 

The results should be included in the *Results* folder. Please remember to appropriately adjust your folder paths at the begining of *main* function in the *main.cpp* file.

## Contact

For questions contact Piotr Maciąg: piotr.maciag@pw.edu.pl
