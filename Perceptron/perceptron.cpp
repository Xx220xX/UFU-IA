//
// Created by hslhe on 18/09/2023.
//

#include "perceptron.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

#define USE_OMP

Perceptron::Perceptron(int nroEntradas, int nroSaidas, REAL_TYPE alpha) : nroEntradas(nroEntradas),
                                                                          nroSaidas(nroSaidas), alpha(alpha) {
    this->w = new REAL_TYPE[nroEntradas * nroSaidas];
    this->y = new REAL_TYPE[nroSaidas];
    this->b = new REAL_TYPE[nroSaidas];
    memset(w, 0, sizeof(REAL_TYPE) * nroSaidas * nroEntradas);
    memset(b, 0, sizeof(REAL_TYPE) * nroSaidas);
}

Perceptron::~Perceptron() {
    delete this->w;
    delete this->y;
    delete this->b;
    w = y = x = t = b = nullptr;
}

void Perceptron::achaSaidas(REAL_TYPE *v_entrada) {
    x = v_entrada;
    REAL_TYPE soma;
#ifdef USE_OMP
#pragma  omp parallel for private(soma)
#endif
    for (int j = 0; j < nroSaidas; ++j) {
        soma = b[j];
        for (int i = 0; i < nroEntradas; ++i) {
            soma += w[i * nroSaidas + j] * x[i];
        }
        y[j] = soma > 0 ? 1. : -1.;
    }
}

void Perceptron::arrumaPesos(REAL_TYPE *v_target) {
    t = v_target;
    int total = nroSaidas * nroEntradas;
#ifdef USE_OMP
#pragma  omp parallel for
#endif
    for (int k = 0; k < total; ++k) {
        int i = k / nroSaidas;
        int j = k % nroSaidas;
        if (y[j] != t[j]) {
            w[k] = w[k] + x[i] * t[j] * alpha;
        }
    }
#ifdef USE_OMP
#pragma  omp parallel for
#endif
    for (int j = 0; j < nroSaidas; ++j) {
        if (y[j] != t[j]) {
            b[j] = b[j] + t[j] * alpha;
        }
    }
}

REAL_TYPE Perceptron::verificaSaida(REAL_TYPE *v_target) {
    REAL_TYPE acerto = 0;
    for (int i = 0; i < nroSaidas; ++i) {
        acerto += (y[i] == v_target[i]);
    }

    return acerto / nroSaidas;
}

#include <Windows.h>

double segundos() {
    FILETIME ft;
    LARGE_INTEGER li;
    GetSystemTimePreciseAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = (LONG) ft.dwHighDateTime;
    unsigned long long int ret = (unsigned long long int) li.QuadPart;
    return (double) ret * 1e-7;
}

std::string fourNeurons2tenClass(REAL_TYPE *output, int outputLength) {
    int x = 0;
    for (int k = 0; k < outputLength; ++k) {
        if (output[k] == 1) {
            x += (1 << k);
        }
    }
    return std::to_string(x);
}

std::string oneNeuronByClass(REAL_TYPE *output, int outputLength) {
    std::string className;
    for (int k = 0; k < outputLength; ++k) {
        if (output[k] == 1) {
            className += "0";
            className[className.length() - 1] += k;
        }
    }
    return className;
}


void DataSet::train(Perceptron *p) {
    if (p->nroEntradas != data2train[0].input.size() || p->nroSaidas > data2train[0].target.size()) {
        std::cerr << "Invalid Size" << std::endl;
    }
    epoch = 0;
    std::vector<Data> *d2test = &data2test;
    if (usetrainAsTeste) {
        d2test = &data2train;
    }
    int data2train_size = data2train.size();
    int data2test_size = d2test->size();
    t0 = segundos();
    double ti;
    REAL_TYPE acertos;
    FILE *log;


    fopen_s(&log, logFile.c_str(), "w");
    if (!log)log = stderr;
    for (int i = 0; i < maxEpoch; ++i) {
        epoch = i;
        int acertos=0;
        REAL_TYPE  R_acertos;
        ti = segundos();
        winRate = 0;
        std::cout << "Epoca " << i + 1 << ": ";
        for (int j = 0; j < data2train_size; ++j) {
            p->achaSaidas(data2train[j].input.data());
            p->arrumaPesos(data2train[j].target.data());
        }
        fprintf(log, "Epoca %d:\n", epoch);
        for (int j = 0; j < data2test_size; ++j) {
            p->achaSaidas(d2test->at(j).input.data());
            fprintf(log, "\t %s is ", d2test->at(j).className.c_str());
            if (funcOut2Class) {
                fprintf(log, "%s", funcOut2Class(p->y, p->nroSaidas).c_str());

            }
            fprintf(log, "\n");
            R_acertos = p->verificaSaida(d2test->at(j).target.data());
            acertos+=(int)R_acertos;
            winRate += R_acertos;
        }

        winRate *= (100. / data2test_size);
        ti = segundos() - ti;
        printf("%.2f%% %d/%d %.4lf s\n", winRate, acertos, data2test_size, ti);
        if (winRate > rateTarget)break;
    }
    trainTime = segundos() - t0;
    if (log != (stderr)) {
        fclose(log);
    }
    printf("Total epocas: %d Win Rate: %.2f%% Tempo total %.4lf s\n", epoch, winRate, trainTime);

}

void DataSet::addDataTrain(Data train) {
    this->data2train.push_back(train);
}

void DataSet::addDataTest(Data test) {
    this->data2test.push_back(test);
}

void DataSet::config(bool usetrainAsTeste, int maxEpochc, float rateTarget, const char *logFile,
                     std::string (*funcOut2Class)(REAL_TYPE *, int)) {
    this->usetrainAsTeste = usetrainAsTeste;
    this->maxEpoch = maxEpochc;
    this->rateTarget = rateTarget;
    this->logFile = logFile;
    this->funcOut2Class = funcOut2Class;
}


