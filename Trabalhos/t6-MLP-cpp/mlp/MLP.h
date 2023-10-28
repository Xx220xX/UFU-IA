
#ifndef T6_MLP_CPP_MLP_H
#define T6_MLP_CPP_MLP_H

#include <iostream>
#include <vector>
#include "config.h"

struct ActivationFunc {
    Number (*f)(Number &x);

    Number (*df)(Number &x);
};


extern ActivationFunc Sigmoid;
extern ActivationFunc Tanh;
extern ActivationFunc Tan;
extern ActivationFunc Relu;


class Layer {
public:
    Size nInput, nOutput;
    Number alpha;
    ActivationFunc &func;
    bool isInit;

    Layer(Size nInput, Size nOutput, Number alpha, ActivationFunc &func, bool isInit = false);

    std::vector<Number> *x{}, w, z, y, dz, dw, dx;

    int forward();

    int backward(std::vector<Number> &dy);

    friend std::ostream &operator<<(std::ostream &os, const Layer &layer);

    void setInput(std::vector<Number> *x);
};

class MLP {
public:
    std::vector<Layer> layers;
    Number *x, *y, defaultAlpha = 0.01;
    std::vector<Number> dy;
    std::vector<Number> t;
    std::vector<Number> mem_x;
    Size nInput, nOutput;

    int addLayer(Size _nOutput, ActivationFunc &_func);

    int addLayer(Size _nOutput, Number _alpha = 0.01, ActivationFunc &_func = Sigmoid);

    int forward(Number *input);

    int forward();

    Number backward(Number *target);

    Number backward();

    friend std::ostream &operator<<(std::ostream &os, const MLP &mlp);

    void setNInput(Size _nInput);
};

namespace std {
    template<typename T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> &vetor) {
        os << "[";
        if (!vetor.empty())
            os << vetor[0];
        for (int i = 1; i < vetor.size(); ++i) {
            os << ", " << vetor[i];
        }
        os << "]";
        return os;
    }

}

#endif //T6_MLP_CPP_MLP_H
