//
// Created by henrique on 28/10/23.
//

#include "Func.h"
#include <cmath>

Func Tanh = {"ftanh", [](Number x){return (Number)tanh(x);}, [](Number x) {
    Number y = cosh(x);
    return (Number) 1.0 / (y * y);
}};

//Func TanhLn = {"ftanh_log", [](Number x) {
//    return (Number) (x>1.?log(x+1.3546975)-0.0948181139:(x<-1.?-log(-x+1.3546975)+0.0948181139:tanh(x)));
//
//    }, [](Number x) {
//    Number y =  x>0.?x:-x;
//    if (y>1) return (Number)(1.0/(y+1.3546975));
//    y = cosh(y);
//    y = 1. /(y*y);
//    return  y;
//}
//};
