#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "code.h"
#include "htables.h"


//Profondeur maximal de l'arbre de huffman
#define NB_LEVEL 16


/**
 * @brief Structure d'un noeud de l'arbre de huffman
 *
 * @param symbol représente le symbole qui sera encodé
 * @param left et @param right sont les sous arbres de Node
 */
typedef struct Node {
    uint8_t symbol;
    struct Node* left, *right;
} Node;


/**
 * @brief Alloue et renvoi un noeud
 *
 * @param symbol le symbol donné à l'initialisation du noeud
 * @return un noeud avec le symbole symbol et des fils NULL 
 */
Node* init_node(uint8_t symbol);


/**
 * @brief Construit l'arbre de huffman des symboles symbols
 *
 * @param symbols les symboles donnés pour construire l'arbre de huffman
 * @param nb_symbols_length les nombres de symboles pour une profondeur données
 * @param nb_symbol le nombre de symboles
 * 
 * @return l'arbre de huffman
 */
Node* build_huffman(uint8_t symbols[], uint8_t nb_symbols_length[], uint8_t nb_symbol);


/**
 * @brief Libère l'arbre de huffman
 *
 * @param root la racine de l'arbre de huffman
 */
void free_huffman(Node* root);


/**
 * @brief Affiche un arbre
 *
 * @param root la racine de l'arbre
 * @param level la profondeur de root
 */
void print_tree(Node* root, int level);


/**
 * @brief Parcours en profondeur l'arbre de huffman, et initialise le tableau des codes de huffman en fonction
 *
 * @param root la racine de l'arbre
 * @param table le tableau des codes de huffman, l'indice correspond au symbole associé au code
 * @param code le code actuel de root
 * @param prof la profondeur de root
 */
void traverse_huffman(Node* root, Code *table, uint16_t code, uint8_t prof);


/**
 * @brief Renvoi la table des codes de l'arbre de huffman
 *
 * @param root la racine de l'arbre de huffman
 * @param len la longueur du tableau à initialiser
 * 
 * @return la table des codes de l'arbre de huffman
 */
Code* huffman_table(Node* root, uint16_t len);


/**
 * @brief Affiche la structure associé au tableau des codes de huffman dans la sortie standard
 *
 * @param table le tableau des codes de huffman
 * @param len la longueur du tableau
 */
void print_huffman_table(Code* table, uint16_t len);

#endif