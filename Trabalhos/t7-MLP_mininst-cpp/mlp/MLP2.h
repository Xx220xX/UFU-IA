
#ifndef T7_MLP_MININST_CPP_MLP2_H
#define T7_MLP_MININST_CPP_MLP2_H


#include <iostream>
#include <string>
#include <vector>
#include "config.h"

typedef std::vector<Number> VN;

struct ActivationFunc {
    Number (*f)(Number &x);

    Number (*df)(Number &x);
};


extern ActivationFunc Sigmoid;
extern ActivationFunc Tanh;
extern ActivationFunc Relu;

struct Layer2 {
    VN _dw, _dx, _dz;
    VN _w, *_x, _z, _y;
    Size no;
    Number alpha;
    ActivationFunc f;
};

class MLP2 {
public:
    VN _x;
    VN _y;
    VN _dy;
    VN _t;
    Size nEntrada, nSaida;
    std::vector<Layer2> layers;
    Number defaultAlpha = 0.1;

    MLP2() = default;

    void setNInput(Size nentrada);

    int addLayer(Size numero_neoronios_saida, Number alpha = -1, ActivationFunc f = Tanh);

    int forward(PNumber x);

    int forward();

    Number backward(PNumber t);
};

#endif //T7_MLP_MININST_CPP_MLP2_H
