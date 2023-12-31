#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"



struct DictNode *dictionary, *tail;

void dictionaryInit() {
    int i;
    struct DictNode *node;
    for (i = 0; i < 256; i++) { /*Inicializace spojového slovníku. 256 protože tolik je znaků v ascii tabulce*/
        node = (struct DictNode *)malloc(sizeof(struct DictNode));
        node->prefix = EMPTY_PREFIX;
        node->character = i;
        appendNode(node);
    }       
}

void appendNode(struct DictNode *node) { /*Přidání uzel*/
    if (dictionary != NULL) tail->next = node;
    else dictionary = node;
    tail = node;
    node->next = NULL;
}

void dictionaryDestroy(){ /*Zničí slovník*/
    while (dictionary != NULL) {
        struct DictNode *temp = dictionary;
        dictionary = dictionary->next; 
        free(temp);
   }
}

int dictionaryLookup(int prefix, int character) {
    struct DictNode *node;
    for (node = dictionary; node != NULL; node = node->next) { /*Ve svlovníku vyhledá hodnotu*/
        if (node->prefix == prefix && node->character == character) return node->value;
    }
    return EMPTY_PREFIX;
}

int dictionaryPrefix(int value) {
    struct DictNode *node; /*Ve slovníku vyhledá prefix*/
    for (node = dictionary; node != NULL; node = node->next) {
        if (node->value == value) return node->prefix;
    }
    return -1;
}

int dictionaryCharacter(int value) {
    struct DictNode *node; /*Ve svlovníku vyhledá znak*/
    for (node = dictionary; node != NULL; node = node->next) {
        if (node->value == value) {
            return node->character;
        }
    }
    return -1;
}

void dictionaryAdd(int prefix, int character, int value) {
    struct DictNode *node; /*Vytvoří nový uzel*/
    node = (struct DictNode *)malloc(sizeof(struct DictNode));
    node->value = value;
    node->prefix = prefix;
    node->character = character;
    appendNode(node); /*Přidá uzel*/
}
