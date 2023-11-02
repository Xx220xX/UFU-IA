//
// Created by henrique on 28/10/23.
//

#include "MLP.h"
#include <random>
#include <iostream>
#include <stdlib.h>

#define USE_OMP_ 1
std::random_device rd;  // Semente do dispositivo aleatório
unsigned long sd = rd();
std::mt19937 generator(sd);
std::uniform_real_distribution<Number> distribution(-1, 1.);

void MLP::randomize(VN &w, Number max_v) {

    Sz lenw = w.size();
    for (Sz i = 0; i < lenw; ++i) {
        w[i] = distribution(generator) * max_v;
    }
}

MLP::Layer::Layer(Sz in, Sz ot, Number alpha, Func &f) : f(f) {
    this->in = in;
    this->inb = in + 1;
    this->ot = ot;
    this->alpha = alpha;
    Y.resize(ot + 1);
    Y[ot] = 1;
    Z.resize(ot);
    DZ.resize(ot);
    W.resize(inb * ot);
    DW.resize(inb * ot);
    DX.resize(in);
}

int MLP::Mlp::Input(Sz in) {
    if (this->in != -1)throw invalid_argument("Entrada já modificada");
    if (in <= 0)throw invalid_argument("Entrada deve ser maior que 0");
    this->in = in;
    lout = in;
    _x.resize(in + 1);
    _x[in] = 1;
    return 0;
}

int MLP::Mlp::setDefaultAlpha(Number alpha) {
    default_alpha = alpha;
    return 0;
}

int MLP::Mlp::addLayer(Sz o, Number alpha, Func &f) {
    layers.push_back(std::move(Layer(lout, o, alpha, f)));
    _dy.resize(o);
    lout = o;
    for (int i = 1; i < layers.size(); ++i) {
        layers[i].X = &layers[i - 1].Y;
    }
    layers[0].X = &_x;
    Layer &last = layers.back();
    y = last.Y.data();
    randomize(last.W,1./last.inb);
    return 0;
}

Number MLP::Mlp::treinar(const Sample &s) {
    return treinar(s.x.data(), s.y.data());
}

Number MLP::Mlp::treinar(const Number *input, const Number *t) {
    Number erro = 0;
    _x.assign(input, input + in);
    int L = layers.size();
#if USE_OMP_ == 1
#pragma  omp parallel shared(L, erro, t) default(none)
#endif
    {
        PNumber w, z, x, y, dx, dw, dy, dz;
        Number alpha;
        Sz I, IB, O, LW;
        Func f;

        for (int l = 0; l < L; ++l) {
            w = layers[l].W.data();
            z = layers[l].Z.data();
            x = layers[l].X->data();
            y = layers[l].Y.data();
            f = layers[l].f;
            IB = layers[l].inb;
            O = layers[l].ot;
#if USE_OMP_ == 1
#pragma omp for
#endif
            for (int j = 0; j < O; ++j) {
                Number soma = 0;
                for (int i = 0; i < IB; ++i) {
                    soma += w[i * O + j] * x[i];
                }
                z[j] = soma;// z = w*x
                y[j] = f.f(soma);// y = f(z)
            }
        }

        dy = _dy.data();
        O = layers[L - 1].ot;

#if USE_OMP_ == 1
#pragma  omp master
#endif
        {
            erro = 0;
            for (int i = 0; i < O; ++i) {
                dy[i] = y[i] - t[i];
                erro += dy[i] * dy[i];
            }
        }
#if USE_OMP_ == 1
#pragma omp barrier
#endif
        for (int l = L - 1; l >= 0; --l) {
            w = layers[l].W.data();
            z = layers[l].Z.data();
            dw = layers[l].DW.data();
            dz = layers[l].DZ.data();
            dx = layers[l].DX.data();
            x = layers[l].X->data();
            f = layers[l].f;
            I = layers[l].in;
            IB = layers[l].inb;
            O = layers[l].ot;
            LW = IB * O;
            alpha = layers[l].alpha;

#if USE_OMP_ == 1
#pragma omp for
#endif
            for (int j = 0; j < O; ++j) {
                dz[j] = f.df(z[j]) * dy[j];
            }
            if (l > 0) {
#if USE_OMP_ == 1
#pragma omp for
#endif
                for (int i = 0; i < I; ++i) {
                    Number soma = 0;
                    for (int j = 0; j < O; ++j) {
                        soma += w[i * O + j] * dz[j];
                    }
                    dx[i] = soma;
                }
            }
#if USE_OMP_ == 1
#pragma omp for
#endif
            for (Sz k = 0; k < LW; ++k) {
                int i = k / O;
                int j = k % O;
                dw[k] = x[i] * dz[j];
                w[k] -= alpha * dw[k];
            }
            dy = layers[l].DX.data();
        }
    }
    return erro / (2. * lout);
}

int MLP::Mlp::prever(const Number *input) {
    int index = 0;
    Number max_value = -INFINITY;

    _x.assign(input, input + in);

    int L = layers.size();
#if USE_OMP_ == 1
#pragma  omp parallel shared(L, max_value, index) default(none)
#endif
    {
        PNumber w, x, y;
        Number alpha;
        Sz I, IB, O, LW;
        Func f;
        for (int l = 0; l < L; ++l) {
            w = layers[l].W.data();
            x = layers[l].X->data();
            y = layers[l].Y.data();
            f = layers[l].f;
            IB = layers[l].inb;
            O = layers[l].ot;
#if USE_OMP_ == 1
#pragma omp for
#endif
            for (int j = 0; j < O; ++j) {
                Number soma = 0;
                for (int i = 0; i < IB; ++i) {
                    soma += w[i * O + j] * x[i];
                }
                y[j] = f.f(soma);
            }
        }
    }

    return maxi(layers.back().Y.data(), lout);
}


int MLP::Mlp::addLayer(Sz o, Number alpha) {
    return addLayer(o, alpha, Tanh);
}

int MLP::Mlp::addLayer(Sz o, Func &f) {
    return addLayer(o, default_alpha, Tanh);
}

int MLP::maxi(PNumber v, int len) {
    int index = 0;
    for (int i = 1; i < len; ++i) {
        if (v[i] > v[index])index = i;
    }
    return index;
}

std::string EncodeVectorToBase64(const VN & data) {
    const char* encodingTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string base64String;
    size_t dataLength = data.size() * sizeof(float);
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data.data());

    for (size_t i = 0; i < dataLength; i += 3) {
        uint32_t group = (bytes[i] << 16) | (bytes[i + 1] << 8) | bytes[i + 2];

        base64String += encodingTable[(group >> 18) & 0x3F];
        base64String += encodingTable[(group >> 12) & 0x3F];
        base64String += encodingTable[(group >> 6) & 0x3F];
        base64String += encodingTable[group & 0x3F];
    }

    // Preencha os caracteres de preenchimento se necessário
    if (dataLength % 3 == 1) {
        base64String[base64String.length() - 1] = '=';
        base64String[base64String.length() - 2] = '=';
    } else if (dataLength % 3 == 2) {
        base64String[base64String.length() - 1] = '=';
    }

    return base64String;
}