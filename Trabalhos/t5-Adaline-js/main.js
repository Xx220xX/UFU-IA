/***
 Trabalho 05
 Utilizar a  base de observações
 Treinar uma Adaline com esta base de observações
 Traçar a linha de regressão linear
 Comparar os resultados de regressão com os obtidos utilizando as equações de a e b
 -   Encontrar o coeficiente de correlação de Pearson e o coeficiente de determinação.
 */
function random(minimo = 0, maximo = 1) {
    return Math.random() * (maximo - minimo) + minimo;
}

class Adaline {
    tamanhoSaida;
    tamanhoEntrada;
    alpha;
    w;
    b;
    x;
    y;
    t;

    constructor(tamanhoEntrada, tamanhoSaida, alpha = 0.1) {
        this.tamanhoEntrada = tamanhoEntrada;
        this.tamanhoSaida = tamanhoSaida;
        this.alpha = alpha;
        this.w = Array(tamanhoSaida);
        this.b = Array(tamanhoSaida);
        this.y = Array(tamanhoSaida);
        for (let i = 0; i < tamanhoSaida; i++) {
            this.w[i] = Array(tamanhoEntrada)
            for (let j = 0; j < tamanhoEntrada; j++) {
                let v = random(-0.8, 0.8);
                v += v > 0 ? 0.2 : -0.2;
                this.w[i][j] = v;
            }
            let v = random(-0.8, 0.8);
            v += v > 0 ? 0.2 : -0.2;
            this.b[i] = v;
        }
    }

    aprende(x, t) {
        this.x = x;
        this.t = t;
        let erro = 0
        for (let i = 0; i < this.tamanhoSaida; i++) {
            let z = this.b[i];
            for (let j = 0; j < this.tamanhoEntrada; j++) {
                z += this.w[i][j] * this.x[j];
            }
            this.y[i] = z;
            let dz = this.t[i] - z;
            erro += dz * dz * 0.5;
            for (let j = 0; j < this.tamanhoEntrada; j++) {
                let dw = dz * x[j];
                this.w[i][j] += dw * this.alpha;
            }
            this.b[i] += dz * this.alpha;
        }
        return erro / this.tamanhoSaida;
    }

    geraSaida(entrada) {
        this.x = entrada;
        for (let i = 0; i < this.tamanhoSaida; i++) {
            let z = this.b[i];
            for (let j = 0; j < this.tamanhoEntrada; j++) {
                z += this.w[i][j] * this.x[j];
            }
            this.y[i] = z;
        }

        return this.y.slice()
    }

    getEquation() {
        const b = this.w[0][0];
        const a = this.b[0]
        return (x) => x * b + a;
    }
}

function regressaoLinear(x, y) {
    let n = x.length;
    let sumX = 0, sumY = 0, sumX2 = 0, sumXY = 0;
    for (let i in x) {
        sumX += x[i];
        sumY += y[i];
        sumX2 += x[i] * x[i];
        sumXY += x[i] * y[i];
    }
    let Sxx = sumX2 - sumX * sumX / n; // desvio padrão de x
    let Sxy = sumXY - sumX * sumY / n; // desvio padrão de y
    let b = Sxy / Sxx;
    let a = (sumY / n) - b * (sumX / n);
    return [a, b]
}

function getFunctionRegressao(x, y) {
    const [a, b] = regressaoLinear(x, y);
    console.log('coeficientes ', a, b)
    return (x) => x * b + a;
}

function correlacaoPearson(x, y) {
    const n = x.length;
    const meanX = MEDIA(x);
    const meanY = MEDIA(y);
    let sumXY = 0;
    let sumX = 0;
    let sumY = 0;
    let sumX2 = 0;
    let sumY2 = 0;
    for (let i in x) {
        const deviationX = x[i] - meanX;
        const deviationY = y[i] - meanY;
        sumX += x[i];
        sumY += y[i];
        sumX2 += x[i] * x[i];
        sumY2 += y[i] * y[i];
        sumXY += x[i] * y[i];
    }
    return (n * sumXY - sumX * sumY) / (Math.sqrt(n * sumX2 - sumX * sumX) * Math.sqrt(n * sumY2 - sumY * sumY));
}


