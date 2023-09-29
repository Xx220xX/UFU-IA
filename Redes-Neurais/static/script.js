const buttonTreinar = document.getElementById('treinarBtn');
const buttonCalcular = document.getElementById('calcularBtn');
const dropdownNeuronio = document.getElementById("dropdown-neuronio")
const inp_maximo_epocas = document.getElementById("maximo_epocas");
const inp_expectWinRate = document.getElementById("expectWinRate");
const inp_alpha = document.getElementById("alpha");
const title = document.getElementById("titulo");
const form_rede = document.getElementById("form_rede");
const epocas = [];
const erros = [];
const acertos = [];
const curva_neuronio = [];
const eixo_neuronio = [];
const ponto = [{}]
const pontosTreinoA = [{}]
const pontosTreinoB = [{}]

const dataset = {}


const dadosDispersao = [{x: 1, y: 10}];

// Configuração do gráfico de linha
const ctxLinha = document.getElementById('graficoLinha').getContext('2d');
const verde = 'rgb(150,243,17)';
const azul = 'rgba(75, 192, 192, 1)';
const graficoLinha = new Chart(ctxLinha, {
    type: 'line',
    data: {
        labels: epocas,
        datasets: [{
            label: 'Erro',
            yAxisID: 'y-axis-1',
            data: erros,
            borderColor: azul,
            borderWidth: 2
        }, {
            label: 'Taxa de acertos',
            yAxisID: 'y-axis-2',
            data: acertos,
            borderColor: verde,
            borderWidth: 2
        }
        ]
    },
    options: {
        scales: {
            'y-axis-1': {
                type: 'linear',
                position: 'left', // Eixo esquerdo

            },
            'y-axis-2': {
                type: 'linear',
                position: 'right', // Eixo direito

            }

        },
        animation: {
            duration: 400, // Define a duração da animação para 500 milissegundos (0,5 segundos)
        }
    }
});
const ctxrede = document.getElementById('grafico da rede').getContext('2d');

const graficoRede = new Chart(ctxrede, {
    type: 'line',
    data: {
        labels: eixo_neuronio,
        datasets: [{
            label: 'Curva do Neurônio',
            data: curva_neuronio,
            borderColor: verde,
            borderWidth: 2
        }, {
            label: 'Pontinho',
            data: ponto,
            borderWidth: 2,
            pointRadius: 5,
            borderColor: 'rgb(0,0,0)',
            backgroundColor: 'rgb(255,221,0)',
            showLine: false
        }, {
            label: 'Dados +1',
            data: pontosTreinoA,
            borderWidth: 2,
            pointRadius: 5,
            borderColor: 'rgb(0,0,0)',
            backgroundColor: 'rgb(48,167,241)',
            showLine: false
        }, {
            label: 'Dados -1',
            data: pontosTreinoB,
            borderWidth: 2,
            pointRadius: 5,
            borderColor: 'rgb(0,0,0)',
            backgroundColor: 'rgb(255,0,0)',
            showLine: false
        },

        ]
    },
    options: {
        scales: {
            x: {
                min: -10,
                max: 10
            },
            y: {
                min: -6,
                max: 6
            }

        },
        animation: {
            duration: 400,
        }
    }
});


const network = {}

const randInt = (max) => Math.floor(Math.random() * max);

function updateStatusTreino(lastrun = false) {
    buttonTreinar.disabled = true;
    // fazer um get e obter a lista de pontos atualizada passando o len
    fetch(`/update?offset=${epocas.length}`, {
        method: 'GET',
        headers: {'Content-Type': 'application/json'},
    })
        .then(response => response.json())
        .then(response => {
            let finalizouTreino = false;

            for (let i in response.erroPorEpoca) {
                epocas.push(epocas.length);
                erros.push(response.erroPorEpoca[i]);
                acertos.push(response.acertoPorEpoca[i]);
            }
            if (response.running === false) {
                finalizouTreino = true;
            }


            //dadosDispersao.push({x: epocas.length, y: Math.random() * 10});
            graficoLinha.update();
            //graficoDispersao.update();
            if (lastrun) {
                buttonTreinar.disabled = false;
                return;
            }
            if (!finalizouTreino) {
                setTimeout(updateStatusTreino, 100);
            } else {
                buttonTreinar.disabled = false;
                pegarRede();
            }
        })
        .catch(err => {
            buttonTreinar.disabled = false;
            console.error(err)
        });
}


