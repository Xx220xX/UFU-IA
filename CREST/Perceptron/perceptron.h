//
// Created by hslhe on 18/09/2023.
//

#ifndef CREST_PERCEPTRON_H
#define CREST_PERCEPTRON_H

#include <vector>
#include <string>

typedef double REAL_TYPE;

class Perceptron {
public:
    REAL_TYPE *w = nullptr, *y = nullptr, *x = nullptr, *t = nullptr, *b = nullptr, alpha = 0;
    int nroEntradas = 0, nroSaidas = 0;

    Perceptron() = default;

    Perceptron(int nroEntradas, int nroSaidas, REAL_TYPE alpha);

    ~Perceptron();

    void achaSaidas(REAL_TYPE *v_entrada);

    bool arrumaPesos(REAL_TYPE *v_target);

    bool treinarExemplo(REAL_TYPE *v_entrada, REAL_TYPE *v_saida);

    int verificaSaida(REAL_TYPE *v_target);

};

double segundos();

struct Data {
    std::string className;
    std::vector<REAL_TYPE> input;
    std::vector<REAL_TYPE> target;
};

struct DataSet {
    int maxEpoch{};
    std::vector<Data> data2train;
    std::vector<Data> data2test;
    std::string logFile;

    void addDataTrain(Data train);

    void addDataTest(Data test);

    float rateTarget{};

    double t0{};
    REAL_TYPE winRate{};
    int epoch{};
    double trainTime{};

    void train(Perceptron *p);

    void config(bool usetrainAsTeste, int maxEpochc, float rateTarget, const char *log_file,
                std::string (*funcOut2Class)(REAL_TYPE *, int));

    bool usetrainAsTeste = 0;

    std::string (*funcOut2Class)(REAL_TYPE *, int);

    int loadData(const char *imageFile, const char *labelFile, std::vector<Data> &v_data, bool use_binary);
};

std::string oneNeuronByClass(REAL_TYPE *output, int outputLength);

std::string Mneurons2Int(REAL_TYPE *output, int outputLength);
typedef  std::string(*M2N)(REAL_TYPE *output, int outputLength);


#endif //CREST_PERCEPTRON_H
