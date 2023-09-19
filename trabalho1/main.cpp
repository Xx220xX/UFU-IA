#include <iostream>
#include "Perceptron/perceptron.h"

using namespace std;
Perceptron *p;
#define N_Entrada 25
#define N_Saida 1
double alpha = 1e-30;
int max_epocas = 100;

struct Sample {
    std::string classe;
    REAL_TYPE input[N_Entrada];
    REAL_TYPE *target;
};
struct Label {
    std::string classe;
    REAL_TYPE target[N_Saida];
};


Label labels[] = {{"X", 1},
                  {"T", -1}};
Sample samples[] = {{"X",
                            1, -1, -1, -1, 1,
                            -1, 1,  -1, 1,  -1,
                            -1, -1, 1, -1, -1,
                            -1, 1,  -1, 1,  -1,
                            1,  -1, -1, -1, 1,
                            labels[0].target
                    },
                    {"T",
                            1, 1,  1,  1,  1,
                            -1, -1, 1,  -1, -1,
                            -1, -1, 1, -1, -1,
                            -1, -1, 1,  -1, -1,
                            -1, -1, 1,  -1, -1,
                            labels[0].target

                    }};


const int nsamples = sizeof(samples) / sizeof(Sample);
const int nlabels = sizeof(labels) / sizeof(Label);

int main(int nargs, char **args) {
    double t0 = segundos();
    int acertos;
    // primeiro caso
    p = new Perceptron(N_Entrada, N_Saida, alpha);
    int epoca;
    for (epoca = 1; epoca <= max_epocas; ++epoca) {
        cout << "Epoca " << epoca << endl;
        for (int s = 0; s < nsamples; ++s) {
            p->achaSaidas(samples[s].input);
            p->arrumaPesos(samples[s].target);
        }
        acertos = 0;
        for (int s = 0; s < nsamples; ++s) {
            p->achaSaidas(samples[s].input);
            cout << samples[s].classe << ": ";
            for (int i = 0; i < N_Saida; ++i) {
                cout << p->y[i] << " ";
            }
            cout <<" is ";
            for (int i = 0; i < nlabels; ++i) {
                if (!memcmp(labels[i].target, p->y, sizeof(REAL_TYPE) * N_Saida)) {
                    cout << labels[i].classe;
                }
            }
            cout << endl;
            for (int i = 0; i < N_Saida; ++i) {
                acertos += (p->y[i] == samples[s].target[i]);
            }
        }
        cout << "Acertos: " << acertos << " of " << N_Saida * nsamples << endl;
        if (acertos >= nsamples * N_Saida) {
            break;
        }
    }
    t0 = segundos() - t0;
    cout << "Tempo de treino " << t0 << "segundos" << endl;
    delete p;
    system("pause");
    return 0;
}