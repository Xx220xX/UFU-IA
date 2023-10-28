#include <iostream>
#include "mininst.h"

#include <io.h>
#include <string>

bool fileExists(const char *filename) {
    return _access(filename, 0) != -1;
}


#define USE_BINARY 0
#define LOG 0
#define EPOCAS 100
#define TARGET 95
struct INFO {
    Number result[10];
};

int main(int nargs, char **args) {
    setlocale(LC_CTYPE, "pt-BR.utf-8");
    Number alpha = 0.001;
    vector<Data> treino;
    vector<Data> teste;
    MLP mlp;
    mlp.defaultAlpha = 0.01;
    mlp.setNInput(28 * 28);
    mlp.addLayer(1000);
    mlp.addLayer(1000);
    mlp.addLayer(10);
    mlp.addLayer(USE_BINARY ? 4 : 10);

    loadData("dataset/t10k-images.idx3-ubyte", "dataset/t10k-labels.idx1-ubyte", teste, USE_BINARY);
    loadData("dataset/train-images.idx3-ubyte", "dataset/train-labels.idx1-ubyte", treino, USE_BINARY);

    treino.resize(6000);
    teste.resize(1000);
    FILE *f;
    string filename = "mininst_" + to_string(USE_BINARY) + "_(";

    filename += to_string(mlp.nInput);
    for (auto &layer: mlp.layers) {
        filename += "-" + to_string(layer.nOutput);
    }
    filename += ")_";
    int indice = 0;

    while (fileExists((filename + to_string(indice) + ".json").c_str())) {
        indice++;
    }
    filename += to_string(indice) + ".json";
    cout << filename << "\n";
    int sz_treino = treino.size();
    int sz_teste = teste.size();
    vector<Number> mse(EPOCAS);
    vector<INFO> estatisticas(10);
    int ddpc = sz_treino / 100;
    for (int ep = 0; ep < EPOCAS; ++ep) {
        Number erro = 0;
        cout << "Epoca " << ep << "\n";
        for (int i = 0; i < sz_treino; ++i) {
            if (i % ddpc == 0)
                printf("\r%.2lf%%   ", i * 100.0 / sz_treino);
            mlp.forward(treino[i].entrada.data());
            erro += mlp.backward(treino[i].saida.data());
        }
        erro /= sz_treino;
        mse[ep] = erro;
        cout << "Erro " << erro << "\n";
        for (int k = 0; k < 10; ++k) {
            for (int j = 0; j < mlp.nOutput; ++j) {
                estatisticas[k].result[j] = 0;
            }
        }
        for (int i = 0; i < sz_teste; ++i) {
            mlp.forward(teste[i].entrada.data());
            Number mx = mlp.y[0];
            for (int j = 1; j < mlp.nOutput; ++j) {
                if (mx < mlp.y[i])mx = mlp.y[0];
            }
            if (mx == 0)mx = 1;
            for (int j = 0; j < mlp.nOutput; ++j) {
                estatisticas[teste[i].valueName].result[j] += mlp.y[j] / mx;
            }
        }

        for (int i = 0; i < 10; ++i) {
            cout << i << ": ";
            Number mx = estatisticas[i].result[0];
            for (int j = 1; j < mlp.nOutput; ++j) {
                if (mx < estatisticas[i].result[0])mx = estatisticas[i].result[0];
            }
            if (mx < mlp.y[i])mx = mlp.y[0];
            for (int j = 0; j < mlp.nOutput; ++j) {
                cout << estatisticas[i].result[j] / mx << " ";
            }
            cout << "\n";
        }
        fflush(stdout);
    }

//    fopen_s(&f,filename.c_str(),"w");

    system("pause");
    return 0;
}