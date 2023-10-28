
#include "Histogram.h"
#include <iostream>

Histogram::Histogram(Number minimox, Number maximox, Size divisoes) : x0(minimox), xe(maximox), divisoes(divisoes) {
    std::cout << divisoes << "\n";
    x.resize(divisoes);
    ny.resize(divisoes);
    delta = (xe - x0) / divisoes;
    for (int i = 0; i < divisoes; ++i) {
        x[i] = i * delta + x0;
    }
}

int Histogram::calculate(std::vector<Number> &y) {
    if (y.empty())return -1;
    std::fill(ny.begin(), ny.end(), (Number) 0.0);
    for (Size i = 0; i < y.size(); ++i) {
        Size j = round((y[i] - x0) / delta);
        if (j < 0)j = 0;
        else if (j >= divisoes)j = divisoes - 1;
        ny[j] += 1;
    }
    Number div = y.size();
    for (int i = 0; i < divisoes; ++i) {
        ny[i] /= div;
    }
    return 0;
}

const std::vector<Number> &Histogram::X() {
    return this->x;
}

const std::vector<Number> &Histogram::NY() {
    return this->ny;
}

int Histogram::calculate(std::vector<Number> &y, Number minimox, Number maximox, Size divisoes) {
    this->divisoes = divisoes;
    this->x0 = minimox;
    this->xe = maximox;
    x.resize(divisoes);
    ny.resize(divisoes);
    delta = (xe - x0) / divisoes;
    for (int i = 0; i < divisoes; ++i) {
        x[i] = i * delta + x0;
    }
    return this->calculate(y);
}

Histogram::Histogram() : Histogram(-1, 1,100) {
}
