#include <iostream>
#include "MLP.h"
#include "Histogram.h"
#include <cmath>
#include <fstream>

#define pi 3.14159265358979323846

#include <iomanip>
#define USE_DADOS_AULA 0
int main() {
    system("cd");
    std::cout << std::fixed << std::setprecision(4);

    int NEPOCAS = 1000;
    std::vector<Number> erros(NEPOCAS);

    MLP mlp;
    mlp.setNInput(1);
    mlp.defaultAlpha = 0.01;
    mlp.addLayer(100, Tanh);
//    mlp.addLayer(50, Tanh);
//    mlp.addLayer(10, Tanh);
    mlp.addLayer(1, Tanh);
#if USE_DADOS_AULA == 0
    Number fa = 200;
    Number f = 2;
    Size n = 200;
    auto func = [&f](Number &x) { return cos(2 * pi *f* x); };
    std::vector<Number> x(n);
    std::vector<Number> y(n);

    for (Size i = 0; i < n; ++i) {
        x[i] = i / fa;
        y[i] = func(x[i]);
    }
#else
    std::vector<Number> x{0.0,0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    std::vector<Number> y{-0.9602, -0.5770, -0.0729, 0.3771, 0.6405, 0.6600, 0.4609, 0.1336, -0.2013, -0.4344, -0.5000};
    Size n = x.size();

#endif

    Number erro, lasterro = -1;
    // treinar



    for (int epoca = 0; epoca < NEPOCAS; ++epoca) {
        erro = 0;
        for (int i = 0; i < x.size(); ++i) {
            mlp.forward(&x[i]);
            erro += mlp.backward(&y[i]);
        }
        erro = erro / x.size();
        erros[epoca] = erro;
        std::cout << "Epoca: " << epoca << " Erro: " << erro << std::endl;
//        if(abs(lasterro-erro)<1e-5)break;
        lasterro = erro;
    }


    std::ofstream file("dados.m");
    file << "clc;close all;clear all;" << std::endl;
    file << "x = " << x << ";" << std::endl;
    file << "y = " << y << ";" << std::endl;
    for (Size i = 0; i < n; ++i) {
        mlp.forward(&x[i]);
        y[i] = mlp.y[0];
    }
    file << "% predict" << std::endl;
    file << "p = " << y << ";" << std::endl;
    file << "erro = " << erros << ";" << std::endl;
    file << "epoca=1:length(erro);"
            "subplot(211);"
            "plot(x,y);"
            "hold on;"
            "plot(x,p);"
            "subplot(212);"
            "plot(epoca,erro);"
         << std::endl;
    file.close();
    system("pause");

    return 0;
}
