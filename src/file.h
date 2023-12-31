#ifndef FILE_H_
#define FILE_H_

#include <stdio.h>
#include <zlib.h>
extern int leftover;
extern int leftoverBits;

#define BUFFER_SIZE 4096
#define CRC_LENGTH 32

/**
 * @brief Binárně zapisuje do souboru
 * 
 * @param output Soubor do kterého má zapisovat
 * @param code Kod, který má zapsat
 */
void writeBinary(FILE* output, int code);

/**
 * @brief Čte z binárního souboru
 * 
 * @param input Soubor ze kterého má číst
 * @return int Kod, který přečetl
 */
int readBinary(FILE* input);

/**
 * @brief Převede kod do binárního stringu
 * 
 * @param code kod, který má převést
 * @return char* string kodu v binární podobě
 */
char* codeToBinary(int code);


/**
 * @brief Převede string binárního kodu na kod 
 * 
 * @param binaryChar string binárního kodu
 * @return int kod
 */
int binaryToCode(char* binaryChar);

/**
 * @brief Tranformuje kod, prootže kod je 12 bitový
 * 
 * @param code Kod, který má transformovat
 * @param input V případě kdy není přebytek načte z tohoto souboru další kod
 * @return int transformovaný kod
 */
int transformCode(int code, FILE* input);

/**
 * @brief vypočte crc souboru
 * 
 * @param binaryMessage soubor u kteréhé se má spočítat crc
 * @return u_int32_t hodnota
 */
u_int32_t calculate_crc32(FILE* binaryMessage);

/**
 * @brief převede crc na binární řetězec
 * 
 * @param crc crc, který se má převést
 * @return char* binární řetězec
 */
char* crcToBinaryChar(u_int32_t crc);
#endif