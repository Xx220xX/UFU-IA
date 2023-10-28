// implementação da Regra Delta para treinamento do adaline
// aplicação: regressão linear
// data: 05/05/2021
// autor: Keiji Yamanaka

clc; // limpa area de comando
clear;// limpa as variáveis da area de trabalho

// dados de treinamento
// tabela verdade
//  x1  x2
f=[2  4  6  8 10 12 14]; // força de trabalho
t=[30 24 22 19 13 10 8]; // tempo de execução

 //gera gráfico dos pontos
clf();  //limpa a janela de gráficos
subplot(2, 2, 1);
set(gca(),"auto_scale", "on");
set(gca(),"data_bounds", [0,0;15,35]);
title("Dados");
xlabel("força de trabalho");
ylabel("tempo de execução");
da=gda();
da.y_location="origin";
da.x_location="origin";
    
plot(f, t,'bd');

pause();
 //gráfico do erro quadrático total
 subplot(2,2,2);
set(gca(),"auto_scale", "on");
title("Erro Quadrático Total");
xlabel("ciclos");
ylabel("EQT");
da=gda();
da.y_location="origin";
da.x_location="origin";

// inicialização das variáveis e dos parâmetros
want=0.5- rand(1,1,"uniform"); // inicialização dos pesos
bant=0.5- rand();
teta = 0;// limiar  da função de ativação degrau(rede treinada)
alfa = 0.01; // taxa de aprendizagem
numciclos=350;  // número  total de ciclos de treinamento
ciclos=0;  // conta o número de vezes que os dados foram apresentados

// treinar enquanto condição de parada não for satisfeita
mprintf("Treinamento do Adaline\n");
while ciclos<numciclos // limite de treinamento
    erroquadratico=0;
    ciclos=ciclos+1; //  conta número de ciclos de treinamento
    mprintf("ciclos = %d\n", ciclos);
     for entrada =1:7 // apresenta todos os padrões de entrada
         yliquido = want*f(entrada)+ bant;
         // função de ativação: linear
         y=yliquido; 
         // cálculo do erro quadrático
         erroquadratico= erroquadratico+(t(entrada)-y)^2;
         // atualização dos pesos
         wnovo= want+alfa*(t(entrada)-y)*f(entrada);
         bnovo=bant+alfa*(t(entrada)-y);
         // salva os pesos para a próxima atualização
         want=wnovo;
         bant=bnovo;
         
     end
     plot(ciclos, erroquadratico, 'r*');
     
end

 //imprimindo a gráfico da regressão linear
 //encontrada pelo Adaline
 subplot(2,2,3);
 title("Regressão Linear");
xlabel("força de trabalho");
ylabel("tempo de execução");
da=gda();
da.y_location="origin";
da.x_location="origin";
    
plot(f, t,'bd');
for abcissa=0:0.5:15
    ordenada = abcissa*wnovo+bnovo;
    plot(abcissa, ordenada,'g.') ;
end
