#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"
#include "file.h"
#include "lzw.h"

/*funkce pro LZW kompresi*/
int compressLZW(FILE *inputFile, FILE *outputFile) {   
    int prefix;
    int character;
    int nextCode = 256;
    int index;
    
    if (!inputFile||!outputFile){
        return 0;
    }

    prefix = getc(inputFile);
    if (prefix == EOF) {
        return 0;
    }
    
    dictionaryInit();/*Inicalizace slovníku*/
    
    /*Dokuď je co číst smyčka probíhá*/
    while ((character = getc(inputFile)) != EOF) {
        
        /*Kontrola jestli je to ve slovníku, pokud ano prefix se přehodí na index. Pokud ne, přidá prefix se znakem do slovníku a zapíše prefix do souboru*/
        if ((index = dictionaryLookup(prefix, character)) != -1) prefix = index;
        else {
            writeBinary(outputFile, prefix);
            if (nextCode < DICETRORY_MAX_SIZE) dictionaryAdd(prefix, character, nextCode++);
            prefix = character;
        }
    }

    /*Zapíše poslední kod*/
    writeBinary(outputFile, prefix); 
    

    if (leftover > 0) fputc(leftoverBits << 4, outputFile);
    
    /*Zničí slovník a uvolní paměť*/
    dictionaryDestroy();
    return 1;
}

/*Funkce pro LZW dekomrpsi*/
int decompressLZW(FILE* inputFile, FILE* outputFile) {
    int currentCode , firstChar;
    int nextCode = 256;
    int previousCode;

    if (!inputFile ||!outputFile){
        return 0;
    }

    previousCode = readBinary(inputFile);

    if (previousCode == 0) {
        return 0;
    }
    fputc(previousCode, outputFile); /*Zapíše první znak do souboru*/
     /*Dokuď je co číst smyčka probíhá*/
    while ((currentCode = readBinary(inputFile)) > 0) {
        if (currentCode >= nextCode) {
            fputc(firstChar = decode(previousCode, outputFile), outputFile); 
        } else firstChar = decode(currentCode, outputFile); 
        
        if (nextCode < DICETRORY_MAX_SIZE) dictionaryAdd(previousCode, firstChar, nextCode++);
        
        previousCode = currentCode;
    }

    dictionaryDestroy();
    return 1;
}

/*Funkce pro dekodování kodu*/
int decode(int code, FILE * outputFile) {
    int character; int temp;

    if (code > 255) { 
        character = dictionaryCharacter(code);
        temp = decode(dictionaryPrefix(code), outputFile); 
    } else {
        character = code;
        temp = code;
    }
    fputc(character, outputFile);
    return temp;
}
