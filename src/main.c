#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lzw.h"
#include "steganography.h"

FILE* inputFile;
FILE* outputFile;
FILE* image;

int main(int argc, char** argv) {
    char* extension;
    int errorCode;

     if (argc == 4) {
        if (strcmp(argv[2], "-h") == 0) { /*Pokud je zadáno -h jako druhý argument začne se lzw komprese, a výsledek se zapíše do obrázku*/ 
            inputFile = fopen(argv[3], "r");
            outputFile = fopen("files/encodedPayload.lzw", "w+b"); 
            image = fopen(argv[1], "r+b");

          

            if (!image || !outputFile || !inputFile) {
                return errorMessage("Soubory nelze otevřít",1,1);
            }

            extension = strrchr(argv[1],'.');
            if (!compressLZW(inputFile, outputFile)){ /*Komprese zprávy*/
                 return errorMessage("Komprese se nezdařila",1,1);
            }

            fflush(outputFile); /*Kdyby náhodou něco zbylo v bufferu, tak to dá do output file*/
    
            errorCode = hideMessageInLSBBlue(image,outputFile,extension);
            if (errorCode == 2) return errorMessage("nevhodný formát obrázku u (tj. není to PNG či BMP, 24-bit/pixel,RGB)\n",2,1);
            else if (errorCode == 3) return errorMessage("obrázek není dostatečně velký pro ukrytí zadaného payloadu a potřebných doprovodných informací\n",3,1);

            fclose(inputFile);
            fclose(outputFile); 
            fclose(image);

            return EXIT_SUCCESS;
        } 
        else if(strcmp(argv[2], "-x") == 0) { /*Pokud je zadáno -x jako druhý argument získá se zprává z obrázku a uloží do výstupního souboru*/
            outputFile = fopen(argv[3], "w"); 
            image = fopen(argv[1], "rb");

            extension = strrchr(argv[1],'.');
            if (outputFile == NULL || image == NULL) {
                return errorMessage("Soubory nelze otevřít",1,2);
            }

            errorCode = getHiddenMessageFromLSBBlue(image, outputFile, extension);/*Získá zprávu z obrázku*/
            if (errorCode == 4) return errorMessage("obrázek určený k extrakci payloadu neobsahuje žádný ukrytý obsah\n",4,2);
            else if (errorCode == 5) return errorMessage("ukrytý obsah byl poškozen, neodpovídá kontrolní součet obsahu či nelze obsah korektně dekomprimovat\n",5,2);
            else if (errorCode == 2) return errorMessage("nevhodný formát obrázku u (tj. není to PNG či BMP, 24-bit/pixel,RGB)\n",2,2);
            
            
            fclose(outputFile); 
            fclose(image);

            return EXIT_SUCCESS;
        }
        else{
           return errorMessage("nesprávně zadané či chybějící parametry na příkazové řádce (také neexistující vstupní/výstupní soubory)\n",1,2);
        }
    }
    else{
        return errorMessage("nesprávně zadané či chybějící parametry na příkazové řádce (také neexistující vstupní/výstupní soubory)\n",1,2);
    }

}

int errorMessage(char* message, int returnCode,int whatFilesToClose){
    printf(message);
    if (whatFilesToClose == 1){
        if (inputFile)fclose(inputFile);
        if (outputFile)fclose(outputFile); 
        if (image)fclose(image);
    }
    else{
        if (outputFile)fclose(outputFile); 
        if (image)fclose(image);
    }
    return returnCode;
}



