import numpy
import numpy as np
from threading import Thread, Lock


class RNA:
    def __init__(self, nro_entrada, nro_saida, alpha=0.1, theta=0):
        self.w = np.random.normal(0.5, 0.1, (nro_entrada, nro_saida)).astype("float")
        self.b = np.random.normal(0.25, 0.05, (1, nro_saida)).astype("float")
        self.alpha = alpha
        self.theta = theta
        self.y = np.array((1, nro_saida), dtype=float)
        self.x = np.array((1, nro_entrada), dtype=float)

    def calcularSaida(self, s):
        self.x = np.array(s)
        y_liq = (self.x @ self.w) + self.b
        self.y = (y_liq >= self.theta) * 2 - 1

    def ajustarPesos(self, t):
        if (self.y != t).all():
            d_w = self.alpha * np.transpose((t - self.y) @ self.x)
            d_b = self.alpha * self.y
            self.w += d_w
            self.b += d_b
        return np.sum((t - self.y) ** 2) / len(t)  # erro

    def aprender(self, entrada, target):
        self.calcularSaida(entrada)
        return self.ajustarPesos(target)

    def saveLast(self):
        self.w_ant = self.w

    def compareLast(self):
        comparando = np.max(np.abs(self.w_ant - self.w))
        return comparando >= 1e-16

    def avalia(self, entrada, target):
        self.calcularSaida(entrada)
        saida = np.where(self.y > 0, 1, -1)
        return (saida == target).all()


class Sample:
    def __init__(self, name, entrada, target):
        self.name = name
        self.entrada: numpy.ndarray = numpy.array(entrada, dtype=float)
        self.target: numpy.ndarray = numpy.array(target, dtype=float)


class DataSet:
    def __init__(self, nro_entrada, nro_saida, name):
        self.rna = RNA(nro_entrada, nro_saida)
        self.name = name
        self.thread = None
        self.nro_saida = nro_saida
        self.nro_entrada = nro_entrada
        self.maximoEpocas = 0
        self.taxaAcertoMinimo = 0
        self.train: list[Sample] = []
        self.test: list[Sample] = []
        self.erroPorEpoca = []
        self.taxaAcertoPorEpoca = []
        self.running = False
        self.lock = Lock()

    def config(self, rna, maximoEpocas, taxaAcertoMinimo, useTrainAsTest=False, alpha=0.1):
        self.maximoEpocas = maximoEpocas
        self.taxaAcertoMinimo = taxaAcertoMinimo
        self.erroPorEpoca = []
        self.taxaAcertoPorEpoca = []
        self.running = False
        self.result = True
        self.rna = rna
        if useTrainAsTest:
            self.test = self.train
        self.rna.alpha = alpha
        return self

    def put(self, samples, isForTest=False):
        samples = [Sample(*sample) for sample in samples]
        self.addSample(samples, isForTest)
        return self

    def addSample(self, sample: list[Sample], isForTest=False):
        if isinstance(sample, list):
            for s in sample:
                if isinstance(s, Sample):
                    if len(s.entrada) != self.nro_entrada or len(s.target) != self.nro_saida:
                        return False
                    if isForTest:
                        self.test.append(s)
                    else:
                        self.train.append(s)
                else:
                    return False
        else:
            return False
        return True

    def treinar(self):
        self.running = True
        try:
            for epoca in range(self.maximoEpocas):
                erro = 0
                acerto = 0
                self.rna.saveLast()
                for sample in self.train:
                    erro += self.rna.treinar(sample.entrada, sample.target)
                for sample in self.test:
                    acerto += self.rna.avalia(sample.entrada, sample.target)
                if self.rna.compareLast():
                    break
                acerto = 100 / len(self.test) * acerto
                erro = erro / len(self.train)
                self.setInfo(erro, acerto)
        except Exception as e:
            print(e)
        self.running = False

    def setInfo(self, erro, taxaAcerto):
        self.lock.acquire()
        self.erroPorEpoca.append(erro)
        self.taxaAcertoPorEpoca.append(taxaAcerto)
        self.lock.release()

    def getInfo(self, offset):
        self.lock.acquire()
        epoca = min(len(self.erroPorEpoca), len(self.taxaAcertoPorEpoca))
        r = dict(running=self.running, epoca=epoca, erroPorEpoca=self.erroPorEpoca[offset:epoca], acertoPorEpoca=self.taxaAcertoPorEpoca[offset:epoca], )
        self.lock.release()
        return r

    def treinarAssyncrono(self):
        if self.running: return False
        self.thread = Thread(target=self.treinar)
        self.thread.start()
        return True

    def toDict(self):
        return dict(name=self.name, nro_entrada=self.nro_entrada, nro_saida=self.nro_saida)
#
#
# # Funcao logica OR
# s = np.array([-1, -1,
#               -1, 1,
#               1, -1,
#               1, 1], dtype=float)
# t = np.array([-1, -1,
#               1, 1,
#               1, 1,
#               1, 1], dtype=float)
#
# continuar = True
#
# nro_target = 2
# nro_entrada = 2
# cases = 4
#
# p = RNA(nro_target, nro_entrada)
# epoca = 1
#
# while continuar and epoca < 10:
#     print("----- Epoca ", epoca, " -----")
#     continuar = p.treinar(cases, s, t)
#     print("\nt: ", t)
#     print("w: ", p.w)
#     print("b: ", p.b, "\n")
#     epoca += 1
