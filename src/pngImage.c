#include "pngImage.h"

png_bytepp rowPointers;
int width, height;
png_structp readPngPtr;
png_infop readInfoPtr;
int counter = 0;
u_int32_t crcFromImage;

int hideMessageInPNG(FILE* image,FILE* binaryMessage){
    char* fullMessage;
    int errorCode;
    if (!image || !binaryMessage){ /*Kontrola jestli existuje obrázek a zpráva */
        return 0;
    }

    

    errorCode = loadPNG(image); /*Načte PNG obrázek*/
    if (errorCode == 2) return 2;
    else if (errorCode == 0) return 0;
    
    fseek(binaryMessage,0,SEEK_SET);
    fullMessage = binaryMessageToChar(binaryMessage); /*Načte celou zprávu z binary souboru*/
    if (fullMessage == NULL){
        return 3;
    }
    
    manipulateLSB(fullMessage); /*Uloží zprávu do modrých LSB bitů*/

    rewind(image); /*Vrátí se zpátky na začátek obázku*/

    if (!writePNG(image)){/*Přepíše obrázek*/
        png_destroy_read_struct(&readPngPtr, &readInfoPtr, NULL); 
        return 0;
    }

    png_destroy_read_struct(&readPngPtr, &readInfoPtr, NULL); /*Zničí čtecí strukturu*/

    return 1;

}

int readMessageFromPNG(FILE* image, FILE* output){
    FILE* binaryFileWithMessage;
    
    binaryFileWithMessage = fopen("files/hiddenMessage","w+b");
    if (!image){
        goto error;
    }

    if (loadPNG(image) == 2){ /*Načte PNG obrázek*/
        fclose(binaryFileWithMessage);
        return 2;
    }

    if (!getManipulatedLSB(binaryFileWithMessage)){ /*Získá zbrávu z modrých LSB bitů*/
        goto error;
    }

    fclose(binaryFileWithMessage);

    png_destroy_read_struct(&readPngPtr, &readInfoPtr, NULL); /*Zničí čtecí strukturu*/

    binaryFileWithMessage = fopen("files/hiddenMessage","rb");
    fseek(binaryFileWithMessage,0,SEEK_SET);

    if (crcFromImage != calculate_crc32(binaryFileWithMessage)){ /*Kontrola crc32*/
        fclose(binaryFileWithMessage);
        return 5;
    }

    fseek(binaryFileWithMessage, 0, SEEK_SET); /*Vrátí se na začátek souboru*/


    decompressLZW(binaryFileWithMessage,output); /*Dekomprimuje zprávu*/

    fclose(binaryFileWithMessage);

    

    return 1;

    error:
        fclose(binaryFileWithMessage);
        return 4;
}

int loadPNG(FILE* image){
    unsigned char signature[PNG_SIGNATURE_SIZE];
    size_t numBytes;
    if (!image){/*Kontrola jestli jde obrázek otevřít*/
        printf("Obrázek nelze otevřít)\n");
        return 0;
    }

    numBytes = fread(signature, 1, PNG_SIGNATURE_SIZE, image);/*Přeče signaturu obrázku*/
    readPngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);/*Vytvoří čtecí strukturu obrázku*/

    if (!readPngPtr){
        return 0;  
    }

    readInfoPtr = png_create_info_struct(readPngPtr); /*Vytvoří info strukturu obrázku*/
    if (!readInfoPtr){
        png_destroy_read_struct(&readPngPtr, &readInfoPtr, NULL);
        return 0;  
    }

    if (setjmp(png_jmpbuf(readPngPtr))) {
        goto errorMessage;
    }

    if (png_sig_cmp(signature,0,numBytes) != 0){ /*Zkontroluje zda obrázek je PNG*/
       goto errorMessage;
    }


    png_set_sig_bytes(readPngPtr, numBytes);/*Jelikož jsem už pár bitů přečetl z obrázku, musím to dát vědět*/
    png_init_io(readPngPtr, image);

    png_read_png(readPngPtr, readInfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
    
    if (png_get_color_type(readPngPtr,readInfoPtr) != PNG_COLOR_TYPE_RGB){ /*Zkontroluje zda obrázek je RGB*/
        goto errorMessage;
    }
   
    
    width = png_get_image_width(readPngPtr, readInfoPtr); /*Šířka obrázku*/
    height = png_get_image_height(readPngPtr, readInfoPtr); /*Výška obrázku*/
   
    rowPointers = png_get_rows(readPngPtr, readInfoPtr); /*Uloží ukazatele na řádky*/
 
    return 1;

    errorMessage:
        png_destroy_read_struct(&readPngPtr, &readInfoPtr, NULL);
        return 2;
}