// Evento de clique no botão "Treinar"
buttonTreinar.addEventListener('click', () => {
    buttonTreinar.disabled = true;
    epocas.splice(0, epocas.length);
    erros.splice(0, erros.length);
    fetch("/treinar", {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({
            'max_epoca': parseInt(inp_maximo_epocas.value),
            'winHate': parseFloat(inp_expectWinRate.value),
            'alpha': parseFloat(inp_alpha.value)
        })
    })
        .then(response => response.json())
        .then(response => {
            updateStatusTreino();
        })
        .catch(err => {
            buttonTreinar.disabled = false;
            console.error(err)
        });


});
buttonCalcular.addEventListener('click', () => {
    if (!('w' in network && 'b' in network)) return;
    let x = []
    let w = network.w;
    let b = network.b;
    for (let i in w) {
        let el = document.getElementById(`rede-x[${i}]`);
        x.push(parseFloat(el.value))
    }
    ponto[0].x = x[0];
    ponto[0].y = x[1];
    graficoRede.update()
    let saida = [];
    for (j in b) {
        let y = b[j];
        for (let i in w) {
            y += w[i][j] * x[i];
        }
        saida.push(y)
    }

    draw(x, saida, w, b)
});


const canvas = new fabric.Canvas('neuralCanvas');


var rede = {};

function draw(entrada, saida, w, b) {
    const sep_saida = canvas.height / saida.length;
    const width = canvas.width / 5;
    const raioNeuronio = 40;
    const sep_entrada = canvas.height / (entrada.length + 1);
    canvas.clear();
    rede = {};
    const textSize = 20;
    rede.neuronios = [];
    rede.entradas = [];
    for (let i = 0; i < saida.length; i++) {
        const x = canvas.width - raioNeuronio * 2;
        const y = sep_saida * (i + 0.5);

        let cor = saida[i] > 0 ? "blue" : "red";

        let circulo = new fabric.Circle({
            radius: raioNeuronio,
            fill: cor,
            left: x,
            top: y
        });
// Criar um texto
        let texto = new fabric.Text(`${saida[i].toFixed(3)}`, {
            left: circulo.left + circulo.width / 2, // Ajustar a posição horizontal do texto
            top: circulo.top + circulo.height / 2,  // Ajustar a posição vertical do texto
            fill: 'Black',        // Cor do texto
            textAlign: 'center',          // Centralizar horizontalmente
            textBaseline: 'middle',       // Centralizar verticalmente
            fontSize: textSize,
            originX: 'center', // Define o ponto de origem horizontal no centro
            originY: 'center', // Define o ponto de origem vertical no centro
        });
// Agrupar o círculo e o texto
        let grupo = new fabric.Group([circulo, texto], {
            selectable: false       // Impedir a seleção do grupo
        });
        rede.neuronios.push(grupo);
        canvas.add(grupo);

    }
    for (let i = 0; i < entrada.length; i++) {
        const x = 0;
        const y = sep_entrada * (i + 1.5);
        let cor = entrada[i] > 0 ? "blue" : "red";
        let retangulo = new fabric.Rect({
            left: x, // Posição horizontal
            top: y,  // Posição vertical
            width: raioNeuronio, // Raio do círculo
            height: raioNeuronio, // Raio do círculo
            fill: cor, // Cor de preenchimento
            stroke: 'black', // Cor da borda
            strokeWidth: 2 // Largura// da borda
        });
// Criar um texto
        let texto = new fabric.IText(`${entrada[i].toFixed(3)}`, {
            left: retangulo.left + retangulo.width / 2, // Ajustar a posição horizontal do texto
            top: retangulo.top + retangulo.height / 2,  // Ajustar a posição vertical do texto
            fill: 'Black',        // Cor do texto
            textAlign: 'center',          // Centralizar horizontalmente
            textBaseline: 'middle',       // Centralizar verticalmente
            fontSize: textSize,
            originX: 'center', // Define o ponto de origem horizontal no centro
            originY: 'center', // Define o ponto de origem vertical no centro
            editable: true
        });
// Agrupar o círculo e o texto
        let grupo = new fabric.Group([retangulo, texto], {
            selectable: false,       // Impedir a seleção do grupo
            editable: true

        });
        grupo.on('mousedown', function (options) {
            let target = options.target;
            if (target && target.type === 'i-text') {
                target.enterEditing();
                target.selectAll();
                canvas.renderAll(); // Renderize o canvas para iniciar a edição
            }
        });
        rede.entradas.push(grupo);
        canvas.add(grupo);
    }


    //bias
    let x = width;
    let y = sep_entrada * (.5);
    let cor = "blue";
    let retangulo = new fabric.Rect({
        left: x, // Posição horizontal
        top: y,  // Posição vertical
        width: raioNeuronio, // Raio do círculo
        height: raioNeuronio, // Raio do círculo
        fill: cor, // Cor de preenchimento
        stroke: 'black', // Cor da borda
        strokeWidth: 2 // Largura// da borda
    });
// Criar um texto
    let texto = new fabric.Text(`Bias`, {
        left: retangulo.left + retangulo.width / 2, // Ajustar a posição horizontal do texto
        top: retangulo.top + retangulo.height / 2,  // Ajustar a posição vertical do texto
        fill: 'Black',        // Cor do texto
        textAlign: 'center',          // Centralizar horizontalmente
        textBaseline: 'middle',       // Centralizar verticalmente
        fontSize: textSize,
        originX: 'center', // Define o ponto de origem horizontal no centro
        originY: 'center', // Define o ponto de origem vertical no centro
    });
// Agrupar o círculo e o texto
    let grupo = new fabric.Group([retangulo, texto], {
        selectable: false       // Impedir a seleção do grupo
    });
    rede.entradas.splice(0, 0, grupo);
    canvas.add(grupo);

    // conectar
    let pesos = copiarMatriz(w);
    pesos.splice(0, 0, b);

    // Crie uma linha para conectar os grupos
    for (let i = 0; i < rede.entradas.length; i++) {
        for (let j = 0; j < rede.neuronios.length; j++) {
            let input = rede.entradas[i];
            let output = rede.neuronios[j];
            let peso = pesos[i][j];

            let line = new fabric.Line([input.left + input.width, input.top + input.height / 2,
                output.left, output.top + output.height / 2], {
                stroke: 'black',
                strokeWidth: 2,
            });

// Calcule o ângulo da linha
            let angle = Math.atan2(output.top - input.top, output.left - input.left) * (180 / Math.PI);

// Crie o texto e configure o ângulo
            let text = new fabric.Text(`${peso}`, {
                left: line.left + line.width / 2,
                top: line.top + line.height / 2,
                angle: angle,
                originX: 'center',
                originY: 'bottom',
                fontSize: 14,
            });
            let linhas = new fabric.Group([line, text], {selectable: false});
            canvas.add(linhas);
        }
    }

}

