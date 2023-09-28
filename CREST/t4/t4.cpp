#include <http/httpserver.h>
#include <stdlib.h>
#include <Windows.h>

typedef double real_t;
using namespace json;

real_t randoF() {
    return ((real_t) rand() / (real_t) RAND_MAX);
}

template<typename T>
void printV(T *v, int len, const char *var = nullptr) {
    if (var)
        std::cout << var << ":";
    std::cout << "[";
    if (len > 0)
        std::cout << v[0];
    for (int i = 1; i < len; ++i) {
        std::cout << " " << v[i];
    }
    std::cout << "]";
    fflush(stdout);
}

class NeuralNetwork {
public:
    real_t alpha = 0.1;
    int nroEntrada{}, nroSaida{}, total{};
    real_t *w{};
    real_t *b{};
    real_t *z{};
    real_t *dz{};
    real_t *a{};
    real_t *x{};
    real_t *t{};
    real_t maxima_alteracao;

    void init(int nroEntrada, int nroSaida) {
        srand(time(nullptr));
        this->nroEntrada = nroEntrada;
        this->nroSaida = nroSaida;
        this->total = nroSaida * nroEntrada;
        w = new real_t[total];
        b = new real_t[nroSaida];
        z = new real_t[nroSaida];
        dz = new real_t[nroSaida];
        a = new real_t[nroSaida];
        for (int i = 0; i < total; ++i) {
            w[i] = randoF() - .5;
        }
        for (int i = 0; i < nroSaida; ++i) {
            b[i] = randoF() - .5;
        }
    }

    ~NeuralNetwork() {
        delete w;
        delete b;
        delete z;
        delete dz;
        delete a;
    }

    real_t treina(real_t *entrada, real_t *target) {
        real_t erro_medio = 0;
        x = entrada;
        t = target;
//        printV(w, total,"w");
//        printV(b, nroSaida,"b");
//        std::cout << "\n";
        // x[1][i] w[i][j] + b[j]= z[j]
        for (int j = 0; j < nroSaida; ++j) {
            real_t Z = b[j];
            real_t dZ;
            for (int i = 0; i < nroEntrada; ++i) {
                Z += w[i * nroSaida + j] * x[i];
            }

            // E = (t-z)^2/2
            // dE = -(t-z)*dz/dW*
            // dz/dw= x[i]
            // dE =
            dZ = t[j] - Z;
            z[j] = Z;
            dz[j] = dZ;
            real_t alteracao = alpha * dZ;
            b[j] += alteracao;
            alteracao = abs(alteracao);
            if (alteracao > maxima_alteracao)maxima_alteracao = alteracao;
            for (int i = 0; i < nroEntrada; ++i) {
                alteracao = alpha * dZ * x[i];
                w[i * nroSaida + j] += alteracao;
                alteracao = abs(alteracao);
                if (alteracao > maxima_alteracao)maxima_alteracao = alteracao;
            }
        }
        for (int j = 0; j < nroSaida; ++j) {
            erro_medio += (dz[j] * dz[j]);
        }
        erro_medio /= (2 * nroSaida);
//        printV(x, nroEntrada,"x");
//        printV(z, nroSaida,"z");
//        printV(t, nroSaida,"t");
//        printV(dz, nroSaida,"dz");
//        std::cout << erro_medio << "\n";
        return erro_medio;
    }

    void geraSaida(real_t *entrada) {
        x = entrada;
        for (int j = 0; j < nroSaida; ++j) {
            z[j] = b[j];
            for (int i = 0; i < nroEntrada; ++i) {
                z[j] += w[i * nroSaida + j] * x[i];
            }
            a[j] = z[j] > 0 ? 1. : -1;
        }
    }

    bool check(real_t *entrada, real_t *saida) {
        geraSaida(entrada);
//        printV(z,nroSaida,"\tz");
//        printV(a,nroSaida," a");
//        printV(saida,nroSaida," t");
        return !memcmp(saida, a, sizeof(real_t) * nroSaida);
    }
};

class Sample {
public:
    std::string name;
    std::vector<real_t> entrada;
    std::vector<real_t> saida;
};

class DataSet {
public:
    std::vector<real_t> erro;
    std::vector<real_t> acertos;
    std::vector<Sample> train;
    std::vector<Sample> test;
    bool useTrainAsTest = true;
    bool running = false;
    int max_epocas{};
    int epoca{};
    real_t expectWinRate{};
    NeuralNetwork *neuralNetwork{};

    void config(int max_epocas, real_t alpha, real_t expectWinRate) {
        neuralNetwork->alpha = alpha;
        this->max_epocas = max_epocas;
        this->expectWinRate = expectWinRate;
    }

    JSON getErro(int offset) {
        JSON r;
        int erro_len = erro.size();
        int acerto_len = acertos.size();
        if (acerto_len < erro_len) {
            erro_len = acerto_len;
        }
        r["error"] = {};
        r["winrate"] = {};
        for (int i = offset; i < erro_len; ++i) {
            r["error"].append(erro[i]);
            r["winrate"].append(acertos[i]);
        }
        r["running"] = running && (epoca == erro_len);
        return r;
    }

    std::vector<Sample> &getTestData() {
        if (useTrainAsTest)
            return train;
        return test;
    }

