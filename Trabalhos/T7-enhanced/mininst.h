
#ifndef T7_MLP_MININST_CPP_MININST_H
#define T7_MLP_MININST_CPP_MININST_H


#include "src/config.h"

#include <vector>
#include <string>
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


int loadData(const char *imageFile, const char *labelFile, vector<Sample> &v_data, bool use_binary, Number zero_representation = 0) {
    FILE *fim, *flb;
    uint32_t (*read_int32)(FILE *);
    uint32_t mnst_length;
    int mnst_w;
    int mnst_h;
    int mnst_im_size;
    uint32_t label_length;
    fim = fopen(imageFile, "rb");
    if (!fim) {
        cerr<<"arquivo "<<imageFile<<" não encontrado"<<endl;
        return -1;
    }
    flb = fopen(labelFile, "rb");
    if (!flb) {
        fclose(fim);
        cerr<<"arquivo "<<labelFile<<" não encontrado"<<endl;
        return -1;
    }

    read_int32 = read_int_little;
    mnst_length = read_int_little(fim);

    if (mnst_length == MNIST_MAGIC_NUMBER_LITTLE) {
        read_int32 = read_int_big2little;
    } else if (mnst_length != MNIST_MAGIC_NUMBER_BIGENDIAN) {
        cerr<<"Arquivo "<<imageFile<<" inválido"<<endl;
        return -2;
    }


    label_length = read_int_little(flb);
    if (label_length == MNIST_LABEL_MAGIC_NUMBER_LITTLE) {
        read_int32 = read_int_big2little;
    } else if (label_length != MNIST_LABEL_MAGIC_NUMBER_BIGENDIAN) {
        cerr<<"Arquivo "<<labelFile<<" inválido"<<endl;
        return -2;
    }
    mnst_length = read_int32(fim);
    label_length = read_int32(flb);
    if (mnst_length != label_length) {
        cerr<<"Arquivos não se correspondem"<<endl;
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

        v_data.push_back(Sample(className, label, std::vector<Number>(mnst_im_size), std::vector<Number>(use_binary ? 4 : 10)));
        Sample &dt = v_data[v_data.size() - 1];
        dt.valueName = label;
        fread(im, 1, mnst_im_size, fim);
        for (int i = 0; i < mnst_im_size; ++i) {
            dt.x[i] = im[i] ? 1 : zero_representation;
        }
        if (use_binary) {

            dt.y[0] = label & 0b0001 ? 1 : zero_representation;
            dt.y[1] = label & 0b0010 ? 1 : zero_representation;
            dt.y[2] = label & 0b0100 ? 1 : zero_representation;
            dt.y[3] = label & 0b1000 ? 1 : zero_representation;
        } else {

            for (unsigned char i = 0; i < 10; ++i) {
                dt.y[i] = (label == i) ? 1 : zero_representation;
            }
        }
    }

    delete im;
    fclose(flb);
    fclose(fim);
    return 0;
}

int loadData(std::string &path,const char *imageFile, const char *labelFile, vector<Sample> &v_data, bool use_binary, Number zero_representation = 0) {
    std::string imf = path+imageFile;
    std::string iml = path+labelFile;
    return loadData(imf.c_str(),iml.c_str(),v_data,use_binary,zero_representation);
}
#endif //T7_MLP_MININST_CPP_MININST_H