int writePNG(FILE* image){
    png_structp pngPtr;
    png_infop infoPtr;
    if (!image){ /*Kontrola jestli jde obrázek otevřít*/
        printf("Soubor nelze otevřít)\n");
        return 0;
    }

    pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); /*Vytvoření zápisové struktury*/
    if (!pngPtr) {
        printf("Chyba při vytváření zápisové struktury.\n");
        return 0;
    }
    infoPtr = png_create_info_struct(pngPtr); /*Vytvoření čtecí struktury*/
    if(!infoPtr){
        png_destroy_write_struct(&pngPtr, &infoPtr);
        printf("Chyba při vytváření info struktury.\n");
        return 0;
    }

    png_init_io(pngPtr, image);
    png_set_rows(pngPtr, infoPtr, rowPointers); /*Nastavrení řádků*/

    png_set_IHDR(pngPtr, infoPtr, width,
                 height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT); /*Nastavení IHDR*/

    png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
  
    png_destroy_write_struct(&pngPtr, &infoPtr); /*Zničení zápisové struktury*/
          
    return 1;
}

int manipulateLSB(char* binaryMessage){
    int x,y;
    size_t messageIndex = 0;
    size_t length;
    if (!binaryMessage){
        return 0;
    }
    length = strlen(binaryMessage); /*Jak dlouho má běžet*/

    /*Změní hodnoty modrého LSB bitu*/
    for (y = 0; y < height; y++) {
        png_bytep row = rowPointers[y];
        for (x = 0; x < width; x++) { 
            png_byte *pixel = &(row[x * 3]);
            if (messageIndex < length) {
                pixel[2] = (pixel[2] & 0xFE) | (binaryMessage[messageIndex] - '0'); /*Změní LSB bit*/
                messageIndex++;
            }
            else goto exitLoop; /*Vyskočení z obvou cyklu*/
        }
    }

    exitLoop:
    free(binaryMessage);
    return 1;
}