function MSE(predict, data) {
    let sum = 0;
    for (let i in predict) {
        sum += (predict[i] - data[i]) ** 2;
    }
    return sum / predict.length;
}

function RMSE(predict, data) {
    return Math.sqrt(MSE(predict, data));
}

function MAE(predict, data) {
    let sum = 0;
    for (let i in predict) {
        sum += Math.abs(predict[i] - data[i]);
    }
    return sum / predict.length;
}

function MEDIA(v) {
    let sum = 0;
    for (let i in v) {
        sum += v[i];
    }
    return sum / v.length;
}

function RSQUARED(predict, data) {
    const mediaT = MEDIA(data);
    let sumN = 0;
    let sumD = 0;
    for (let i in predict) {
        sumN += (data[i] - predict[i]) ** 2;
        sumD += (data[i] - mediaT) ** 2;
    }
    return sumN / sumD;
}

class Dados {
    treino;
    test;
    tamanhoEntrada;
    tamanhoSaida;
    rede;
    status;
    detais;

    constructor(tamanhoEntrada, tamanhoSaida, alpha, useTreinoAsTest = true) {
        this.tamanhoEntrada = tamanhoEntrada;
        this.tamanhoSaida = tamanhoSaida;
        this.treino = [];
        this.test = useTreinoAsTest ? this.treino : [];
        this.rede = null;
        this.status = {
            epoca: -1,
            erro: [],
            acerto: [],
        }
        this.details = [];
    }

    add(entrada, saida, used2train = true) {
        if (entrada.length != this.tamanhoEntrada || saida.length != this.tamanhoSaida) {
            throw "Invalid size";
        }
        if (used2train)
            this.treino.push([entrada, saida]);
        else
            this.test.push([entrada, saida]);
    }

    getInfo(offset) {
        return this.details.slice(offset);
    }

    async treinar(epocas = 100, alpha = 0.01) {
        this.rede = new Adaline(this.tamanhoEntrada, this.tamanhoSaida, alpha);
        this.status.epoca = 0;
        this.details.length = 0;
        await event_update_status('Treinando')
        let nShow = 200;

        let tenPercentEpocas = Math.floor(epocas / nShow);
        let expected = Array(this.test.length);
        let predict = Array(this.test.length);
        for (let i in this.test) {
            expected[i] = this.test[i][1][0];
        }

        for (let i = 0; i < epocas; i++) {
            tenPercentEpocas--;
            let info = {
                erro: 0,
                R2: 0,
                MSE: 0,
                MAE: 0,
                RMSE: 0,
                f: null
            }
            for (let sample of this.treino) {
                info.erro += this.rede.aprende(sample[0], sample[1])
            }
            this.status.epoca = i + 1;

            for (let j in this.test) {
                let y = this.rede.geraSaida(this.test[j][0])
                predict[j] = y[0];
            }
            info.R2 = RSQUARED(predict, expected)
            info.MSE = MSE(predict, expected)
            info.RMSE = RMSE(predict, expected)
            info.MAE = MAE(predict, expected)

            info.R2 /= this.test.length;
            info.MSE /= this.test.length;
            info.RMSE /= this.test.length;
            info.MAE /= this.test.length;
            info.f = this.rede.getEquation()
            this.details.push(info)
            if (tenPercentEpocas <= 0 || i + 1 === epocas || i === 0) {
                tenPercentEpocas = Math.floor(epocas / nShow);
                await dadosGrafico.addinfo(i, info)
                await event_update_status(`Treinando epoca ${i}`)
            }
        }
        await event_update_status('Finalizou')
        event_end_train();
    }
}

