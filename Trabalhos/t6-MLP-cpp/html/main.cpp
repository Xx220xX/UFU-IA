#include <iostream>
#include "mlp/MLP.h"
#include "mlp/Histogram.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>
#include "http/httpserver.h"
#include <thread>

#define pi 3.14159265358979323846
HTTPServer server;
#define IPv4 "0.0.0.0"
#define IPv6 "::"
#define PORT 8080

using namespace std;
using namespace json;

enum STATE {
    GETPOINTS, GETTINGNET, RUNNING, FINISH
};
struct Sample {
    vector<Number> entrada;
    vector<Number> saida;
};

struct {
    MLP mlp;
    vector<Sample> samples;
    Size nentrada{}, nsaida{};
    Number alphaSaida = 0.1;
    Size i{};
    Size maxEpoca{};
    Size epoca{};

    vector<std::string> resumo{};

    std::string samplesJSON;
    int state = STATE::GETPOINTS;
    vector<Number> erros;
    int nextresumo = 0;
    int addResumo() {
        if (epoca<nextresumo)
            return -1;
        nextresumo += (maxEpoca-1)/100;
        JSON r;
        r["epoca"] = epoca;
        r["pontos"] = JSON();
        JSON &data = r["pontos"];
        for (int i = 0; i < samples.size(); i++) {
            mlp.forward(samples[i].entrada.data());
            data[i]["x"] = mlp.x[0];
            data[i]["y"] = mlp.y[0];
        }
        cout<<r.dump()<<endl;
        resumo.push_back(std::move(r.dump()));
        return 0;
    }

    int setSize(Size size) {
        samples.resize(size);
        i = 0;
        return 0;
    }

    bool addSample(Number x, Number y) {
        if ((nentrada * nsaida) != 1)return false;
        if (i >= samples.size()) { samples.resize(i + 1); }
        samples[i] = Sample({x}, {y});
        i++;
        return true;
    }

    bool addSample(Number *x, Size nx, Number *y, Size ny) {
        if (nentrada != nx || nsaida != ny)return false;
        if (i >= samples.size()) { samples.resize(i + 1); }
        samples[i] = Sample(vector<Number>(x, x + nx), vector<Number>(y, y + ny));
        return true;
    }

    thread t;
} app;


void treinar() {
    cout << "treinado\n";
    app.erros.resize(app.maxEpoca);
    for (int ep = 0; ep < app.maxEpoca; ++ep, app.epoca++) {
        cout << ep << endl;
        Number erro = 0;
        for (Sample &s: app.samples) {
            app.mlp.forward(s.entrada.data());
            erro += app.mlp.backward(s.saida.data());
        }
        erro /= app.samples.size();
        app.erros[ep] = erro;
        app.addResumo();
    }
    app.state = STATE::FINISH;
}


