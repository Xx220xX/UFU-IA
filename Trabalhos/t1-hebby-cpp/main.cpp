#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#define BEGINTABLE(out)fprintf(out,"\\begin{table}[H]\n");
#define CENTERING(out)fprintf(out,"\\centering\n");
#define CAPTION(out, f, ...)fprintf(out,"\\caption{");fprintf(out,f,##__VA_ARGS__);fprintf(out, "}");
#define LABEL(out, f, ...)fprintf(out,"\\label{");fprintf(out,f,##__VA_ARGS__);fprintf(out, "}");
#define COLUMNS(out, n, position){fprintf(out,"|" position "|");for(int _i=1;_i<n;_i++)fprintf(out,position "|");}


void latexHead(FILE *out, int columns) {
	BEGINTABLE(out)
	CENTERING(out)
	fprintf(out, "\\begin{tabular}{");
	COLUMNS(out, columns, "c")
	fprintf(out, "}\n\\hline\n");
}


int main() {
	int x1[4] = {-1, -1, 1, 1};
	int x2[4] = {-1, 1, -1, 1};
	setlocale(LC_CTYPE, "pt-BR.utf-8");
	int t[4] = {};
	FILE *lx_hedd;
	FILE *lx_validacao;
	FILE *lx_tables;
	FILE *lx_nao_ok;
	fopen_s(&lx_hedd, "../tex/hedd.tex", "w");
	fopen_s(&lx_validacao, "../tex/verifica.tex", "w");
	fopen_s(&lx_tables, "../tex/t16possiveis.tex", "w");
	fopen_s(&lx_nao_ok, "../tex/nnok.tex", "w");
	int w1, w2, b, dw1, dw2, db, y, okay;
	int c = 0;
	int acertos = 0;
	char call[100];
	fprintf(lx_nao_ok, "\\begin{itemize}\n");

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < 2; ++k) {
				for (int l = 0; l < 2; ++l) {
					latexHead(lx_hedd, 10);
					latexHead(lx_tables, 3);
					fprintf(lx_hedd, "$x_1$ & $x_2$ & 1 & t & $dw_1$ & $dw_2$ & db & $w_1$ & $w_2$ & b \\\\ \\hline\n");
					fprintf(lx_tables, " $x_1$ & $x_2$ & t\\\\ \\hline\n");
					b = 0, w1 = 0, w2 = 0;
					t[0] = 2 * i - 1;
					t[1] = 2 * j - 1;
					t[2] = 2 * k - 1;
					t[3] = 2 * l - 1;
					for (int m = 0; m < 4; ++m) {
						fprintf(lx_tables, " %d & %d & %d\\\\ \\hline\n", x1[m], x2[m], t[m]);
						dw1 = x1[m] * t[m];
						dw2 = x2[m] * t[m];
						db = t[m];
						w1 += dw1;
						w2 += dw2;
						b += db;
						fprintf(lx_hedd, "%d & %d & 1 & %d & %d & %d & %d & %d & %d & %d\\\\ \\hline\n", x1[m], x2[m], t[m], dw1, dw2, db, w1, w2, b);
					}
					fprintf(lx_tables, "\\end{tabular}\n");
					fprintf(lx_hedd, "\\end{tabular}\n");
					CAPTION(lx_tables, "%s %d", "Função lógica", c + 1)
					LABEL(lx_tables, "tab:%d", c + 1)
					CAPTION(lx_hedd, "%s %d", "Hedd Função lógica", c + 1)
					LABEL(lx_hedd, "hedd:%d", c + 1)
					fprintf(lx_tables, "\\end{table}\n\n");
					fprintf(lx_hedd, "\\end{table}\n\n");


					fprintf(lx_validacao, "\\begin{figure}[H]\n"
										  "\\centering\n"
										  "\\begin{minipage}[c]{0.49\\linewidth}\n"
										  "\\centering\n"
										  "\\[\n"
										  "\\begin{aligned}\n"
										  "&w_1 = %d\\\\\n"
										  "&w_2 = %d\\\\\n"
										  "&b = %d\\\\\n"
										  "\\end{aligned}\n"
										  "\\]\n"
										  "\\begin{tabular}{|c|c|c|c|c|}\\hline\n"
										  "$x_1$ & $x_2$ & t & y & Resultado\\\\ \\hline\n", w1, w2, b);

					okay = 1;
					for (int m = 0; m < 4; ++m) {
						y = (b + w1 * x1[m] + w2 * x2[m]) > 0 ? 1 : -1;
						okay *= (y == t[m]);
						fprintf(lx_validacao, " %d & %d & %d & %d & %s\\\\ \\hline\n", x1[m], x2[m], t[m], y, (y == t[m]) ? "Certo" : "Errado");
					}
					fprintf(lx_validacao, "\\end{tabular}\n"
										  "\\end{minipage}\n"
										  "\\hfill\n"
										  "\\begin{minipage}[c]{0.5\\linewidth}\n"
										  "\\centering\n"
										  "\\singlespacing\n"
										  "\\includegraphics[width=1.2\\textwidth]{im/im%d}\n"
										  "\\end{minipage}\n"
										  "\\caption{Validação Função Lógica %d}\n"
										  "\\label{vl%d}\n"
										  "\\end{figure}\n\n", c + 1, c + 1, c + 1);
					snprintf(call, 100, "python ../main.py \"Função logica %d\" ../im/im%d.png %d %d %d", c + 1, c + 1, w1, w2, b);
					system(call);

					if (!okay) {
						fprintf(lx_nao_ok, "\t\\item Função logica %d\n", c + 1);
					}

					acertos += okay;
					c++;
				}
			}
		}
	}
	fprintf(lx_nao_ok, "\\end{itemize}\n");
	fclose(lx_hedd);
	fclose(lx_validacao);
	fclose(lx_tables);
	fclose(lx_nao_ok);

	return 0;
}
