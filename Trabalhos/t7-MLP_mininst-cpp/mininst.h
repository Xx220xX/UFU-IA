
#ifndef T7_MLP_MININST_CPP_MININST_H
#define T7_MLP_MININST_CPP_MININST_H


#include "mlp/config.h"
#include <vector>
using namespace std;
#define MNIST_MAGIC_NUMBER_LITTLE  0x03080000
#define MNIST_MAGIC_NUMBER_BIGENDIAN  0x00000803
#define MNIST_LABEL_MAGIC_NUMBER_LITTLE 0x01080000
#define MNIST_LABEL_MAGIC_NUMBER_BIGENDIAN 0x00000801

struct MININST {
    int32_t length;
    int16_t w;
    int16_t h;

};
struct Data {
    string className;
    int valueName;
    vector<Number> entrada, saida;
};

uint32_t read_int_big2little(FILE *f) {
    uint32_t num;
    fread(&num, 4, 1, f);
    return ((num >> 24) & 0xFF) |
           ((num >> 8) & 0xFF00) |
           ((num << 8) & 0xFF0000) |
           ((num << 24) & 0xFF000000);
}

uint32_t read_int_little(FILE *f) {
    uint32_t num;
    fread(&num, 4, 1, f);
    return num;
}

int loadData(const char *imageFile, const char *labelFile, vector<Data> &v_data, bool use_binary) {
    FILE *fim, *flb;
    uint32_t (*read_int32)(FILE *);
    uint32_t mnst_length;
    int mnst_w;
    int mnst_h;
    int mnst_im_size;
    uint32_t label_length;
    fopen_s(&fim, imageFile, "rb");
    if (!fim)return -1;
    fopen_s(&flb, labelFile, "rb");
    if (!flb) {
        fclose(fim);
        return -1;
    }

    read_int32 = read_int_little;
    mnst_length = read_int_little(fim);

    if (mnst_length == MNIST_MAGIC_NUMBER_LITTLE) {
        read_int32 = read_int_big2little;
    } else if (mnst_length != MNIST_MAGIC_NUMBER_BIGENDIAN) {
        return -2;
    }


    label_length = read_int_little(flb);
    if (label_length == MNIST_LABEL_MAGIC_NUMBER_LITTLE) {
        read_int32 = read_int_big2little;
    } else if (label_length != MNIST_LABEL_MAGIC_NUMBER_BIGENDIAN) {
        return -2;
    }
    mnst_length = read_int32(fim);
    label_length = read_int32(flb);
    if (mnst_length != label_length) {
        return -3;
    }
    std::cout << "Read: " << label_length << std::endl;
    mnst_w = read_int32(fim);
    mnst_h = read_int32(fim);
    mnst_im_size = mnst_h * mnst_w;
    char className[] = "0";
    char label;
    unsigned char *im;
    im = new unsigned char[mnst_h * mnst_w];
    for (int n = 0; n < mnst_length; ++n) {
        fread(&label, 1, 1, flb);
        className[0] = '0' + label;
        v_data.push_back(Data(className,label, std::vector<Number>(mnst_im_size), std::vector<Number>(use_binary ? 4 : 10)));
        Data &dt = v_data[v_data.size() - 1];
        fread(im, 1, mnst_im_size, fim);
        for (int i = 0; i < mnst_im_size; ++i) {
            dt.entrada[i] = im[i] ? 1 : -1;
        }
        if (use_binary) {

            dt.saida[0] = label & 0b0001   ? 1 : -1;;
            dt.saida[1] = label & 0b0010   ? 1 : -1;;
            dt.saida[2] = label & 0b0100   ? 1 : -1;;
            dt.saida[3] = label & 0b1000   ? 1 : -1;;
        } else {

            for (unsigned char i = 0; i < 10; ++i) {
                dt.saida[i] = (label == i) ? 1 : -1;
            }
        }
    }

    delete im;
    fclose(flb);
    fclose(fim);
    return 0;
}

#endif //T7_MLP_MININST_CPP_MININST_H
