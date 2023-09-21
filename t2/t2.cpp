#include <iostream>
#include "Perceptron/perceptron.h"

using namespace std;


int main(int nargs, char **args) {
    setlocale(LC_CTYPE, "pt-BR.utf-8");
    // primeiro caso
    cout << "Primeiro caso, um único neurônio." << endl;
    DataSet dataSet = {100};
    dataSet.addDataTrain(
            Data({"X", {1, -1, -1, -1, 1, -1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1, 1, -1, 1, -1, 1, -1, -1, -1, 1},
                  {1, -1}}));
    dataSet.addDataTrain(
            Data({"T", {1, 1, 1, 1, 1, -1, -1, 1, -1, -1, -1, -1, 1, -1, -1, -1, -1, 1, -1, -1, -1, -1, 1, -1, -1},
                  {-1, 1}}));
    Perceptron *p = new Perceptron(25, 2, 0.1);
    dataSet.config(true, 100, 95, "T2_2neuronio.txt",
                   [](REAL_TYPE *y, int len) -> std::string {
                       std::string r;
                       if (((int) y[0]) == 1)
                           r += "X";
                       if (((int) y[1]) == 1)r += "T";
                       return r;
                   });
    dataSet.train(p);

    delete p;
    cout << endl << "Segundo caso, dois neurônios." << endl;
    p = new Perceptron(25, 1, 0.1);
    dataSet.config(true, 100, 95, "T2_1neuronio.txt",
                   [](REAL_TYPE *y, int len) -> std::string {
                       return ((int) y[0]) == 1. ? "X" : "T";
                   });

    dataSet.train(p);
    delete p;
//    system("pause");
    return 0;
}