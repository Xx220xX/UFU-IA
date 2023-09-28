from flask import Flask, render_template, send_file, abort, send_from_directory, request, jsonify, Response, redirect
from flask_cors import CORS
import os
from RedeNeural import *

app = Flask('Redes Neurais', static_url_path='/static', static_folder='static')

# Inicializa CORS permitiendo todos los orígenes, métodos y encabezados por defecto
cors = CORS(app)


@app.route('/select')
def select():
    return send_from_directory('static', 'escolherDataSet.html'), 200


@app.route('/')
def home():
    global dataSet
    header = request.args
    if 'datasetid' not in header:
        return redirect('/select')
    datase_id_int = int(header['datasetid'])

    if datase_id_int < 0 or datase_id_int >= len(dataSets):
        abort(404)
    dataSet = dataSets[datase_id_int]
    return send_from_directory('static', 'treinar.html'), 200


@app.route('/datasets', methods=['GET'])
def getDatasets():
    data = []
    dt: DataSet
    id = 0
    for dt in dataSets:
        data.append(dict(name=dt.name, nro_entrada=dt.nro_entrada, nro_saida=dt.nro_saida, id=id))
        id += 1
    return jsonify(data), 200


@app.route('/dataset', methods=['GET'])
def getDataset():
    if dataSet is None:
        abort(404)
    return jsonify({"name": dataSet.name})


@app.route('/favicon.ico')
def favicon(): return send_from_directory('static', 'favicon.jpeg', mimetype='image/jpeg'), 200


@app.route('/style.css')
def style():
    return send_from_directory('static', 'style.css'), 200


@app.route('/script.js')
def script(): return send_from_directory('static', 'script.js', mimetype='application/javascrip'), 200


@app.route('/background')
def background(): return send_from_directory('static', 'OIG.jpeg'), 200


def checkKeys(data, keys):
    for key in keys:
        if not key in data:
            return False, key
    return True, None


from Datasets import *

dataSet: DataSet = dataSets[0]


@app.route('/createDataSet', methods=['POST'])
def createDataSet():
    global dataSet, dataSets
    # Verifica se o content type do request é JSON
    if request.is_json:
        body = request.get_json(force=True)
        ok, misskey = checkKeys(body, ['nro_entrada', 'nro_saida', 'name'])
        if not ok:
            return jsonify({'erro': f"miss param '{misskey}'"}), 400
        dataSet = DataSet(body['nro_entrada'], body['nro_saida'], body['name'])
        dataSets.append(dataSet)
        return '{"message":"created"}', 200
    else:
        # Se o content type não for JSON, retorne um erro
        return jsonify({'erro': 'O corpo da requisição deve conter JSON'}), 400


@app.route('/addSample', methods=['POST'])
def addSample():
    global dataSet
    # Verifica se o content type do request é JSON

    if request.is_json:
        body = request.get_json(force=True)
        ok, misskey = checkKeys(body, ['samples', 'usedForTrain'])
        if not ok:
            return jsonify({'erro': f"miss param '{misskey}'"}), 400
        samples = body['samples']
        if not isinstance(samples, list):
            return jsonify({'erro': f"'samples' must be a list"}), 400
        for i in range(len(samples)):
            if not isinstance(samples[i], dict):
                return jsonify({'erro': f"'samples[{i}]' must be a json"}), 400
            ok, misskey = checkKeys(samples[i], ['name', 'input', 'target'])
            if not ok:
                return jsonify({'erro': f"'samples[{i}]' missing param '{misskey}'"}), 400
        if not dataSet:
            return '{"erro":"you must create the Data Set"}', 202
        samples = [Sample(sample['name'], sample['input'], sample['target']) for sample in samples]
        if not dataSet.addSample(samples, body['usedForTrain']):
            return jsonify({'erro': f"invalida data"}), 400
        return f'{{"message":"added {len(samples)}"}}', 200
    else:
        # Se o content type não for JSON, retorne um erro
        return jsonify({'erro': 'O corpo da requisição deve conter JSON'}), 400


@app.route('/treinar', methods=['POST'])
def treinar():
    if request.is_json:
        body = request.get_json(force=True)
        ok, misskey = checkKeys(body, ['max_epoca', 'winHate', 'alpha'])
        if not ok:
            return jsonify({'erro': f"miss param '{misskey}'"}), 400
        dataSet.config(RNA(dataSet.nro_entrada, dataSet.nro_saida, body['alpha']), body['max_epoca'], body['winHate'])
        if dataSet.treinarAssyncrono():
            return '{"message":"created"}', 200
        else:
            return jsonify({'erro': 'alread running'}), 400
    return jsonify({'erro': 'O corpo da requisição deve conter JSON'}), 400


@app.route('/update', methods=['GET'])
def update():
    offset = 0
    args = request.args
    if 'offset' in args:
        offset = int(args['offset'])
    info = dataSet.getInfo(offset)
    return jsonify(info), 200


@app.route('/network', methods=['GET'])
def network():
    w = []
    for i in range(dataSet.rna.nro_entrada):
        l = []
        for j in range(dataSet.rna.nro_saida):
            l.append(float(dataSet.rna.w[i][j]))
        w.append(l)
    b = []
    for j in range(dataSet.rna.nro_saida):
        b.append(float(dataSet.rna.b[0][j]))
    return jsonify(dict(w=w, b=b)), 200


app.run(debug=False, load_dotenv=False, host='0.0.0.0', port=80)
