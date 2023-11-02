//
// Created by henrique on 28/10/23.
//

#ifndef TRABALHOS_MLP_H
#define TRABALHOS_MLP_H

#include "config.h"
#include "Func.h"
#include <stdexcept>
#include <ostream>


std::string EncodeVectorToBase64(const VN & data);

namespace MLP {
    using namespace std;

    void randomize(VN &w, Number max_v);


    int maxi(PNumber v, int len);

    class Layer {
    public:
        Sz in, ot, inb;
        VN W, Z, Y;
        VN DX, DW, DZ;
        VN *X;
        Func &f;
        Number alpha;

        Layer(Sz in, Sz ot, Number alpha, Func &f);
    };


    class Mlp {
    public:
        vector<Layer> layers;
        Sz in = -1;
        VN _x, _dy;
        Sz lout = -1;
        PNumber y;
        Number default_alpha = 0.01;

        int Input(Sz in);

        int setDefaultAlpha(Number alpha);

        int addLayer(Sz o, Number alpha, Func &f);

        int addLayer(Sz o, Number alpha);

        int addLayer(Sz o, Func &f = Tanh);


        Number treinar(const Sample &s);

        Number treinar(const Number *input, const Number *t);

        int prever(const Number *input);

        friend ostream &operator<<(ostream &os, Mlp &m) {
            os << "[\n\t";
            bool first = true;
            for (const MLP::Layer &l: m.layers) {
                if (!first) {
                    os << ",\n\t";
                }
                first = false;
                os << "{\"func\":\"" << l.f.name;
                string b64 = EncodeVectorToBase64(l.W);
                os<<",\"in\":"<<l.inb<<",\"out\":"<<l.ot;
                os<<"\",\"w\":\""<<b64<<"\"";
                os << "}";
            }
            os << "\n]";
            return os;
        }
    };

}
namespace std {
    template<typename T>
    ostream &operator<<(ostream &os, const vector<T> &v) {
        os << "[";
        Sz L = v.size();
        if (L > 0)
            os << v[0];
        for (Sz i = 1; i < L; ++i) {
            os << "," << v[i];
        }
        os << "]";
        return os;
    }
}
#endif //TRABALHOS_MLP_H
