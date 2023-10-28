import os

fileRedesDisponiveis = 'redesDisponiveis.js'
redes = 'redes.js'
estatisticas = 'estatisticas.js'
#
#
# jsons = os.listdir('./treinados')
# print('const redesDisponiveis =',[x.replace('.json','') for x in jsons],file=open(fileRedesDisponiveis,'w'))
#
# file = open(redes,'w')
#
# print('const pesosTreinados = [];',file=file)
# i = 0
# for json in jsons:
#     text = open('./treinados/'+json).read().replace('\n','').replace('\r','').replace(' ','')
#
#     # print(f'pesosTreinados.push(null);',file=file)
#     print(f'pesosTreinados[{i}] = {text};',file=file)
#     i+=1
#
#
#
#


es = open('estatisticas/out.txt', 'r').read()

es = es.split('Dados foram escritos no arquivo com sucesso')
print(es[:2])
for e in es:
    e = e.split('\n')

    while len(e) > 0 and not e[0].startswith('Epoca'):
        e.pop(0)
        continue
    if len(e) == 0:
        break
    print(e)
    epoca = e.pop(0)
    erro = epoca.split('Erro ')[-1]
    epoca = epoca.split(' ')[1]
    result = {}
    for i in range(10):
        linha = e.pop(0).split(': ')[-1].strip()
        result[i] = [int(x) for x in linha.split(' ')]
        soma = sum(result[i])

        result[i] = [round(x / soma * 100, 2) for x in result[i]]
    print(epoca, erro)
    soma = 0
    for k, v in result.items():
        soma += v[k]
        print(k, v[k], v)
    print(soma / 10)
