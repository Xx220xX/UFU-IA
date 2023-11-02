import os, json

fileRedesDisponiveis = 'redesDisponiveis.js'
redes = 'redes.js'

all_files = [x.replace('.json', '') for x in os.listdir('../treinadas') if x.endswith('.json')]


def load(filename):
    ett = filename + '.txt'
    info = {'Epocas': [], 'rede': None}
    with open(ett, 'r') as f:
        estaticas = f.read().replace('\r', '')
        epocas = estaticas.split('\n\n')
        for epoca in epocas:
            resumo = {'confusao': []}
            for line in epoca.splitlines():
                k, v = line.split(': ')
                if k == 'Epoca': continue
                if k.isdigit():
                    resumo['confusao'].append(eval(v, {}, {}))
                else:
                    resumo[k] = eval(v)
            info['Epocas'].append(resumo)
    sjson = open(filename + '.json', 'r').read().replace('\n','')
    sjson = sjson.strip()
    if sjson.startswith('{'):
        sjson = sjson[1:-1]
    info['rede'] = json.loads(sjson)
    return info
file = open(redes,'w')
disponiveis = open(fileRedesDisponiveis,'w')
print('redes = [',file=file)
print('const redesDisponiveis = [',file=disponiveis)
begin = False
for f in all_files:
    if begin:
        print(',',file=file)
        print(',',file=disponiveis)
    info = load('../treinadas/' + f)
    print(f'"{f}"',file=disponiveis,end='')
    print(info,file=file,end='')
    begin = True
print('];',file=file)
print('];',file=disponiveis)

