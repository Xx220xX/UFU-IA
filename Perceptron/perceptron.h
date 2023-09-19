//
// Created by hslhe on 18/09/2023.
//

#ifndef CREST_PERCEPTRON_H
#define CREST_PERCEPTRON_H
typedef double REAL_TYPE;


class Perceptron {
public:
    REAL_TYPE *w = nullptr, *y = nullptr, *x = nullptr, *t = nullptr, *b = nullptr, alpha = 0;
    int nroEntradas = 0, nroSaidas = 0;

    Perceptron() = default;

    Perceptron(int nroEntradas, int nroSaidas, REAL_TYPE alpha);

    ~Perceptron();

    void achaSaidas(REAL_TYPE *v_entrada);

    void arrumaPesos(REAL_TYPE *v_target);
};

double segundos();

#endif //CREST_PERCEPTRON_H
