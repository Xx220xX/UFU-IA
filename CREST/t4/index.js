const buttonTreinar = document.getElementById('treinarBtn');
const inp_maximo_epocas = document.getElementById("maximo_epocas");
const inp_expectWinRate = document.getElementById("expectWinRate");
const inp_alpha = document.getElementById("alpha");

const epocas = [];
const erros = [];
const dadosDispersao = [{x: 1, y: 10}];

// Configuração do gráfico de linha
const ctxLinha = document.getElementById('graficoLinha').getContext('2d');
const graficoLinha = new Chart(ctxLinha, {
    type: 'line',
    data: {
        labels: epocas,
        datasets: [{
            label: 'Erro',
            data: erros,
            borderColor: 'rgba(75, 192, 192, 1)',
            borderWidth: 2
        }]
    }
});

// Configuração do gráfico de dispersão
const ctxDispersao = document.getElementById('graficoDispersao').getContext('2d');
const graficoDispersao = new Chart(ctxDispersao, {
    type: 'scatter',
    data: {
        datasets: [{
            label: 'Dados de Dispersão',
            data: dadosDispersao,
            backgroundColor: 'rgba(255, 99, 132, 0.7)'
        }]
    },
    options: {
        scales: {
            x: {
                type: 'linear',
                position: 'bottom'
            },
            y: {
                min: 0,
                max: 12
            }
        }
    }
});


const randInt = (max) => Math.floor(Math.random() * max);

function updateStatusTreino() {
    // fazer um get e obter a lista de pontos atualizada passando o len
    const len = randInt(50);
    let req_erro = [];
    let finalizouTreino = false;
    for (let i = 0; i < len; i++) {
        req_erro.push(100 * Math.exp((-epocas.length - i) / 200));
    }
    for (let e of req_erro) {
        epocas.push(epocas.length);
        erros.push(e);
    }
    if (epocas.length > 200) {
        finalizouTreino = true;
    }


    //dadosDispersao.push({x: epocas.length, y: Math.random() * 10});
    graficoLinha.update();
    //graficoDispersao.update();
    if (!finalizouTreino)
        setTimeout(updateStatusTreino, 100);
    else {
        buttonTreinar.disabled = false;
    }
}


// Evento de clique no botão "Treinar"
buttonTreinar.addEventListener('click', () => {
    buttonTreinar.disabled = true;
    epocas.splice(0, epocas.length);
    erros.splice(0, erros.length);


    let _data = {
        maximo_epocas: inp_maximo_epocas.value,
        alpha: inp_alpha.value,
        expectWinRate: expectWinRate.value,
        code: 200
    }
    let url = "/teste";
    const options = {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify(_data)
    };
    console.log(options)

    fetch(url, options)
        .then(response => response.json())
        .then(response => console.log(response))
        .catch(err => console.error(err));

    updateStatusTreino();

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
        console.log(x, y, canvas.width, canvas.height)
        let cor = saida[i] > 0 ? "blue" : "red";

        let circulo = new fabric.Circle({
            radius: raioNeuronio,
            fill: cor,
            left: x,
            top: y
        });
// Criar um texto
        let texto = new fabric.Text(`${saida[i]}`, {
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
        console.log(x, y, canvas.width, canvas.height)
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
        let texto = new fabric.IText(`${entrada[i]}`, {
            left: retangulo.left + retangulo.width / 2, // Ajustar a posição horizontal do texto
            top: retangulo.top + retangulo.height / 2,  // Ajustar a posição vertical do texto
            fill: 'Black',        // Cor do texto
            textAlign: 'center',          // Centralizar horizontalmente
            textBaseline: 'middle',       // Centralizar verticalmente
            fontSize: textSize,
            originX: 'center', // Define o ponto de origem horizontal no centro
            originY: 'center', // Define o ponto de origem vertical no centro
            editable:true
        });
// Agrupar o círculo e o texto
        let grupo = new fabric.Group([retangulo, texto], {
            selectable: false,       // Impedir a seleção do grupo
            editable:true

        });
        grupo.on('mousedown', function (options) {
            let target = options.target;
            console.log(target)
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
    console.log(x, y, canvas.width, canvas.height)
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
    let pesos = w;
    pesos.splice(0, 0, b);

    // Crie uma linha para conectar os grupos
    for (let i = 0; i < rede.entradas.length; i++) {
        for (let j = 0; j < rede.neuronios.length; j++) {
            let input = rede.entradas[i];
            let output = rede.neuronios[j];
            let peso = pesos[i][j];
            console.log(input)
            let line = new fabric.Line([input.left + input.width , input.top + input.height / 2,
                output.left, output.top + output.height / 2], {
                stroke: 'black',
                strokeWidth: 2,
            });

// Calcule o ângulo da linha
            let angle = Math.atan2(output.top - input.top, output.left - input.left) * (180 / Math.PI);

// Crie o texto e configure o ângulo
            let text = new fabric.Text(`${peso}`, {
                left: line.left+line.width/2,
                top: line.top+line.height/2,
                angle: angle,
                originX: 'center',
                originY: 'bottom',
                fontSize:14,
            });
            let linhas = new fabric.Group([line, text],{selectable: false});
            canvas.add(linhas);
        }
    }

}

draw([1, 2, -2.1], [-1, 1.2], [[1, 2], [-1, 2], [3, 2.]], [0.1, 0.4])
