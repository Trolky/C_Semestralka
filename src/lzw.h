#ifndef LZW_H_
#define LZW_H_

#include <stdio.h>

#define DICETRORY_MAX_SIZE 4095 /*Maximum záznamů ve slovníku*/
#define CODE_LENGTH 12 /*Délka kodu*/


/**
 * @brief Udělá LZW kompresi na zadaný soubor. A výsledek vypíše do binárního souboru encodedPayload.lzw
 * 
 * @param inputFile Soubor ze kterého má číst 
 * @param outputFile Soubor do kterého má zapisovat
 * @return int 1 pokud se to povedlo, jinak 0
 */
int compressLZW(FILE *inputFile, FILE *outputFile);


/**
 * @brief Udělá LZW dekompresi ze souboru encodedPayload.lzw
 * 
 * @param inputFile Soubor se kterého má číst
 * @param outputFile Soubor do kterého má zapisovat
 * @return int 1 pokud se to povedlo, jinak 0
 */
int decompressLZW(FILE * inputFile, FILE * outputFile);


/**
 * @brief Dékoúduje kod a zapíše ho do souboru
 * 
 * @param code Kod
 * @param outputFile Soubor do kterého má zapisovat
 * @return int ASCII znak v int
 */
int decode(int code, FILE * outputFile);
#endif