const baseObservacoes = new Dados(1, 1, 0.1);
baseObservacoes.treino.push([[-3.4557427], [-1.6239881]],
    [[-3.4822109], [-1.0618243]],
    [[-3.1973477], [-1.3302606]],
    [[-2.9263939], [-0.8884057]],
    [[-1.9737541], [-0.9198156]],
    [[-1.9189151], [-0.4542355]],
    [[-1.9781052], [-0.2378872]],
    [[-1.0670384], [-0.451436]],
    [[-1.4996849], [-0.1417166]],
    [[-1.1159294], [0.464144]],
    [[-0.781221], [0.8016596]],
    [[-0.5649162], [0.2934546]],
    [[-0.0050037], [1.1213746]],
    [[0.1969329], [0.8547785]],
    [[0.3321719], [1.2483002]],
    [[0.8792238], [1.8912561]],
    [[1.0729627], [1.8531781]],
    [[1.2940016], [1.9523057]],
    [[1.4121696], [1.8967559]],
    [[2.0611459], [2.7176396]],
    [[2.1194811], [2.2022322]],
    [[2.9184708], [2.5712507]],
    [[2.3294477], [3.3505474]],
    [[3.0532795], [3.5432877]],
    [[3.1210985], [3.4236653]],
    [[3.5576675], [4.0951618]],
    [[3.7092212], [3.9549787]],
    [[3.8422916], [4.5025232]],
    [[4.7783129], [3.7643277]],
    [[4.4976173], [4.666918]],
    [[4.683564], [4.5328549]],
    [[5.5428325], [4.9349832]],
    [[5.2959788], [5.3850333]],
    [[6.1256565], [4.7989586]],
    [[6.5558376], [5.7666838]],
    [[6.4062025], [6.0690915]],
    [[6.6951968], [5.9136599]],
    [[7.4498412], [6.1221843]],
    [[6.9709788], [6.5711131]],
    [[7.3426909], [6.0160765]],
    [[7.9904375], [6.722859]],
    [[8.3039034], [6.492281]],
    [[8.685398], [7.2993508]],
    [[9.1763368], [7.0432869]],
    [[9.0756499], [7.5261253]],
    [[9.2065044], [8.1722142]],
    [[9.530235], [8.1273208]],
    [[10.350861], [7.595554]],
    [[10.663104], [8.3859757]],
    [[10.343534], [8.3096467]]);
const baseX = Array(baseObservacoes.treino.length);
const baseY = Array(baseObservacoes.treino.length);

class DadosGrafico {