int getManipulatedLSB(FILE* binaryFileWithMessage){
    int x,y;
    int charLength = 0;
    long length;
    char* binaryChar;
    char* crcBinaryChar; 
    char* lengthBinaryChar;
    int firstFound = 0,crcFound = 0,legthFound = 0;

    if (!binaryFileWithMessage ||!rowPointers){
        return 0;
    }
    /*Alokování paměti pro normální a speciální znaky*/
    binaryChar = (char*)malloc(NORMAL_BINARY_CHAR_LENGTH+1);
    crcBinaryChar = (char*)malloc(CRC_LENGTH+1);
    lengthBinaryChar = (char*)malloc(CRC_LENGTH+1);

    /*Získání hodnoty modrého LSB bitu*/
    for (y = 0; y < height; y++) {
        png_bytep row = rowPointers[y];
        for (x = 0; x < width; x++) { 
            png_byte *pixel = &(row[x * 3]);
            if ((firstFound == 1 && crcFound == 1 && legthFound == 1)){ /*zapisování zprávy a najití posledního znaku*/
                binaryChar[charLength] = (pixel[2] & 0x01)+'0'; /*Vytahování jedntlivích bitu*/
                charLength++;
                if (charLength == 8) {
                    binaryChar[8] = '\0';
                    length -= NORMAL_BINARY_CHAR_LENGTH;
                        if (length == 0) { /*Najití posledního znaku*/
                            free(binaryChar);
                            return 1;
                        }
                    fputc(binaryToCode(binaryChar), binaryFileWithMessage);
                    free(binaryChar);
                    binaryChar = (char*)malloc(9);
                    charLength = 0;
                }
            }
            else if (x < FIRST_AND_LAST_CHAR_LENGTH && firstFound == 0){ /*Najde první znak, pokud je první znak 10101010, tak pokračuje dál, pokud ne ukončí činost*/
                binaryChar[charLength] = (pixel[2] & 0x01)+'0'; /*Vytahování jedntlivích bitu*/
                charLength++;
                if (charLength == 8) {
                    binaryChar[charLength] = '\0';
                    if (strcmp(binaryChar, FIRST_AND_LAST_CHAR)) {
                        free(binaryChar);
                        return 0;
                    }
                    free(binaryChar);
                    binaryChar = (char*)malloc(9);
                    charLength = 0;
                    firstFound = 1;
                } 
            }

            else { 
                if (crcFound == 0){ /*Najítí crc32 */
                    crcBinaryChar[charLength] = (pixel[2] & 0x01)+'0';
                    charLength++;
                    if (charLength == 32){
                        crcBinaryChar[charLength] = '\0';
                        crcFromImage = binaryToCode(crcBinaryChar);
                        charLength = 0;
                        free(crcBinaryChar);
                        crcFound =1;
                    }
                }
                else{ /*Najití délky souboru, která je zakodovaná v 32 bitech v obrázku*/
                    lengthBinaryChar[charLength] = (pixel[2] & 0x01)+'0';
                    charLength++;
                    if (charLength == 32){
                        lengthBinaryChar[charLength] = '\0';
                        length = binaryToCode(lengthBinaryChar)+NORMAL_BINARY_CHAR_LENGTH; /*Od délky odečte první znak, crc znak a délku samotného znaku*/
                        charLength = 0;
                        free(lengthBinaryChar);
                        legthFound = 1;
                    }
                }
            }
        }
    }
    return 1;
}

char* binaryMessageToChar(FILE* binaryMessage){
    char* fullMessage;
    int byte;
    long fileSize;
    char* binaryChar;
    char* crcBinaryChar;
    char* fileLength;

    if (!binaryMessage){
        return NULL;
    }

    fseek(binaryMessage,0,SEEK_END);
    fileSize = ftell(binaryMessage); /*Zjištění velikosti zprávy*/
    if ((fileSize*8)+(2*FIRST_AND_LAST_CHAR_LENGTH)+CRC_LENGTH>(width*height)){ /*Kontrola jestli se zpráva dá zakodovat do obrázku*/
         return NULL;
    }

    fseek(binaryMessage,0,SEEK_SET);

    fullMessage = (char*)malloc((fileSize*NORMAL_BINARY_CHAR_LENGTH)+(CRC_LENGTH*2)+(FIRST_AND_LAST_CHAR_LENGTH)+1); /*Alokování velikosti zprávy*/
    if (!fullMessage){
        return NULL;
    }

    crcBinaryChar = crcToBinaryChar(calculate_crc32(binaryMessage));
    fileLength = crcToBinaryChar(fileSize*8);
    fseek(binaryMessage,0,SEEK_SET);

    addCharacterToMessage(FIRST_AND_LAST_CHAR_LENGTH,fullMessage,FIRST_AND_LAST_CHAR); /*Přidání prvního znaku*/

    addCharacterToMessage(CRC_LENGTH,fullMessage,crcBinaryChar); /*Přidání CRC*/
    free(crcBinaryChar);

    addCharacterToMessage(CRC_LENGTH,fullMessage,fileLength); /*Přidání délky souboru*/
    free(fileLength);

    while ((byte = fgetc(binaryMessage)) != EOF){ /*Přidání znaků z binárního souboru*/
        binaryChar = codeToBinary(byte);

        addCharacterToMessage(NORMAL_BINARY_CHAR_LENGTH,fullMessage,binaryChar); /*Přidání zprávy*/

        free(binaryChar);
    }

    fullMessage[counter] = '\0';
    return fullMessage;
}

void addCharacterToMessage(int length, char* fullMessage, char* binaryChar){
    int i;
    for (i = 0;i<length;i++){ 
         fullMessage[counter] = binaryChar[i];
         counter++;
    }
}