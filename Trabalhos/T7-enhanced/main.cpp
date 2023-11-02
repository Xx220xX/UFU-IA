
#include "src/MLP.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <unistd.h> // Para a função access
#include <string>
#include <sys/stat.h>// stat
#include "mininst.h"

using namespace MLP;
using namespace std;


bool fileExists(const std::string &filename) {
    return (access(filename.c_str(), F_OK) != -1);
}
int pastaExiste(const char *caminho) {
    struct stat info;
    if (stat(caminho, &info) != 0) {
        // Falha ao obter informações do arquivo/diretório
        return 0;
    }
    if (S_ISDIR(info.st_mode)) {
        // É um diretório
        return 1;
    }
    // Não é um diretório
    return 0;
}

int main(int nargs, char **args) {
    Mlp mlp;
    vector<Sample> teste;
    vector<Sample> treino;
    Number erro;
    Number acertos = 0;
    int label;
    vector<vector<Number>> confusao(10);
    Number lastAcerto = 0;
    string filesave;
    ofstream fmlp;
    ofstream fstt;
    // alpha arg [1], n1, n2, n3, n4 , n5 ...
    string path = "";
    if (!fileExists(path + "dataset/train-images.idx3-ubyte")) {
        path = "../";
        if (!fileExists(path + "dataset/train-images.idx3-ubyte")) {
            path = "../../";
            if (!fileExists(path + "dataset/train-images.idx3-ubyte")) {
                cerr << "Imagens não encontradas\n";
                exit(-1);
            }
        }

    }



    int e = loadData(path, "dataset/train-images.idx3-ubyte", "dataset/train-labels.idx1-ubyte", treino, false, -1);
    if (e < 0)exit(e);
    e = loadData(path, "dataset/t10k-images.idx3-ubyte", "dataset/t10k-labels.idx1-ubyte", teste, false, -1);
    if (e < 0)exit(e);

    mlp.Input(treino[0].x.size());
    if (nargs > 1) {
        Number alpha = atof(args[1]);
        mlp.setDefaultAlpha(alpha);
        for (int i = 2; i < nargs; ++i) {
            int n = atoi(args[i]);
            mlp.addLayer(n);
        }
    } else {
        mlp.setDefaultAlpha(0.01);
        mlp.addLayer(20);
        mlp.addLayer(20);
        mlp.addLayer(20);
    }
    mlp.addLayer(treino[0].y.size());
    if(!pastaExiste("./treinadas/")){
        mkdir("./treinadas/",0777);
    }
    cout<<mlp<<endl;

    return 0 ;

    filesave = "./treinadas/mlp_" + to_string(mlp.default_alpha) + "(";
    filesave += to_string(mlp.in);
    for (int l = 0; l < mlp.layers.size(); ++l) {
        filesave += "_" + to_string(mlp.layers[l].ot) + mlp.layers[l].f.name;
    }
    filesave += ")";
    {
        int i = 0;
        while (fileExists(filesave + to_string(i) + ".json")) {
            i++;
        }
        filesave += to_string(i);
    }
    fstt.open(filesave + ".txt");

    for (auto &l: confusao) {
        l.resize(10);
    }

    for (int epoca = 0; epoca < 100; ++epoca) {
        erro = 0;
        for (auto &v: confusao) {
            std::fill(v.begin(), v.end(), 0);
        }
        acertos = 0;
        fstt << "Epoca: " << epoca << endl;
        cout << "Epoca: " << epoca << " ";
        for (auto &s: treino) {
            erro += mlp.treinar(s);
        }

        for (auto &s: teste) {
            label = mlp.prever(s.x.data());
            acertos += label == s.valueName;
            confusao[s.valueName][label] += 1;
        }
        erro /= treino.size();
        fstt << "Erro: " << erro << endl;
        cout << "Erro: " << erro << " ";
        acertos *= 100. / teste.size();
        fstt << "acertos: " << acertos << endl;
        cout << "acertos: " << acertos << endl;
        if (lastAcerto > acertos || acertos == 100)break;
        lastAcerto = acertos;
        for (int j = 0; j < 10; ++j) {
            fstt << j << ": " << confusao[j] << "\n";
        }
        fmlp.open(filesave + ".json");
        fmlp << mlp;
        fmlp.close();

        fstt << endl;

    }
    fstt.close();
    fmlp.close();
    return 0;
}