function pegarRede() {
    // draw([1, 2, -2.1], [-1, 1.2], [[1, 2], [-1, 2], [3, 2.]], [0.1, 0.4])
    fetch(`/network`, {
        method: 'GET',
        headers: {'Content-Type': 'application/json'},
    }).then(response => response.json())
        .then(response => {
            network.w = response.w;
            network.b = response.b;
            let form = ''
            for (let i in network.w) {
                form += `<input id="rede-x[${i}]" type="number" class="form-control" placeholder="x${i}" aria-label="Username">\n`;
            }
            form_rede.innerHTML = form;
            desenharRede();
            setTimeout(updateStatusTreino, 100, true);
        })
        .catch(err => {
            console.error(err);
        });

}

function geraCurva(j) {
    let x0 = [-6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6]
    eixo_neuronio.splice(0, eixo_neuronio.length);
    curva_neuronio.splice(0, curva_neuronio.length);
    pontosTreinoA.splice(0, pontosTreinoA.length);
    pontosTreinoB.splice(0, pontosTreinoB.length);

    let fun = (x, w, b) => -(x * w[0][j] + b[j]) / w[1][j]
    for (let i in x0) {
        let y = fun(x0[i], network.w, network.b);
        //console.log(x0[i],y,network.w[0][j]*x0[i]+network.w[1][j]*y+network.b[j])
        eixo_neuronio.push(x0[i]);
        curva_neuronio.push(y);
    }

    for (let s of dataset.train) {
        console.log(s)
        if (s[1][0] > 0) {
            pontosTreinoA.push({x: s[0][0], y: s[0][1]})
        } else {
            pontosTreinoB.push({x: s[0][0], y: s[0][1]})
        }
    }

    graficoRede.update();

}

function desenharRede() {
    dropdownNeuronio.innerHTML = ''
    for (let i in network.b) {
        let a = document.createElement('a');
        a.className = "dropdown-item";
        a.addEventListener('click', () => {
            geraCurva(i);
        });
        a.innerHTML = `Neuronio ${i}`
        dropdownNeuronio.appendChild(a);
    }
    geraCurva(0)

}

function copiarMatriz(matriz) {
    var novaMatriz = [];
    for (var i = 0; i < matriz.length; i++) {
        novaMatriz[i] = matriz[i].slice(); // Use o método slice para copiar os elementos da linha
    }
    return novaMatriz;
}

fetch(`/dataset`, {
    method: 'GET',
    headers: {'Content-Type': 'application/json'},
}).then(response => response.json())
    .then(response => {
        title.innerHTML = response.name;
        dataset.name = response.name;
        dataset.train = response.train;
    })
    .catch(err => {
        console.error(err);
        window.location.href = '/';
    });
// network.w = [
//     [0.1],
//     [0.2]
// ];
// network.b = [0.1]
// geraCurva(0)

