
#include <cstring>
#include "MLP.h"
#include <random>
#include <string>
#include <sstream>

ActivationFunc Sigmoid = {
        [](Number &x) { return 1 / (1 + exp(-x)); },
        [](Number &x) { return Sigmoid.f(x) * (1.0 - Sigmoid.f(x)); },
};
ActivationFunc Tanh = {
        [](Number &x) { return tanh(x); },
        [](Number &x) { return 1. / pow(cosh(x), 2); },
};
ActivationFunc Relu = {
        [](Number &x) { return x > 0 ? x : 0.; },
        [](Number &x) { return x > 0 ? 1. : 0.; },
};
ActivationFunc Tan = {
        [](Number &x) { return tan(x); },
        [](Number &x) { return 1. / pow(cos(x), 2); },
};

Number randf(Number a, Number b);

void Layer::setInput(std::vector<Number> *x) {
    this->x = x;
}

Layer::Layer(Size nInput, Size nOutput, Number alpha, ActivationFunc &func, bool isInit) :
        nInput(nInput), nOutput(nOutput), alpha(alpha), func(func), isInit(isInit) {
    w.resize((nInput + BIAS) * nOutput);
    dw.resize((nInput + BIAS) * (nOutput));
    z.resize(nOutput);
    dx.resize(nInput);
    dz.resize(nOutput);
    y.resize(nOutput + BIAS);

#if BIAS == 1
    this->y[nOutput] = 1;
#endif
    for (Size i = this->w.size() - 1; i >= 0; --i) {
        w[i] = randf(-0.48, 0.48);
        w[i] += w[i] > 0 ? 0.02 : -0.02;
    }
}

int Layer::forward() {
    Size inputWithBias = nInput + BIAS;
//    printf("layer:0x%p  x:0x%p y:0x%p\n", this, this->x, &this->y);
#pragma  omp parallel for
    for (int j = 0; j < nOutput; ++j) {
        Number soma = 0;
        for (int i = 0; i < inputWithBias; ++i) {
            soma += w[i * nOutput + j] * (*x)[i];
        }
        z[j] = soma;
        y[j] = func.f(soma);
    }
    return 0;
}

int Layer::backward(std::vector<Number> &dy) {
    Size inputWithBias = nInput + BIAS;
#pragma  omp parallel for
    for (int i = 0; i < nOutput; ++i) {
        dz[i] = func.df(z[i]) * dy[i];
    }
#pragma  omp parallel for
    for (int j = 0; j < nOutput; ++j) {
        for (int i = 0; i < inputWithBias; ++i) {
            dw[i * nOutput + j] = dz[j] * (*x)[i];
        }
    }
    if (!isInit) {
//        #pragma  omp parallel for
        for (int i = 0; i < nInput; ++i) {
            Number soma = 0;
            for (int j = 0; j < nOutput; ++j) {
                soma = dz[j] * w[i];
            }
            dx[i] = soma;
        }
    }
#pragma  omp parallel for
    for (Size k = inputWithBias * nOutput - 1; k >= 0; --k) {
        w[k] = w[k] - alpha * dw[k];
    }

    return 0;
}

int MLP::forward(Number *input) {
    memcpy_s(this->x, nInput * sizeof(Number), input, nInput * sizeof(Number));
    return this->forward();
}

int MLP::forward() {
    for (Layer &layer: this->layers) {
        layer.forward();
    }
    return 0;
}


Number MLP::backward(Number *target) {
    memcpy_s(this->t.data(), nOutput * sizeof(Number),
             target, nOutput * sizeof(Number));
    return this->backward();
}

Number MLP::backward() {
    std::vector<Number> &last_dy = this->dy;
    Number erro = 0;
    for (int i = 0; i < nOutput; ++i) {
        last_dy[i] = y[i] - t[i];
        erro += last_dy[i] * last_dy[i] * 0.5;
    }
    for (int i = this->layers.size() - 1; i >= 0; --i) {
        layers[i].backward(last_dy);
        last_dy = layers[i].dx;
    }
    return erro / nOutput;
}

int MLP::addLayer(Size _nOutput, ActivationFunc &_func) {
    return this->addLayer(_nOutput, defaultAlpha, _func);
}

int MLP::addLayer(Size _nOutput, Number _alpha, ActivationFunc &_func) {
    if (layers.empty()) {
        layers.push_back(std::move(Layer(this->nInput, _nOutput, _alpha, _func, true)));
        mem_x.resize(nInput + BIAS);
        this->x = mem_x.data();
#if BIAS == 1
        this->mem_x[nInput] = 1;
#endif
    } else {
        layers.push_back(std::move(Layer(this->nOutput, _nOutput, _alpha, _func)));
        layers[0].setInput(&mem_x);
        for (int i = 1; i < layers.size(); ++i) {
            layers[i].setInput(&layers[i - 1].y);
        }
    }

    dy.resize(_nOutput);
    t.resize(_nOutput);
    this->nOutput = _nOutput;
    y = layers[layers.size() - 1].y.data();
    return 0;
}

std::string prettyPrintJson(const std::string &jsonString) {
    std::string prettyJson;
    int indentLevel = 0;
    bool inString = false;
    bool inArray = false;
    bool inObject = false;

    for (char c: jsonString) {
        if (c == '"' && (prettyJson.empty() || prettyJson.back() != '\\')) {
            inString = !inString;
        }

        if (!inString) {
            switch (c) {
                case '{':
                    prettyJson += c;
                    prettyJson += '\n';
                    indentLevel++;
                    prettyJson.append(indentLevel, '\t');
                    inObject = true;
                    break;
                case '}':
                    prettyJson += '\n';
                    indentLevel--;
                    inObject = false;
                    prettyJson.append(indentLevel, '\t');
                    prettyJson += c;
                    break;
                case '[':
                    inArray = true;
                    prettyJson += c;
                    break;
                case ']':
                    inArray = false;
                    prettyJson += c;
                    break;
                case ',':
                    prettyJson += c;
                    if (!inArray || inObject) {
                        prettyJson += '\n';
                        prettyJson.append(indentLevel, '\t');
                    }
                    break;
                default:
                    prettyJson += c;
                    break;
            }
        } else {
            prettyJson += c;
        }
    }

    return prettyJson;
}

void MLP::setNInput(Size _nInput) {
    this->nInput = _nInput;
}

Number randf(Number a, Number b) {
    static std::default_random_engine generator{(std::mt19937::result_type) time(NULL)};

    std::uniform_real_distribution<Number> distribution(a, b);
    return distribution(generator);
}


std::ostream &operator<<(std::ostream &os, const Layer &layer) {
    os <<
       //    "{"
       //    "\"nInput\":" << layer.nInput <<","
       //    "\"nOutput\":" << layer.nOutput << ","
       "["
//    "\"w\":"
            ;
    const Number *w = layer.w.data();
    for (int i = 0; i < layer.nInput + BIAS; ++i) {
        if (i > 0)os << ", ";
        os << std::vector<Number>(w + i * layer.nOutput, w + (i + 1) * layer.nOutput);
    }
    os << "]"
//          "}"

//        layer.w
            ;
    return os;
}

std::ostream &operator<<(std::ostream &os, const MLP &mlp) {
//    std::ostringstream out;
//    out << mlp.layers;
//    os << prettyPrintJson(out.str());
    os << "[\n";
    if (!mlp.layers.empty())
        os << "" << mlp.layers[0];
    for (int i = 1; i < mlp.layers.size(); ++i) {
        os << ",\n" << mlp.layers[i];
    }
    os << "\n]";

    return os;
}