    constructor(x, y) {
        this.points = []
        this.minx = x[0];
        this.maxx = x[0]
        this.running = false;
        for (let i in x) {
            this.points.push({x: x[i], y: y[i]})
            if (x[i] > this.maxx) this.maxx = x[i];
            if (x[i] < this.minx) this.minx = x[i];
        }

        const fregressao = getFunctionRegressao(x, y);

        this.minx = this.minx - this.maxx * 0.1;
        this.maxx = this.maxx + this.maxx * 0.1;
        this.length = 0;
        this.epocas = []
        this.R2 = []
        this.MSE = []
        this.RMSE = []
        this.MAE = []
        this.adaline = [{}, {}]
        this.adalineEpocas = []
        this.timeUpdate = 100;
        this.regressao = [{x: this.minx, y: fregressao(this.minx)}, {x: this.maxx, y: fregressao(this.maxx)}]
        this.contextMetrics = document.getElementById('metrics').getContext('2d');
        const metricsPointRadios = 0;
        const metricsBorderRadius = 1;
        this.metrics = new Chart(this.contextMetrics, {
            type: 'line',
            data: {
                labels: this.epocas,
                datasets: [
                    {
                        label: 'MSE',
                        data: this.MSE,
                        borderColor: 'rgb(1,255,0)',
                        color: 'rgb(1,255,200)',
                        borderWidth: metricsBorderRadius,
                        pointRadius: metricsPointRadios,
                        showLine: true
                    },
                    {
                        label: 'RMSE',
                        data: this.RMSE,
                        borderColor: 'rgb(201,125,0)',
                        borderWidth: metricsBorderRadius,
                        pointRadius: metricsPointRadios,
                        showLine: true
                    }, {
                        label: 'MAE',
                        data: this.MAE,
                        borderColor: 'rgb(201,125,200)',
                        borderWidth: metricsBorderRadius,
                        pointRadius: metricsPointRadios,
                        showLine: true
                    }, {
                        label: 'R²',
                        data: this.R2,
                        borderColor: 'rgb(140,15,20)',
                        borderWidth: metricsBorderRadius,
                        pointRadius: metricsPointRadios,
                        showLine: true
                    }
                ]
            },
            options: {
                scales: {
                    x: {
                        type: 'linear',
                        position: 'bottom',
                        title: {
                            display: true,
                            text: 'Época',
                        },
                    },
                    y: {
                        type: 'linear',
                        position: 'left',

                    },
                },
                animation: {
                    duration: 400, // Define a duração da animação para 500 milissegundos (0,5 segundos)
                }
            }
        });
        this.contextMapa = document.getElementById('mapa').getContext('2d');
        this.mapa = new Chart(this.contextMapa, {
            type: 'line',
            data: {
                datasets: [
                    {
                        label: `Base de observações, Pearson ${coefPearson.toExponential(2)}`,
                        data: this.points,
                        pointRadius: 3,
                        borderColor: 'rgb(0,0,0)',
                        color: 'rgb(247,255,0)',
                        borderWidth: 1,
                        showLine: false
                    }, {
                        label: 'Regressão Linear',
                        data: this.regressao,
                        borderColor: 'rgb(205,10,0)',
                        color: 'rgb(205,10,0)',
                        pointRadius: 0,
                        borderWidth: 1,
                        showLine: true
                    }, {
                        label: 'Adaline',
                        data: this.adaline,
                        borderColor: 'rgb(16,148,241)',
                        borderWidth: 2,
                        showLine: true
                    },
                ]
            },
            options: {
                scales: {
                    x: {
                        min: this.minx,
                        max: this.maxx,
                        type: 'linear',
                        position: 'bottom',
                    },
                    y: {
                        type: 'linear',
                        position: 'left',
                    }
                }
            }
        });

    }

    clear() {
        this.MSE.length = 0;
        this.MAE.length = 0;
        this.R2.length = 0;
        this.epocas.length = 0;
        this.adalineEpocas.length = 0;
        this.mapa.update()
        this.metrics.update()
        progress.length = 0;
    }

    addinfo(epoca, info) {
        this.R2.push(info.R2);
        this.MSE.push(info.MSE);
        this.RMSE.push(info.RMSE);
        this.MAE.push(info.MAE);
        this.adalineEpocas.push([{x: this.minx, y: info.f(this.minx)}, {
            x: this.maxx,
            y: info.f(this.maxx)
        }])
        this.epocas.push(epoca);
        this.length = this.epocas.length;
        this.metrics.update('none')
        progress.length += 1;
        progress.updatePosition(progress.length - 1)
    }

    show(epoca) {

        if (epoca >= this.adalineEpocas.length) {
            console.log('Epoca invalida', epoca, this.adalineEpocas.length)
            return
        }
        this.adaline[0] = this.adalineEpocas[epoca][0];
        this.adaline[1] = this.adalineEpocas[epoca][1];
        this.mapa.update('none');
    }
}

