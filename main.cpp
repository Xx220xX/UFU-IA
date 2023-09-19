#include "http/httpserver.h"
#include <locale>

#include "Perceptron/perceptron.h"

int main() {
	setlocale(LC_CTYPE, "pt-BR.utf-8");
//	HTTPServer server;
//	server.ipv4INIT("0.0.0.0", 80); // Defina a porta aqui
//	server.startListening();

	perceptron = new Perceptron(2, 2, 1e-15);
	// gerar tabela verdade
	REAL_TYPE x[] = {-1, -1,
					 -1, 1,
					 1, -1,
					 1, 1};
	REAL_TYPE t[] = {1, -1,
					 1, 1,
					 1, 1,
					 -1, 1};

	for (int epoca = 0; epoca < 200; ++epoca) {
		std::cout << (epoca + 1) << " ____\n";
		for (int i = 0; i < 4; ++i) {
			perceptron->achaSaidas(x + i * 2);
			perceptron->arrumaPesos(t + i * 2);
		}
		std::cout << "x1 x2 -> y t" << std::endl;
		int acertos = 0;
		for (int i = 0; i < 4; ++i) {
			perceptron->achaSaidas(x + i * 2);
			acertos += (perceptron->y[0] == t[i * 2]);
			acertos += (perceptron->y[1] == t[i * 2 + 1]);
			std::cout << x[i * 2] << " " << x[i * 2 + 1] << " -> " << perceptron->y[0] << " " << perceptron->y[1] << " | " << t[i * 2] << " " << t[i * 2 + 1] << std::endl;
		}
		if (acertos == 8) {
			break;
		}

	}
	delete perceptron;
	return 0;
}
