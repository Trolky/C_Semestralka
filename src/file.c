#include "file.h"
#include "steganography.h"

int leftover = 0;
int leftoverBits;

void writeBinary(FILE * output, int code) {
    if (!output){
        printf("Soubor nelze otevřít");
        return;
    }

    /*Pokud zbyl nějaký zbytek bitů po zakodování*/
    if (leftover > 0) {
        int previousCode = (leftoverBits << 4) + (code >> 8); /*Veme 4 bity v leftOver a vezme první 4 bity z codu*/
        fputc(previousCode, output); 
        fputc(code, output);

        leftover = 0;
    } else {
        leftoverBits = code & 0xF; /*Jelikož délka codu je 12 bitu tak veme poslední 4 bity a uloží je do leftOverBits*/
        leftover = 1;

        fputc(code >> 4, output); /*Do souboru zapíše 8 bitů a leftOver se nastaví na 1*/
    }
}

int readBinary(FILE* input) { /*Čtení z binárního souboru*/
    int code;
    if (!input){
        printf("Soubor nelze otevřít");
        return 0;
    }

    code = fgetc(input);  
    if (code == EOF) return 0;
    return transformCode(code, input);
}


int transformCode(int code, FILE* input){ /*Transformuje code*/
    if (!input){
        printf("Soubor nelze otevřít");
        return 0;
    }

    if (code == EOF) return 0;
    if (leftover > 0) {
        code = (leftoverBits << 8) + code;
        
        leftover = 0;
    } else {
        int nextCode = fgetc(input);
        
        leftoverBits = nextCode & 0xF;
        leftover = 1;
        
        code = (code << 4) + (nextCode >> 4);
    }
    return code;
}


char* codeToBinary(int code) { /*Převede kod do binárního řetězce*/
    int i;
    char* binary = (char*)malloc(9);
    for (i = 7; i >= 0; --i) {
         binary[7 - i] = ((code >> i) & 1) + '0';
    }

    binary[8] = '\0';
    return binary;
}

int binaryToCode(char* binaryChar){ /*Převede binární řetězec na kod*/
    if (!binaryChar){
        return -1;
    }
    return (int)strtol(binaryChar,NULL,2);
}


u_int32_t calculate_crc32(FILE* binaryMessage) { /*Vypočte crc32 souboru*/
    uint8_t buffer[BUFFER_SIZE];
    uint32_t crc32_value;
    size_t bytesRead;
    if (!binaryMessage) {
       return 0;
    }

    crc32_value = crc32(0L, Z_NULL, 0); /*Incializace crc32*/

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), binaryMessage)) > 0) {
        crc32_value = crc32(crc32_value, buffer, bytesRead);
    }

    return crc32_value;
}

char* crcToBinaryChar(u_int32_t crc){ /*Převede crc32 na binární řetězec*/
    int i;
    char* binary;
    if (!crc){
        return NULL;
    }

    binary = (char*)malloc(CRC_LENGTH+1); /*Alokování paměti*/
    for (i = CRC_LENGTH-1; i >= 0; --i) { /*přídání jednotlivých bitu do řetězce*/
         binary[(CRC_LENGTH-1) - i] = ((crc >> i) & 1) + '0';
    }

    binary[CRC_LENGTH] = '\0';
    return binary;
}