class ProgressBar {
    constructor(customProgressBar = '.custom-progress-bar', progressFill = 'progressFill', progressThumb = 'progressThumb') {
        this.customProgressBar = document.querySelector(customProgressBar);
        this.progressFill = document.getElementById(progressFill);
        this.progressThumb = document.getElementById(progressThumb);
        this.progressActive = false;
        this.st = document.getElementById("status");
        this.length = -1;
        this.last = -1;
        this.updateProgress(0);
        this.customProgressBar.addEventListener('mousedown', (e) => {
            const clickX = e.clientX - this.customProgressBar.getBoundingClientRect().left;
            this.updateProgress(clickX);
            this.progressActive = true;
            // st.innerText = "down";
        });
        document.addEventListener('mouseup', (e) => {
            if (this.progressActive) {
                this.progressActive = false;
                // this.st.innerText = "up";
            }
        });
        document.addEventListener('mousemove', (e) => {
            if (!this.progressActive) return;
            // this.st.innerText = "move";
            const clickX = e.clientX - this.customProgressBar.getBoundingClientRect().left;
            this.updateProgress(clickX, true)
        });
    }

    updateProgress(clickX) {
        const progressBarWidth = this.customProgressBar.offsetWidth;
        let progress = (clickX / progressBarWidth);
        let i = Math.round(progress * (this.length - 1));
        i = i < 0 ? 0 : i >= this.length ? this.length - 1 : i;
        this.updatePosition(i, true)
    }


    updatePosition(i, isuser = false) {
        let progress = 0;
        console.log(i)
        if (i === this.last) return;
        if (this.length < 0) {
            i = -1;
            progress = 0;
            this.st.innerText = ``
        } else {
            this.last = i;
            let epoca = dadosGrafico.epocas[i];
            progress = (i) / (this.length - 1) * 100;
            document.getElementById("st_epoca").innerText = epoca;
            document.getElementById("st_MSE").innerText = baseObservacoes.details[epoca].MSE.toExponential(3)
            document.getElementById("st_RMSE").innerText = baseObservacoes.details[epoca].RMSE.toExponential(3)
            document.getElementById("st_R2").innerText = baseObservacoes.details[epoca].R2.toExponential(3)
            document.getElementById("st_MAE").innerText = baseObservacoes.details[epoca].MAE.toExponential(3)
            if (this.length > 0) dadosGrafico.show(i);

        }
        this.progressFill.style.width = `${progress}%`;
        this.progressThumb.style.left = `${progress}%`;

    }
}

function aguardarRenderizacao() {
    return new Promise((resolve) => {
        requestAnimationFrame(() => {
            resolve();
        });
    });
}

async function event_update_status(value) {
    out_status.innerText = "STATUS:'" + value + "'";
    await aguardarRenderizacao();
}

function event_end_train() {
    btn_start.disabled = false;
    dadosGrafico.running = false;

}

async function call_treinar() {
    btn_start.disabled = true;
    dadosGrafico.running = true;
    dadosGrafico.clear()
    progress.length = -1;
    progress.last = -1;
    progress.updatePosition(0)

    if (inp_alpha.value === '')
        inp_alpha.value = 0.01;
    if (inp_MaxEpocas.value === '')
        inp_MaxEpocas.value = 100;
    const maxEpocas = parseInt(inp_MaxEpocas.value);
    const alpha = parseFloat(inp_alpha.value);
    baseObservacoes.treinar(maxEpocas, alpha);

}


const btn_start = document.getElementById("btn-start")
const inp_MaxEpocas = document.getElementById("maximo_epocas")
const inp_alpha = document.getElementById("alpha")
const out_status = document.getElementById("status-area")

const progress = new ProgressBar();
// achar regressão
for (let i in baseObservacoes.treino) {
    baseX[i] = baseObservacoes.treino[i][0][0];
    baseY[i] = baseObservacoes.treino[i][1][0];
}
const coefPearson = correlacaoPearson(baseX, baseY)
const coefDeterminacao = coefPearson ** 2;

const dadosGrafico = new DadosGrafico(baseX, baseY);

document.getElementById("st_pearson").innerText = coefPearson.toExponential(3)
document.getElementById("st_determinacao").innerText = coefDeterminacao.toExponential(3)




