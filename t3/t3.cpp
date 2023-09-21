#include <iostream>
#include "Perceptron/perceptron.h"

using namespace std;


int main(int nargs, char **args) {
    setlocale(LC_CTYPE,"pt-BR.utf-8");

    REAL_TYPE  alpha = 0.1;
    // primeiro caso
    cout<<"Primeiro caso, um 10 neurônios."<<endl;
    Perceptron *p = new Perceptron(20, 10, alpha);
    DataSet dataSet= {};

    dataSet.addDataTrain(Data({"0",{-1,1,1,-1,1,-1,-1,1,1,-1,-1,1,1,-1,-1,1,-1,1,1,-1},{1,-1,-1,-1,-1,-1,-1,-1,-1,-1}}));
    dataSet.addDataTrain(Data({"1",{-1,1,-1,-1,1,1,-1,-1,-1,1,-1,-1,-1,1,-1,-1,1,1,1,-1},{-1,1,-1,-1,-1,-1,-1,-1,-1,-1}}));
    dataSet.addDataTrain(Data({"2",{-1,1,1,-1,1,-1,-1,1,-1,-1,1,-1,-1,1,-1,-1,1,1,1,1},{-1,-1,1,-1,-1,-1,-1,-1,-1,-1}}));
    dataSet.addDataTrain(Data({"3",{1,1,1,-1,-1,-1,-1,1,-1,-1,1,-1,-1,-1,-1,1,1,1,1,-1},{-1,-1,-1,1,-1,-1,-1,-1,-1,-1}}));
    dataSet.addDataTrain(Data({"4",{1,-1,1,-1,1,-1,1,-1,1,1,1,-1,-1,-1,1,-1,-1,-1,1,-1},{-1,-1,-1,-1,1,-1,-1,-1,-1,-1}}));
    dataSet.addDataTrain(Data({"5",{1,1,1,1,1,-1,-1,-1,1,1,1,-1,-1,-1,-1,1,1,1,1,-1},{-1,-1,-1,-1,-1,1,-1,-1,-1,-1}}));
    dataSet.addDataTrain(Data({"6",{-1,1,1,1,1,-1,-1,-1,1,1,1,-1,1,-1,-1,1,-1,1,1,-1},{-1,-1,-1,-1,-1,-1,1,-1,-1,-1}}));
    dataSet.addDataTrain(Data({"7",{1,1,1,1,-1,-1,-1,1,-1,-1,1,-1,-1,1,-1,-1,-1,1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,1,-1,-1}}));
    dataSet.addDataTrain(Data({"8",{-1,1,1,-1,1,-1,-1,1,-1,1,1,-1,1,-1,-1,1,-1,1,1,-1},{-1,-1,-1,-1,-1,-1,-1,-1,1,-1}}));
    dataSet.addDataTrain(Data({"9",{-1,1,1,-1,1,-1,-1,1,-1,1,1,1,-1,-1,-1,1,1,1,1,1},{-1,-1,-1,-1,-1,-1,-1,-1,-1,1}}));
    dataSet.config(true,100,95,"Numeros_0-9_10-neuronios.txt",oneNeuronByClass);
    dataSet.train(p);
    delete p;
    p = new Perceptron(20, 4, alpha);
    cout<<"Segundo caso, 4 neurônios."<<endl;
    for (int i = 0; i < 10; ++i) {
        std::vector<REAL_TYPE> target = {-1,-1,-1,-1};
        dataSet.data2train[i].target[0] = (i&0b0001)?1:-1;
        dataSet.data2train[i].target[1] = (i&0b0010)?1:-1;
        dataSet.data2train[i].target[2] = (i&0b0100)?1:-1;
        dataSet.data2train[i].target[3] = (i&0b1000)?1:-1;
    }
    dataSet.config(true, 100, 95, "Numeros_0-9_4-neuronios.txt", Mneurons2Int);
    dataSet.train(p);
    delete p;

//    system("pause");
    return 0;
}