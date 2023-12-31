#ifndef BMP_H_
#define BMP_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include <string.h>
#include "lzw.h"

#define BMP_HEADER_SIGNATURE 0x4D42
#define NORMAL_BYTE_LENGTH 8
#define FIRST_AND_LAST_CHAR "10101010" /*První a poslední znak, který se zapisuje do obrázku, aby se poznalo, jestli je v obrázku něco ukryté*/

/**
 * @brief Struktura BMP obrázku
 */
#pragma pack(push, 1)
struct BMPHeader{
    uint16_t signature; 
    uint32_t fileSize;      
    uint32_t reserved;      
    uint32_t dataOffset; 
    uint32_t headerSize;  
    int32_t  width;         
    int32_t  height;
    uint16_t planes;        
    uint16_t bitsPerPixel;  
};
#pragma pack(pop)
/**
 * @brief Načte obrázek ze souboru
 * 
 * @param image Obrázek o které získá údaje
 * @param header Struktura, do které má hlavičku obrázku
 * @return int vrátí 1 jestli se to podařilo, jinak 0
 */
int loadBMP(FILE *image ,struct BMPHeader *header);

/**
 * @brief Získá počet pixelů v obrázku
 * 
 * @param imageHeader Strukturu BMPHeader
 * @return int počet pixelu
 */
int getNumberOfPixels(struct BMPHeader imageHeader);

/**
 * @brief Schová zprávu v LSB modrého kanálu u obrázku BMP
 * 
 * @param image obrázek, do kterého se má ukrýt zpráva
 * @param binaryMessage Zpráva, která se má ukrýt
 * @return int vrátí 1 jestli se to podařilo, jinak 0
 */
int hideMessageInBMP(FILE* image, FILE* binaryMessage);

/**
 * @brief Vyjme zprávu z LSB modrého kanálu u obrázku BMP
 * 
 * @param image Obrázek, ze kterého má číst
 * @param output Kam má zapisovat výsledek
 * @return int vrátí 1 jestli se to podařilo, jinak 0
 */
int getMessageFromBMP(FILE* image, FILE* output);

/**
 * @brief Zapíše do obrázku binární podobu kodu
 * 
 * @param binaryCode binární kod
 * @param image obrázek, kam má zapisovat
 * @param length kolik bitu se má zapsat
 */
void writeCharacter(char* binaryCode,FILE* image, int length);

/**
 * @brief Přečte znak z obrázku
 * 
 * @param image Obrázek, že kterého má číst
 * @param length kolik bitu se má přečíst
 * @return char* String binárního kodu
 */
char* readCharacter(FILE* image, int length);
#endif