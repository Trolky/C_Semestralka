#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#define EMPTY_PREFIX -1

/** @brief Struktura popisující slovník */
struct DictNode {
    int value; /* Pozice ve slovníku*/
    int prefix; /* Prefix */
    int character; /*Písmeno*/
    struct DictNode *next; /*Ukazatel na další node*/
};

/**
 * @brief Inicializace slovníku a naplnění ho základními ASCII znaky
 */
void dictionaryInit();

/**
 * @brief Přidání dalšího nodu do slovníku
 * @param node Node, který se má přidat
 */
void appendNode(struct DictNode *node);

/** @brief Uvolní pamět a zničí slovník */
void dictionaryDestroy();

/**
 * @brief Koukne jestli je prefix se znakem ve slovníku
 * @param prefix vyhledávaný prefix
 * @param character vyhledávaný znak
 * @return int Node hodnotu nebo -1(Pokud tam není)
 */
int dictionaryLookup(int prefix, int character);

/**
 * @brief Vyhledá prefix ve slovníku
 * @param value Node hodnota
 * @return int Hodnota prefixu nebo -1(Pokud tam není)
 */
int dictionaryPrefix(int value);


/**
 * @brief Vyhledá znak ve slovníku
 * @param value Node value
 * @return int Znak nebo -1(Pokud tam není)
 */
int dictionaryCharacter(int value);

/**
 * @brief Přidá prefix, znak a hodnotu do slovníku
 * 
 * @param prefix prefix
 * @param character znak
 * @param value hodnota
 */
void dictionaryAdd(int prefix, int character, int value);
#endif