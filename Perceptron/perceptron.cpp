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

bool Perceptron::arrumaPesos(REAL_TYPE *v_target) {
    t = v_target;
    bool w_changed = false;
    int total = nroSaidas * nroEntradas;
#ifdef USE_OMP
#pragma  omp parallel for
#endif
    for (int k = 0; k < total; ++k) {
        int i = k / nroSaidas;
        int j = k % nroSaidas;
        if (y[j] != t[j]) {
            w[k] = w[k] + x[i] * t[j] * alpha;
            w_changed = true;
        }
    }
#ifdef USE_OMP
#pragma  omp parallel for
#endif
    for (int j = 0; j < nroSaidas; ++j) {
        if (y[j] != t[j]) {
            b[j] = b[j] + t[j] * alpha;
            w_changed = true;
        }
    }
    return w_changed;
}

bool Perceptron::treinarExemplo(REAL_TYPE *v_entrada, REAL_TYPE *v_saida) {
    x = v_entrada;
    t = v_saida;
    REAL_TYPE soma;
    bool w_changed = false;
#ifdef USE_OMP
#pragma  omp parallel for
#endif
    for (int j = 0; j < nroSaidas; ++j) {
        soma = b[j];
        for (int i = 0; i < nroEntradas; ++i) {
            soma += w[i * nroSaidas + j] * x[i];
        }
        y[j] = soma > 0 ? 1. : -1.;
        if (y[j] != t[j]) {
            w_changed = true;
            for (int i = 0; i < nroEntradas; ++i) {
                w[i * nroSaidas + j] += alpha * x[i] * t[j];
            }
        }

    }
    return w_changed;
}

int Perceptron::verificaSaida(REAL_TYPE *v_target) {
//    REAL_TYPE acerto = 0;
//    for (int i = 0; i < nroSaidas; ++i) {
//        acerto += (y[i] == v_target[i]);
//    }
//
//    return acerto / nroSaidas;
    return !memcmp(y, v_target, nroSaidas * sizeof(REAL_TYPE));
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

std::string Mneurons2Int(REAL_TYPE *output, int outputLength) {
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
            className += std::to_string(k);
        }
    }
    return className;
}

template<typename T>
void fprintVector(FILE *f, const char *nfmt, T *v, int length) {
    fprintf(f, "{");
    if (length > 0) {
        fprintf(f, nfmt, v[0]);
    }
    for (int i = 1; i < length; ++i) {
        fprintf(f, ",");
        fprintf(f, nfmt, v[i]);
    }
    fprintf(f, "}");


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
    FILE *log = nullptr;
    if (!logFile.empty())
        fopen_s(&log, logFile.c_str(), "w");
    for (int i = 0; i < maxEpoch; ++i) {
        epoch = i;
        int acertos = 0;
        REAL_TYPE R_acertos;
        bool w_changed = false;
        ti = segundos();
        winRate = 0;
        std::cout << "Epoca " << i + 1 << ": ";

        for (int j = 0; j < data2train_size; ++j) {
            w_changed |= p->treinarExemplo(data2train[j].input.data(), data2train[j].target.data());
        }
        if (log)
            fprintf(log, "Epoca %d:\n", epoch);
        for (int j = 0; j < data2test_size; ++j) {
            p->achaSaidas(d2test->at(j).input.data());
            if (log && funcOut2Class) {
                fprintf(log, "\t %s is ", d2test->at(j).className.c_str());
                fprintf(log, "%s", funcOut2Class(p->y, p->nroSaidas).c_str());
                fprintVector(log, "%.0lf", p->y, p->nroSaidas);
                fprintVector(log, "%.0lf", d2test->at(j).target.data(), p->nroSaidas);
                fprintf(log, "\n");
            }
            R_acertos = p->verificaSaida(d2test->at(j).target.data());
            acertos += (int) R_acertos;
            winRate += R_acertos;
        }

        winRate *= (100. / data2test_size);
        ti = segundos() - ti;
        printf("%.2f%% %d/%d %.4lf s\n", winRate, acertos, data2test_size, ti);
        if (winRate > rateTarget || !w_changed)break;

    }
    trainTime = segundos() - t0;
    if (log) {
        fclose(log);
    }
    printf("Total epocas: %d Win Rate: %.2f%% Tempo total %.4lf s\n", epoch + 1, winRate, trainTime);

}

