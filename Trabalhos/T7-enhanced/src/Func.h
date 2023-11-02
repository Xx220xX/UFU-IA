//
// Created by henrique on 28/10/23.
//

#ifndef TRABALHOS_FUNC_H
#define TRABALHOS_FUNC_H

#include "config.h"
#include <string>

struct Func {
    std::string name;

    Number (*f)(Number x) = nullptr;

    Number (*df)(Number x) = nullptr;
};

extern Func Tanh;

//extern Func TanhLn;

#endif //TRABALHOS_FUNC_H
