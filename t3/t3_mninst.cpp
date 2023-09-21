#include <iostream>
#include "Perceptron/perceptron.h"

using namespace std;

struct MININST {
    int32_t length;
    int16_t w;
    int16_t h;

};
#define USE_BINARY 0
#define LOG 0
#define EPOCAS 100
#define TARGET 95
int main(int nargs, char **args) {
    setlocale(LC_CTYPE, "pt-BR.utf-8");

    REAL_TYPE alpha = 0.001;
    // primeiro caso
#if USE_BINARY == 1
    cout << "Segundo caso, um 4 neurônios." << endl;
#else
    cout << "Primeiro caso, um 10 neurônios." << endl;
#endif
    DataSet dataSet = {};
    int erro;
    erro = dataSet.loadData("dataset/train-images.idx3-ubyte", "dataset/train-labels.idx1-ubyte", dataSet.data2train,
#if USE_BINARY == 1
    true
#else
                            false
#endif
                            );
    cout << "load train:" << erro << endl;
    erro = dataSet.loadData("dataset/t10k-images.idx3-ubyte", "dataset/t10k-labels.idx1-ubyte", dataSet.data2test,false);
    cout << "load test:" << erro << endl;
    Perceptron *p;
    p = new Perceptron(dataSet.data2train[0].input.size(), dataSet.data2train[0].target.size(), alpha);
//    dataSet.data2train.resize(1000);
//    dataSet.data2test.resize(100);
    dataSet.config(false,EPOCAS,TARGET,
#if LOG == 1
#if USE_BINARY == 1
            "MNIST-4neuronios.txt"
                   ,
                   fourNeurons2tenClass);
#else
                   "MNIST-10neuronios.txt"
                   ,oneNeuronByClass);
#endif
#else
                   nullptr, nullptr);
#endif;
    dataSet.train(p);

    delete p;
    system("pause");
    return 0;
}