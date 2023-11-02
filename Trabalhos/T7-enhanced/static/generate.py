from redes import  *
import base64, struct, json

# Função para codificar um array de floats em base64
def encode_array_to_base64(float_array):
    # Converter os floats em bytes
    byte_array = bytearray()
    for num in float_array:
        byte_array.extend(struct.pack('f', num))

    # Codificar os bytes em base64
    base64_string = base64.b64encode(byte_array).decode('utf-8')
    return base64_string


for mlp in redes:
    for layer in mlp['rede']:
        w = []
        for col in layer['w']:
            w.extend(col)
        layer['in'] = len(w)
        layer['out'] = len(layer['w'][0])
        layer['w'] = encode_array_to_base64(w)


print('const redes =', json.dumps(redes,indent=4),file=open('redes.js','w'))