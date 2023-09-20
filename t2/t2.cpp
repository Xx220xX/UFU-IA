#include <iostream>
#include "Perceptron/perceptron.h"

using namespace std;


int main(int nargs, char **args) {
    setlocale(LC_CTYPE,"pt-BR.utf-8");
    // primeiro caso
    cout<<"Primeiro caso, um único neurônio."<<endl;
    Perceptron *p = new Perceptron(25, 2, 0.1);
    DataSet dataSet= {100};
    dataSet.addDataTrain(Data({"X",{1, -1, -1, -1, 1,-1, 1,  -1, 1,  -1,-1, -1, 1, -1, -1,-1, 1,  -1, 1,  -1,1,  -1, -1, -1, 1},{1,-1}}));
    dataSet.addDataTrain(Data({"T",{1, 1,  1,  1,  1,-1, -1, 1,  -1, -1,-1, -1, 1, -1, -1,-1, -1, 1,  -1, -1,-1, -1, 1,  -1, -1},{-1,1}}));
    dataSet.config(true,100,95);
    dataSet.train(p);

    delete p;
    cout<<endl<<"Segundo caso, dois neurônios."<<endl;
    p = new Perceptron(25, 1, 0.1);
    dataSet.train(p);
    delete p;
    system("pause");
    return 0;
}