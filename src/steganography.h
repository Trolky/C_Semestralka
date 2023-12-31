#ifndef STEGANOGRAPHY_H_
#define STEGANOGRAPHY_H_

#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include "file.h"
#include "pngImage.h"


/**
 * @brief Schová zprávu v LSB modrého kanálu u obrázku
 * 
 * @param image obrázek, do kterého se má ukrýt zpráva
 * @param binaryMessage Zpráva, která se má ukrýt
 * @param extension přípona obrázku
 * @return int vrátí 1 jestli se to podařilo, jinak 0
 */
int hideMessageInLSBBlue(FILE* image, FILE* binaryMessage, char* extension);

/**
 * @brief Vyjme zprávu z LSB modrého kanálu u obrázku
 * 
 * @param image Obrázek, ze kterého má číst
 * @param output Kam má zapisovat výsledek
 * @param extension přípona obrázku
 * @return int vrátí 1 jestli se to podařilo, jinak 0
 */
int getHiddenMessageFromLSBBlue(FILE* image, FILE* output, char* extension);



#endif