int main() {
    beginHTTP();
    server.ipv4INIT(IPv4, PORT);
    server.ipv6INIT(IPv6, PORT);
    system("cd");

    server.Rota("GET", "/", HTTP() {
        switch (app.state) {
            case STATE::GETPOINTS:
                return server.renderTextResponse("static/GETPOINTS.html");
            case STATE::GETTINGNET:
                return server.renderTextResponse("static/GETTINGNET.html");
            case STATE::RUNNING:
                return server.renderTextResponse("static/RUNNING.html");
            case STATE::FINISH:
                return server.renderTextResponse("static/finish.html");
        }
        return server.NOTFOUND();
    });

    server.Rota("POST", "/upload", HTTP() {
        JSON &body = request["body"];
        Size len = body.size();
        Size nx = 1, ny = 1;
        bool hasx = true, hasy = true;
        app.samplesJSON = body.dump();
        if (len > 0) {
            hasx = body[0].hasKey("x");
            hasy = body[0].hasKey("y");
            if (!hasx || !hasy) {
                return server.Response(400, CT_Text, "trola não mano");
            }
            nx = body[0]["x"].size();
            ny = body[0]["y"].size();
            if (ny < 0) {
                ny = 1;
            }
            if (nx < 0) {
                nx = 1;
            }
            app.setSize(len);
        } else {
            return server.Response(400, CT_Text, "trola não mano");
        }
        if (nx == ny && nx == 1) {
            app.nentrada = 1;
            app.nsaida = 1;
            for (Size i = 0; i < len; ++i) {
                hasx = body[i].hasKey("x");
                hasy = body[i].hasKey("y");
                if (!hasx || !hasy) {
                    return server.Response(400, CT_Text, "trola não mano");
                }
                JSON &X = body[i]["x"];
                JSON &Y = body[i]["y"];
                Number x, y;
                x = X.ToFloat(hasx);
                y = Y.ToFloat(hasy);
                if (!hasx || !hasy) {
                    return server.Response(400, CT_Text, "trola não mano");
                }
                app.addSample(x, y);
            }
            app.state = STATE::GETTINGNET;
            return server.Response(200, CT_Text, "ok");
        } else if (nx == 1 || ny == 1) {
            return server.Response(400, CT_Text, "trola não mano");
        } else {
            vector<Number> x, y;
            app.nentrada = nx;
            app.nsaida = ny;
            x.resize(nx);
            y.resize(ny);
            for (Size i = 0; i < len; ++i) {
                hasx = body[i].hasKey("x");
                hasy = body[i].hasKey("y");
                if (!hasx || !hasy) {
                    return server.Response(400, CT_Text, "trola não mano");
                }
                JSON &X = body[i]["x"];
                JSON &Y = body[i]["y"];
                if (X.size() != nx || X.size() != ny) {
                    return server.Response(400, CT_Text, "trola não mano");
                }

                X.ToDoubleP(x.data(), nx);
                Y.ToDoubleP(y.data(), ny);
                app.addSample(x.data(), nx, y.data(), ny);
            }
            app.state = STATE::GETTINGNET;

            return server.Response(200, CT_Text, "ok");
        }
        return server.Response(400, CT_Text, "trola não mano");
    });

    server.Rota("GET", "/data", HTTP() {
        JSON r;
        r["dados"] = app.samplesJSON;
        r["maxepoca"] = app.maxEpoca;
        return server.Response(200, r);
    });

#define check()if(!ok){ cout<<"line:"<<__LINE__<<endl;\
return server.Response(400, CT_Text, "trola não mano");}
    server.Rota("POST", "/setNet", HTTP() {
        JSON &body = request["body"];
        bool ok = true;
        app.mlp = MLP();
        app.mlp.setNInput(app.nentrada);
        int len = body["camadasOcultas"].size();
        Number alpha;
        int neuroniosSaida;
        ActivationFunc &func = Tanh;
        string funcaoAtivacao;
        cout << body.dump() << endl;
        for (int i = 0; i < len; ++i) {
            JSON &camada = body["camadasOcultas"][i];
            alpha = camada["alpha"].ToFloat(ok);
            check()
            neuroniosSaida = camada["neuroniosSaida"].ToInt(ok);
            check()
            funcaoAtivacao = camada["funcaoAtivacao"].ToString(ok);
            check()
            if (funcaoAtivacao == "tanh") {
                func = Tanh;
            } else if (funcaoAtivacao == "tan") {
                func = Tan;
            } else if (funcaoAtivacao == "sigmoid") {
                func = Sigmoid;
            } else if (funcaoAtivacao == "relu") {
                func = Relu;
            } else {
                return server.Response(400, CT_Text, "trola não mano");
            }
            app.mlp.addLayer(neuroniosSaida, alpha, func);
        }
        alpha = body["alphaSaida"].ToFloat(ok);
        check()
        funcaoAtivacao = body["funcaoSaida"].ToString(ok);
        check()
        if (funcaoAtivacao == "tanh") {
            func = Tanh;
        } else if (funcaoAtivacao == "tan") {
            func = Tan;
        } else if (funcaoAtivacao == "sigmoid") {
            func = Sigmoid;
        } else if (funcaoAtivacao == "relu") {
            func = Relu;
        } else {

            return server.Response(400, CT_Text, "trola não mano");
        }
        app.mlp.addLayer(app.nsaida, alpha, func);
        int maxEpoca = body["epocas"].ToInt(ok);
        check()
        app.maxEpoca = maxEpoca;

        app.state = STATE::RUNNING;
        app.t = thread(treinar);


        return server.Response(200, CT_Text, "ok");
    });

    server.Rota("GET", "/favicon.ico", HTTP() {
        return self.renderTextResponse("static/favicon.jpg", CT_IM_JPEG);
    });


    try {
        server.startListening();
    } catch (
            const std::exception &e
    ) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    endHTTP();

    return 0;
}
