#ifndef PNG_IMAGE_H_
#define PNG_IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "lzw.h"
#include <string.h>
#include <math.h>
#include <zlib.h>
#include <png.h>

#define PNG_SIGNATURE_SIZE 8
#define FIRST_AND_LAST_CHAR "10101010"
#define FIRST_AND_LAST_CHAR_LENGTH 8
#define NORMAL_BINARY_CHAR_LENGTH 8
/**
 * @brief Schová zprávu v LSB modrého kanálu u obrázku PNG
 * 
 * @param image obrázek, do kterého se má načíst
 */
int loadPNG(FILE* image);

/**
 * @brief Zapíše zprávu do PNG obrázku
 * 
 * @param image obrázek do, kterého se má zapisovat
 * @return int 1 jestli se to povedlo, jinak 0
 */
int writePNG(FILE* image);

/**
 * @brief Schová zprávu v PNG obrázku
 * 
 * @param image obrázek do, kterého se má zpráva schovat
 * @param binaryMessage zpráva, která se má schovat
 * @return int 1 jestli se to povedlo, jinak 0
 */
int hideMessageInPNG(FILE* image, FILE* binaryMessage);

/**
 * @brief Zmení modré LSB bity u PNG obrázku
 * 
 * @param binaryMessage zpráva, která se tam má schovat
 * @return int 1 jestli se to povedlo, jinak 0
 */
int manipulateLSB(char* binaryMessage);

/**
 * @brief Převede binární soubor na řetězec
 * 
 * @param binaryMessage binární soubor
 * @return char* řetězec se zprávou
 */
char* binaryMessageToChar(FILE* binaryMessage);

/**
 * @brief Přečte zprávu z PNG obrázku
 * 
 * @param image obrázek, ze kterého se má číst
 * @param output soubor, kam má danou zprávu zapsat
 * @return int 1 jestli se to povedlo, jinak 0
 */
int readMessageFromPNG(FILE* image, FILE* output);

/**
 * @brief Získá zprávu s modrého LSB a zapíše ji do binárího souboru
 * 
 * @param binaryMessage Soubor kam má bity zapisovat
 * @return int 1 jestli se to povedlo, jinak 0
 */
int getManipulatedLSB(FILE* binaryFileWithMessage);

/**
 * @brief Přidá znak do řetězce
 * 
 * @param length kolik znaků se má přidat
 * @param fullMessage kam se to má přidat
 * @param binaryChar co se má přidat
 */
void addCharacterToMessage(int length, char* fullMessage, char* binaryChar);
#endif