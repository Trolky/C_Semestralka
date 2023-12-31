#include "steganography.h"


int hideMessageInLSBBlue(FILE* image, FILE* binaryMessage,char* extension) {
    int errorCode;
    if (!binaryMessage || !image){
         return 0;
    }
    if (strcmp(extension,".bmp") == 0){ /*Pokud má obrázek příponu .bmp, předpokládám, že to bude bmp a pracuji se souborek jako s bmp obrázkem*/
        errorCode = hideMessageInBMP(image,binaryMessage);
        switch(errorCode){    
            case 2: return 2;
                   
            case 3: return 3;
                        
            default: return 1;
        }
    }
    else if (strcmp(extension,".png") == 0){/*Pokud má obrázek příponu .png, předpokládám, že to bude bmp a pracuji se souborek jako s png obrázkem*/
        errorCode = hideMessageInPNG(image,binaryMessage);
        switch(errorCode){
            case 2: return 2;
                   
            case 3: return 3;
                        
            default: return 1;
        }
    }
    else{
        return 2;
    }
}



int getHiddenMessageFromLSBBlue(FILE* image, FILE* output,char* extension) {
    int errorCode;
    if (!output ||!image){
        return 0;
    }
    if (strcmp(extension,".bmp") == 0){/*Pokud má obrázek příponu .bmp, předpokládám, že to bude bmp a pracuji se souborek jako s bmp obrázkem*/
        errorCode =getMessageFromBMP(image,output);
        switch(errorCode){
                    
            case 2: return 2;
                   
            case 4: return 4;

            case 5: return 5;  

            default: return 1;
        }
    }
    else if (strcmp(extension,".png") == 0){/*Pokud má obrázek příponu .png, předpokládám, že to bude bmp a pracuji se souborek jako s png obrázkem*/
        errorCode = readMessageFromPNG(image,output);
        switch(errorCode){
            case 2: return 2;
                   
            case 4: return 4;

            case 5: return 5;  

            default: return 1;
        }
    }
    else{
        printf("nevhodný formát obrázku u (tj. není to PNG či BMP, 24-bit/pixel,RGB)\n");
        return 2;
    }
}