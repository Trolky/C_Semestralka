#include "bmp.h"

int loadBMP(FILE *image ,struct BMPHeader *header){

    fread(header, sizeof(struct BMPHeader), 1, image); /*Přečte hlavičku bmp souboru*/

    if (header->signature != BMP_HEADER_SIGNATURE || header->bitsPerPixel != 24) { 
        return 0;
    }

    return 1;
}

int getNumberOfPixels(struct BMPHeader imageHeader){ /*Získá počet pixelů*/
   return (imageHeader.height * imageHeader.width);
}

int hideMessageInBMP(FILE* image, FILE* binaryMessage){
    struct BMPHeader header;
    int byte;
    char* binaryChar;
    long fileSize;

    if (!image || !binaryMessage) {
        return 0;
    }

    if (loadBMP(image,&header) == 0){ /*Kontrola jestli se jedná o BMP obrázek*/
        return 2;
    }

    if ((getNumberOfPixels(header))<((8*ftell(binaryMessage))+(2*NORMAL_BYTE_LENGTH)+CRC_LENGTH)){ /*Kontrola jestli se zpráva vejde do obrázku*/
        return 3;
    }
    fseek(image, header.dataOffset, SEEK_SET); /*Najde začátek obrázku*/

    fseek(binaryMessage,0, SEEK_END); /*Najde začátek zprávy*/
    fileSize = ftell(binaryMessage);
    fseek(binaryMessage,0, SEEK_SET); 

    /*Zapsání rozpoznávacího znaku*/
    writeCharacter(FIRST_AND_LAST_CHAR,image,NORMAL_BYTE_LENGTH);

    writeCharacter(crcToBinaryChar(calculate_crc32(binaryMessage)),image,CRC_LENGTH); /*Zapíše crc*/

    writeCharacter(crcToBinaryChar(fileSize*8),image,CRC_LENGTH);

    fseek(binaryMessage,0, SEEK_SET); /*Najde začátek zprávy*/

    /*Zapisování zakódované zprávy do obrázku*/
    while ((byte = fgetc(binaryMessage)) != EOF){
        binaryChar = codeToBinary(byte);
        writeCharacter(binaryChar,image,NORMAL_BYTE_LENGTH);
    }
    return 1;
}

int getMessageFromBMP(FILE* image, FILE* output){
    struct BMPHeader header;
    char* character;
    FILE* binaryFileWithMessage;
    u_int32_t crc;
    long fileSize;
    
    binaryFileWithMessage = fopen("files/hiddenMessage","w+b");/*Do tohoto souboru se zapisujou jednotlivé byty získané z obrázku*/

    if (!image || !output || !binaryFileWithMessage) {
        printf("Soubory nelze otevřít\n");
        fclose(binaryFileWithMessage);
        return 0;
    }

    /*Kontrola jestli je obrázek typu bmp*/
    if (!loadBMP(image,&header)){ 
        fclose(binaryFileWithMessage);
        return 2;
    }


    fseek(image, header.dataOffset, SEEK_SET);

    /*Kontrola jestli je v obrázku něco schované*/
    if (strcmp(character = readCharacter(image,NORMAL_BYTE_LENGTH),FIRST_AND_LAST_CHAR)){
        free(character);
        fclose(binaryFileWithMessage);
        return 4; 
    }

    free(character);

    character = readCharacter(image,CRC_LENGTH); /*Přečte crc ze souboru*/
    crc = binaryToCode(character); /*Převede crc na číslo*/
    free(character);

    character = readCharacter(image,CRC_LENGTH); /*Přečte velikost souboru*/
    fileSize = binaryToCode(character); /*Převede velikost souboru na číslo*/
    free(character);

    /*Dokud je velikost souboru větší než 0*/
    while (fileSize>0){
        fileSize -=NORMAL_BYTE_LENGTH;
        character = readCharacter(image,NORMAL_BYTE_LENGTH);
        fputc(binaryToCode(character),binaryFileWithMessage);
        free(character);
    }

    fclose(binaryFileWithMessage);


    binaryFileWithMessage = fopen("files/hiddenMessage","rb");
    fseek(binaryFileWithMessage, 0, SEEK_SET);

    if (crc!=calculate_crc32(binaryFileWithMessage)){/*Krontrola crc32*/
        printf("ukrytý obsah byl poškozen, neodpovídá kontrolní součet obsahu či nelze obsah korektně dekomprimovat");
        fclose(binaryFileWithMessage);
        return 5; 
    }


    fseek(binaryFileWithMessage, 0, SEEK_SET);
    /*Veme soubour hiddenMessage a provede jeho dekompresi*/
    decompressLZW(binaryFileWithMessage,output);

    fclose(binaryFileWithMessage);

    return 1;
}

void writeCharacter(char* binaryCode,FILE* image,int length){
    int i;
    if (!image){
        return;
    }
    /*Zapíše jednotlivé bity do LSB modrého kanálu*/
    for (i = 0; i < length; ++i) {
        uint8_t pixel[3];
        fread(&pixel, sizeof(uint8_t), 3, image);
        pixel[0] = (pixel[0] & 0xFE)|(binaryCode[i]-'0');

        fseek(image, -3, SEEK_CUR);
        fwrite(&pixel, sizeof(uint8_t), 3, image);
    }
}


char* readCharacter(FILE* image, int length){
    int i;
    char* extractedMessage =(char*)malloc(length+1); 

    if (!image){
        return NULL;
    }
    /*Čte jednotlivé bity z obrázku*/
    for (i = 0; i < length; ++i) {
            uint8_t pixel[3];
            fread(&pixel, sizeof(uint8_t), 3, image);
            extractedMessage[i] = (pixel[0] & 0x01)+'0';
    }

    extractedMessage[length] = '\0';
    return extractedMessage;
} 