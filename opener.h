#ifndef _OPENER_H
#define _OPENER_H

using namespace std;

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>

struct imageHeader{
    unsigned int width;
    unsigned int height;
    unsigned int bitDepth;
    unsigned int colorType;
    char IHDRChunk[25];
};

class image{
    public:
    char fileName[100];
    char* myImgData;
    char* hiddenMessage;
    bool containsMessage;
    unsigned int imageSize;

    image(char[]);
    void encode(void);
    void decode(void);

    private:
    imageHeader myImageHeader;
};

void endianFix(char *);

#endif