void DataSet::addDataTrain(Data train) {
    this->data2train.push_back(train);
}

void DataSet::addDataTest(Data test) {
    this->data2test.push_back(test);
}

void DataSet::config(bool usetrainAsTeste, int maxEpochc, float rateTarget, const char *logFile,std::string (*funcOut2Class)(REAL_TYPE *, int)) {
    this->usetrainAsTeste = usetrainAsTeste;
    this->maxEpoch = maxEpochc;
    this->rateTarget = rateTarget;
    if (logFile) {
        this->logFile = logFile;
    } else {
        this->logFile = "";
    }
    this->funcOut2Class = funcOut2Class;
}


#define PRT(x)std::cout<<#x ": "<< x <<std::endl;

uint32_t read_int_big2little(FILE *f) {
    uint32_t num;
    fread(&num, 4, 1, f);
    return ((num >> 24) & 0xFF) |
           ((num >> 8) & 0xFF00) |
           ((num << 8) & 0xFF0000) |
           ((num << 24) & 0xFF000000);
}

uint32_t read_int_little(FILE *f) {
    uint32_t num;
    fread(&num, 4, 1, f);
    return num;
}

int DataSet::loadData(const char *imageFile, const char *labelFile, std::vector<Data> &v_data, bool use_binary) {
#define MNIST_MAGIC_NUMBER_LITTLE  0x03080000
#define MNIST_MAGIC_NUMBER_BIGENDIAN  0x00000803
#define MNIST_LABEL_MAGIC_NUMBER_LITTLE 0x01080000
#define MNIST_LABEL_MAGIC_NUMBER_BIGENDIAN 0x00000801

    FILE *fim, *flb;
    uint32_t (*read_int32)(FILE *);
    uint32_t mnst_length;
    int mnst_w;
    int mnst_h;
    int mnst_im_size;

    uint32_t label_length;


    fopen_s(&fim, imageFile, "rb");
    if (!fim)return -1;
    fopen_s(&flb, labelFile, "rb");
    if (!flb) {
        fclose(fim);
        return -1;
    }

    read_int32 = read_int_little;
    mnst_length = read_int_little(fim);

    if (mnst_length == MNIST_MAGIC_NUMBER_LITTLE) {
        read_int32 = read_int_big2little;
    } else if (mnst_length != MNIST_MAGIC_NUMBER_BIGENDIAN) {
        return -2;
    }


    label_length = read_int_little(flb);
    if (label_length == MNIST_LABEL_MAGIC_NUMBER_LITTLE) {
        read_int32 = read_int_big2little;
    } else if (label_length != MNIST_LABEL_MAGIC_NUMBER_BIGENDIAN) {
        return -2;
    }
    mnst_length = read_int32(fim);
    label_length = read_int32(flb);
    if (mnst_length != label_length) {
        return -3;
    }
    std::cout << "Ready: " << label_length << std::endl;
    mnst_w = read_int32(fim);
    mnst_h = read_int32(fim);
    mnst_im_size = mnst_h * mnst_w;
    char className[] = "0";
    char label;
    unsigned char *im;
    im = new unsigned char[mnst_h * mnst_w];
    for (int n = 0; n < mnst_length; ++n) {
        fread(&label, 1, 1, flb);
        className[0] = '0' + label;
        v_data.push_back(
                Data(className, std::vector<REAL_TYPE>(mnst_im_size), std::vector<REAL_TYPE>(use_binary ? 4 : 10)));
        Data &dt = v_data[v_data.size() - 1];
        fread(im, 1, mnst_im_size, fim);
        for (int i = 0; i < mnst_h; ++i) {
            dt.input[i] = im[i] ? 1 : -1;
        }
        if (use_binary) {
            dt.target[0] = label & 0b0001;
            dt.target[1] = label & 0b0010;
            dt.target[2] = label & 0b0100;
            dt.target[3] = label & 0b1000;
        } else {
//            if(label==0){
//                PRT(label)
//            }
            for (unsigned char i = 0; i < 10; ++i) {
                dt.target[i] = (label == i) ? 1 : -1;
            }
        }
    }

    delete im;
    fclose(flb);
    fclose(fim);
    return 0;
}


