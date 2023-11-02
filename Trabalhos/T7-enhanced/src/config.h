//
// Created by henrique on 28/10/23.
//

#ifndef TRABALHOS_CONFIG_H
#define TRABALHOS_CONFIG_H

#include <cstdint>
#include <vector>
#include <string>

typedef float Number;
typedef Number *PNumber;
typedef int64_t Sz;
typedef std::vector<Number> VN;

struct Sample {
    std::string className;
    int valueName;
    std::vector<Number> x, y;
};

#endif //TRABALHOS_CONFIG_H
