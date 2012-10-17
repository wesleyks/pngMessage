#include "opener.h"

image::image(char filename[]){
    strcpy(fileName,filename);
    ifstream inFile;
    inFile.open(filename,ifstream::in | ifstream::binary);
    if (inFile.is_open()){
        char fileHeader[8];
        inFile.read(fileHeader,8);
        char validate[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        if (strncmp(validate,fileHeader,8) == 0) cout << "File is a png." << endl;
        else
        {
            cout << "File is not a png." <<endl;
            return;
        }
    }
    else
    {
        cout << "File cannot be opened." << endl;
        return;
    }

    inFile.read(myImageHeader.IHDRChunk,25);
    char width[4] = {myImageHeader.IHDRChunk[8],
                                    myImageHeader.IHDRChunk[9],
                                    myImageHeader.IHDRChunk[10],
                                    myImageHeader.IHDRChunk[11]};
    endianFix(width);
    myImageHeader.width = *(unsigned int*)width;
    char height[4] = {myImageHeader.IHDRChunk[12],
                                    myImageHeader.IHDRChunk[13],
                                    myImageHeader.IHDRChunk[14],
                                    myImageHeader.IHDRChunk[15]};
    endianFix(height);
    myImageHeader.height = *(unsigned int*)height;
    myImageHeader.bitDepth = (unsigned int)myImageHeader.IHDRChunk[16];
    myImageHeader.colorType = (unsigned int)myImageHeader.IHDRChunk[17];

    printf("image dimensions are: %ux%u\n",myImageHeader.width,myImageHeader.height);
    printf("bit depth: %u | color type: %u\n",myImageHeader.bitDepth,myImageHeader.colorType);
    /*
    if (!(myImageHeader.bitDepth==8&&myImageHeader.colorType==2)){
        cout << "Need an rgb image file with 24 bit depth" << endl;
        return;
    }*/

    bool dataReached = false;
    containsMessage = false;
    while (!dataReached){
        char chunkSizeArray[4];
        unsigned int chunkSize;

        inFile.read(chunkSizeArray,4);          //read in the size of the chunk
        chunkSize = *(unsigned int *)chunkSizeArray;
        endianFix((char*)&chunkSize);

        char chunkType[4];
        char chunkCRC[4];
        char chunkData[chunkSize];

        inFile.read(chunkType,4);               //read in the chunk type
        inFile.read(chunkData,chunkSize);       //read in the chunk data
        inFile.read(chunkCRC,4);                //read in the chunk crc
        printf("chunk type: %.4s chunk size: %u\n",chunkType, chunkSize);
        if(strncmp(chunkType,"IDAT",4)==0)
        {
            imageSize = chunkSize;
            myImgData = (char*)malloc(chunkSize);
            memcpy(myImgData,chunkData,chunkSize);
            dataReached = true;
        }
        else if(strncmp(chunkType,"sBPl",4)==0)
        {
            hiddenMessage = (char*)malloc(chunkSize);
            memcpy(hiddenMessage,chunkData,chunkSize);
            containsMessage = true;
        }

        //dataReached = true;
    }
    inFile.close();
}
void image::encode(void)
{
    unsigned int maxLength = imageSize/(2*8/myImageHeader.bitDepth)-1;
    char message[maxLength];
    char outFileName[100];
    printf("Name the output file(include '.png' at the end): ");
    cin.getline(outFileName,100);
    printf("The max length of your message is %u characters.\n\nEnter message: ",maxLength);
    cin.getline(message,maxLength);/*
    for (unsigned int i=0;i<maxLength;i++)
    {
        unsigned char dest1 = myImgData[2*i];
        unsigned char dest2 = myImgData[2*i+1];
        unsigned char messageChar = message[i];

        dest1 = dest1 & 0xF0;
        dest1 = dest1|((messageChar>>4)&0x0F);
        dest2 = dest2 & 0xF0;
        dest2 = dest2|((messageChar)&0x0F);
        myImgData[2*i] = dest1;
        myImgData[2*i+1] = dest2;
    }*/
    ////// cop out code
    unsigned int messageSize = strlen(message);
    char rawMessage[messageSize];
    strcpy(rawMessage,message);
    /////
    unsigned int imgsize = imageSize;
    printf ("size of imgdata: %u", imgsize);
    ofstream outFile;
    outFile.open(outFileName,ofstream::binary | ofstream::out);
    char fileHeader[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    char fileFooter[] = {0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
    outFile.write(fileHeader,8);                //write the file header
    outFile.write(myImageHeader.IHDRChunk,25);  //write the header chunk

    ///cop out code
    endianFix((char*)&messageSize);
    outFile.write((char*)&messageSize,4);
    outFile.write("sBPl",4);
    endianFix((char*)&messageSize);
    printf("message size is: %u\n",messageSize);
    outFile.write(rawMessage,messageSize);
    outFile.write("CRC ",4);
    ///

    endianFix((char*)&imageSize);
    outFile.write((char*)&imageSize,4);           //write the image size
    outFile.write("IDAT",4);                        //write IDAT
    outFile.write(myImgData,imgsize);         //write the pixels
    outFile.write("CRC ",4);                    //write the CRC
    outFile.write(fileFooter,12);
    outFile.close();
}
void image::decode(void)
{
    if(containsMessage)
    {
        cout << "\nThe secret message is: " << hiddenMessage << "\n" << endl;
    }
    else cout << "This image doesn't contain a secret message.\n";
}

void endianFix(char * input){
    const int checkEndian = 1;
    if ( !((*(char*)&checkEndian) == 0 )) { //if not big endian, we need to reverse stuff
        unsigned int size = sizeof(input);
        char temp[size];
        for (unsigned int i=0;i<size;i++){
            temp[i] = input[size-i-1];
        }
        for (unsigned int i=0;i<size;i++){
            input[i] = temp[i];
        }
    }
}
