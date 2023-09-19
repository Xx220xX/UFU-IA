//
// Created by hslhe on 18/09/2023.
//

#include "perceptron.h"
#include <cstdlib>
#include <cstring>

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