    void add(std::string name, std::initializer_list<real_t> entrada, std::initializer_list<real_t> saida,
             bool isTrain = true) {
        if (isTrain) {
            train.push_back(Sample(name, entrada, saida));
        } else {
            test.push_back(Sample(name, entrada, saida));
        }
    }

    void addTest(std::string name, std::initializer_list<real_t> entrada, std::initializer_list<real_t> saida) {
        add(name, entrada, saida, false);
    }

    int treinar() {
        erro.clear();
        acertos.clear();
        epoca = 0;
        real_t sum_erro;
        real_t sum_acerto;
        std::vector<Sample> &dt_test = getTestData();
        if (train.size() <= 0 || dt_test.size() <= 0)return -1;
        running = true;
        for (int epoch = 0; epoch < max_epocas; ++epoch) {
            sum_erro = 0;
            sum_acerto = 0;
            neuralNetwork->maxima_alteracao = 0;
            std::cout << "Epoca " << epoca << "\n";
            for (int i = 0; i < train.size(); ++i) {
                sum_erro += neuralNetwork->treina(train[i].entrada.data(), train[i].saida.data());
            }
            for (int i = 0; i < dt_test.size(); ++i) {
                sum_acerto += neuralNetwork->check(dt_test[i].entrada.data(), dt_test[i].saida.data());
//                printV(&sum_acerto,1,"sum");
//                std::cout<<"\n";
            }



            sum_acerto = 100. * sum_acerto / dt_test.size();
            sum_erro = sum_erro / train.size();

            erro.push_back(sum_erro);
            acertos.push_back(sum_acerto);
            this->epoca = epoch;
            if (sum_acerto > expectWinRate) {
//                printV(&sum_acerto,1,"sum_acerto");
                break;
            }
            if (neuralNetwork->maxima_alteracao < neuralNetwork->alpha / 1000) {
//                printV(&neuralNetwork->maxima_alteracao,1,"maxima_alteracao");
                break;
            }
        }
        running = false;
        return 0;
    }
};

void HTMLFUNCTION(HTTPServer &server) {
    server.ipv4INIT("0.0.0.0", 80);
    server.Rota("GET", "/", HTTP() { return self.renderTextResponse("index.html"); });
    server.Rota("GET", "/index.js", HTTP() { return self.renderTextResponse("index.js", CT_JS, 200); });
    server.Rota("GET", "/favicon.ico", HTTP() { return self.renderBinaryResponse("favicon.jpeg", CT_IM_JPEG, 200); });
    server.Rota("GET", "/background", HTTP() { return self.renderBinaryResponse("OIG.jpeg", CT_IM_JPEG, 200); });
    server.Rota("GET", "/style.css", HTTP() { return self.renderTextResponse("style.css", CT_CSS, 200); });

    server.Rota("POST", "/treinar", HTTP() {
        bool ok;
        real_t alpha;
        real_t expectWinRate;
        int maximo_epocas;
        DataSet *d = (DataSet *) self.info;
        if (d->running) return self.Response(401, CT_Text, "alread runnig");
        maximo_epocas = request["body"]["maximo_epocas"].ToInt(ok);
        if (!ok) return self.Response(400, CT_Text, "missing 'maximo_epocas'");
        alpha = request["body"]["alpha"].ToFloat(ok);
        if (!ok) return self.Response(400, CT_Text, "missing 'alpha'");
        if (alpha <= 0.) return self.Response(400, CT_Text, "'alpha' must be greater than 0");
        if (alpha >= 1.) return self.Response(400, CT_Text, "'alpha' must be less than 1");
        expectWinRate = request["body"]["expectWinRate"].ToFloat(ok);
        if (!ok) return self.Response(400, CT_Text, "missing 'expectWinRate'");
        if (expectWinRate > 1.)return self.Response(400, CT_Text, "'alpha' must be less than or equal 1");
        d->config(maximo_epocas, alpha, expectWinRate);
        LPTHREAD_START_ROUTINE handle = [](void *pd) -> DWORD {
            return ((DataSet *) pd)->treinar();
        };
        CreateThread(
                NULL,    // default security attributes
                0,           // use default stack size
                handle,    // thread function name
                d,          // argument to thread function
                0,      // use default creation flags
                nullptr);   // returns the thread identifier

        return self.Response(200, CT_Text, "treinando");

    });
    server.Rota("GET", "/treinar", HTTP() {
        int offset = request["params"]["offset"].ToInt();
        JSON response = ((DataSet *) self.info)->getErro(offset);
        return self.Response(200, response);
    });
    server.Rota("POST", "/close", HTTP() {
        self.run = false;
        return self.Response(200, CT_Text, "Finalizando programa");
    });

    server.Rota("POST","/teste",HTTP(){
       return self.Response(request["body"]["code"].ToInt(),request["body"]);
    });

    server.startListening();
}

int main() {
    srand(time(nullptr));
    setlocale(LC_CTYPE, "pt-BR.utf-8");
    HTTPServer server;
    DataSet d;
    NeuralNetwork n{};
    d.neuralNetwork = &n;
    server.info = &d;

    n.init(2, 1);
    d.add("0", {-1, -1}, {-1});
    d.add("0", {-1, 1}, {-1});
    d.add("0", {1, -1}, {-1});
    d.add("1", {1, 1}, {1});
    d.useTrainAsTest = true;
    HTMLFUNCTION(server);
//    d.config(10000, 1e-2, 0.98);
//    d.treinar();

    std::cout << "Finalizado com sucesso\n";

    return 0;
}
