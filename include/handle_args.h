#ifndef _HANDLE_ARGS_H_
#define _HANDLE_ARGS_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "mcu.h"


/**
 * @brief Chaines de caractères de formatage du texte dans un terminal (en ANSI Escape Sequences)
 */
#define fBOLD "\x1b[1m"
#define fTITLE "\e[1m\x1B[32m"
#define fWARN "\x1b[31m"
#define fRESET "\x1B[0m"


/**
 * @brief Structure d'une dimension des valeurs d'un sous-echantillonage
 *
 * @param h Correspond au sous-echantillonage horizontal
 * @param v Correspond au sous-echantillonage vertical
 */
typedef struct Sample_val {
    uint8_t h;
    uint8_t v;
} Sample_val;


/**
 * @brief Alloue et renvoi une chaine de caractère commençant par le caractère pointé par s
 *
 * @param s adresse d'un caractère d'une chaine de caractère
 * @return la chaine de caractère allouée à partir de s
 */
char *str_save(char* s);


/**
 * @brief Affiche dans le terminal le mode d'emploi d'utilisation du programme
 * 
 */
void print_help();


/**
 * @brief Verifie que les valeurs de sous echantillonage sont conforme à la norme JPEG.
 * On exclu aussi les cas ou les valeurs de sous echantillonage ne divise pas la taille du block
 * @param sample Les valeurs de sous echantillonages
 * 
 */
bool verify_sample(Sample_val sample[3]);


/**
 * @brief Libère sample et associe ses valeurs à la liste de valeur du sous-echantillonage ech
 *
 * @param str_sample chaine de caractère à transormé en valeur de sous-echantillonage
 * @param sample Les valeurs de sous echantillonages
 * @return true si la transformation s'est bien passée, false sinon
 */
bool split_sample(char* str_sample, Sample_val sample[3]);


/**
 * @brief Lit les arguments tapés à l'éxécution de la fonction et modifie les valeurs de file_path,
 * outfile et sample en fonction.
 *
 * @param argc Le nombre d'arguments à l'éxecution
 * @param argv Les arguments
 * @param file_path Pointeur vers le chemin d'entrée
 * @param outfile Pointeur vers le chemin de sortie
 * @param sample Liste des valeurs du sous-echantillonage
 * @return true si la lecture s'est bien passée, false sinon
 */
bool read_args(int argc, char* argv[], char** file_path, char** outfile, Sample_val sample[3]);


/**
 * @brief Libère file_path et outfile
 *
 * @param file_path Pointeur vers le chemin d'entrée
 * @param outfile Pointeur vers le chemin de sortie
 */
void free_args(char** file_path, char** outfile);

#endif