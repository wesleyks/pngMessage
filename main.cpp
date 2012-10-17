#include <iostream>
#include "opener.h"
using namespace std;

int main()
{
    bool run = true;
    char response[100];
    while(run){
        char fileName[100];
        image* myImage;
        cout << "0: Exit.\n1: encode message.\n2: decode message.\n->";
        cin.getline(response,100);
        switch(response[0]){
            case '1':
                cout << "Enter name of the file you wish to encode to. This program will make a copy of it and encode the message into the copy(must be .png): " ;
                cin.getline(fileName,100);
                myImage = new image(fileName);
                myImage->encode();
                delete myImage;
                break;
            case '2':
                cout << "Enter name of the file you wish to decode: " ;
                cin.getline(fileName,100);
                myImage = new image(fileName);
                myImage->decode();
                delete myImage;
                break;
            case '0':
                run = false;
                break;
        }

    }
    return 0;
}
