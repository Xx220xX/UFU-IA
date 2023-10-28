
#include "MLP2.h"
#include <random>
#include <omp.h>

ActivationFunc Sigmoid = {
        [](Number &x) { return 1 / (1 + exp(-x)); },
        [](Number &x) { return (Number) (Sigmoid.f(x) * (1.0 - Sigmoid.f(x))); },
};
ActivationFunc Tanh = {
        [](Number &x) { return tanh(x); },
        [](Number &x) { return (Number) (1. / pow(cosh(x), 2)); },
};
ActivationFunc Relu = {
        [](Number &x) { return (Number) (x > 0 ? x : 0.); },
        [](Number &x) { return (Number) (x > 0 ? 1. : 0.); },
};

Number randf(Number a, Number b) {
    static std::default_random_engine generator{(std::mt19937::result_type) time(NULL)};
    std::uniform_real_distribution<Number> distribution(a, b);
    return distribution(generator);
}

int MLP2::addLayer(Size numero_neoronios_saida, Number alpha, ActivationFunc f) {
    if (alpha < 0)
        alpha = defaultAlpha;
    layers.resize(layers.size() + 1);
    Layer2 &l = layers[layers.size() - 1];
    Size nx = nEntrada;
    if (layers.size() > 1) {
        nx = layers[layers.size() - 2].no;
    }
    nSaida = numero_neoronios_saida;
    l._dw.resize((nx + 1) * numero_neoronios_saida);
    l._w.resize((nx + 1) * numero_neoronios_saida);
    l._z.resize(numero_neoronios_saida);
    l._dz.resize(numero_neoronios_saida);
    l._dx.resize(nx);
    l._y.resize(numero_neoronios_saida + 1);
    l._y[numero_neoronios_saida] = 1;
    l.alpha = alpha;
    l.f = f;
    for (int i = l._w.size() - 1; i >= 0; i--) {
        l._w[i] = randf(-0.48, 0.48);
        l._w[i] += l._w[i] > 0 ? 0.02 : -0.02;
    }
    layers[0]._x = &_x;
    for (int i = 1; i < layers.size(); ++i) {
        layers[i]._x = &layers[i - 1]._y;
    }
    return 0;
}

int MLP2::forward() {
    PNumber x, w, z, y;
    Size nlayer = layers.size();
    Size I, O;
#pragma  omp parallel
    {
        for (int l = 0; l < nlayer; ++l) {
#pragma omp master
            {
                I = layers[l]._x->size();
                O = layers[l]._z.size();
                x = layers[l]._x->data();
                y = layers[l]._y.data();
                z = layers[l]._z.data();
                w = layers[l]._w.data();
            }
            int tid = omp_get_thread_num();
            printf("Hello world from omp thread %d\n", tid);
#pragma  omp for
            for (int j = 0; j < O; ++j) {
                Number soma = 0;
                for (int i = 0; i < I; ++i) {
                    soma += x[i] * w[i * O + j];
                }
                z[j] = soma;
                y[j] = layers[l].f.f(soma);
            }

        }
    }
    return 0;
}

Number MLP2::backward(PNumber t) {
    memcpy(reinterpret_cast<void *>(this->_t.size()), t, nSaida * sizeof(Number));
    Size nlayer = layers.size();
    PNumber dy = _dy.data();
    PNumber x, y, dx, dz, z, w, dw;
    Number alpha, erro = 0;
    Size I, O, Ia;
    ActivationFunc *af;
    for (int i = 0; i < nSaida; ++i) {
        dy[i] = _y[i] - t[i];
        erro += dy[i] * dy[i];
    }

#pragma  omp parallel
    {
        for (int l = nlayer - 1; l >= 0; --l) {
#pragma  omp master
            {
                I = layers[l]._x->size();
                Ia = I - 1;
                O = layers[l]._z.size();
                x = layers[l]._x->data(),
                dx = layers[l]._dx.data(),
                z = layers[l]._z.data(),
                dz = layers[l]._dz.data(),
                w = layers[l]._w.data(),
                dw = layers[l]._dw.data();
                af = &layers[l].f;
                alpha = layers[l].alpha;

            }
            // calcular dz
#pragma  omp parallel for
            for (int i = 0; i < O; ++i) {
                dz[i] = af->df(z[i]) * dy[i];
            }
#pragma  omp parallel for
            for (int j = 0; j < O; ++j) {
                for (int i = 0; i < I; ++i) {
                    dw[i * O + j] = dz[j] * x[i];
                }
            }
            if (l > 0) {
#pragma  omp parallel for
                for (int i = 0; i < Ia; ++i) {
                    Number soma = 0;
                    for (int j = 0; j < O; ++j) {
                        soma = dz[j] * w[i];
                    }
                    dx[i] = soma;
                }
            }
#pragma  omp parallel for
            for (Size k = O * I - 1; k >= 0; --k) {
                w[k] = w[k] - alpha * dw[k];
            }
#pragma  omp master
            {
                dy = dx;
            }
        }
    }
    return erro / 2;
}

int MLP2::forward(PNumber x) {
    for (int i = 0; i < nEntrada; ++i) {
        _x[i] = x[i];
    }
//    memcpy((void*)this->_x.size(), x, nEntrada * sizeof(Number));
    return forward();
}

void MLP2::setNInput(Size nentrada) {
    this->nEntrada = nentrada;
    _x.resize(nentrada + 1);
    _x[nentrada] = 1;
}
