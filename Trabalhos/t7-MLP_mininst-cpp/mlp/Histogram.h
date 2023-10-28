
#ifndef T6_MLP_CPP_HISTOGRAM_H
#define T6_MLP_CPP_HISTOGRAM_H

#include <vector>
#include "config.h"

class Histogram {
    std::vector<Number> x, ny;
    Number x0, xe, delta;
    Size divisoes;
public:
    Histogram() ;
    Histogram(Number minimox , Number maximox , Size divisoes);

    int calculate(std::vector<Number> &y);

    int calculate(std::vector<Number> &y, Number minimox, Number maximox, Size divisoes = 100);

    const std::vector<Number> &X();

    const std::vector<Number> &NY();
};


#endif //T6_MLP_CPP_HISTOGRAM_H
