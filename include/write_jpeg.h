#ifndef _WRITE_OUTPUT_FILE_H_
#define _WRITE_OUTPUT_FILE_H_

#include <stdint.h>
#include <stdio.h>

#include "handle_args.h"
#include "read_ppm.h"
#include "qtables.h"
#include "htables.h"



/**
 * @brief Ouvre le fichier passé en paramètre
 * 
 * @param default_file_path Chemin par défaut pour la création du fichier de sortie JPEG, 
 * (le même que celui de l'image en entrée)
 * @param given_file_path Si l'option --outfile à été précisée lors de l'éxecution, alors
 * le chemin indiqué par cette varaible est prioritaire sur le chemin par défaut et c'est 
 * à cet endroit que sera créé la fichier de sortie.
 * Vaut NULL si --outfile n'a pas été précisé.
 * 
 * @return pointeur vers le fichier de sortie
*/
FILE* open_file (char* default_file_path, char* given_file_path);


/**
 * @brief converti le .ppm de l'image à encoder en .jpg
 * Utile seulement lorsqu'on créé le fichier de sorite à l'emplacement par défaut.
 * 
 * @param ppm_path chaine de caractère contenant le chemin de fichier d'entrée.
*/
void convert_ppm_path_to_jpeg (char* ppm_path);


/**
 * @brief Ecrit le header JPEG dans le fichiers de sortie, soit les section SOI, APP0, DQT,
 * SOF0, DHT et SOS sans les données brutes de l'image encodé. Les sections DQT, SOF0, DHT et SOS
 * sont délégués aux fonctions header_sec_part_gray ou header_sec_part_col selon le type de l'image.
 * 
 * @param file Pointeur vers le fichier de sortie
 * @param header Données de header du fichier d'entrée (pour récupérer la taille de l'image et son type)
 * @param sample_val Tableau des paramètres d'échantillonnage des composantes
*/
extern void write_JPEG_header (FILE* file, Ppm_header header, Sample_val* sample_val);


/**
 * @brief Construis la seconde partie du header pour les images en niveau de gris (à partir de la section DQT) 
 * car cette partie diffère selon le type de l'image à encoder (niveau de gris ou couleur).
 * 
 * @param header Données de header du fichier d'entrée (pour récupérer la taille de l'image et son type)
 * @param sample_val Tableau des paramètres d'échantillonnage des composantes
 * 
 * @return tableau à passer à la fonction fwrite pour l'écrire dans le fichier de sortie.
*/
uint8_t* header_sec_part_gray (Ppm_header header, Sample_val* sample_val);


/**
 * @brief Construis la seconde partie du header pour les images en couleur (à partir de la section DQT) 
 * 
 * @param header Données de header du fichier d'entrée (pour récupérer la taille de l'image et son type)
 * @param sample_val Tableau des paramètres d'échantillonnage des composantes
 * 
 * @return tableau à passer à la fonction fwrite pour l'écrire dans le fichier de sortie.
*/
uint8_t* header_sec_part_col (Ppm_header header, Sample_val* sample_val);


/**
 * @brief Fonction utilisée lors de l'écriture des données brutes de l'image dans le fichier de sortie.
 * 
 * @param to_write buffer à écrire dans le fichier de sortie. La fonction utilise la variable static file_ptr
 * comme pointeur vers le fichier dans lequel écrire.
*/
extern void write_file (uint16_t* to_write, FILE* file);


/**
 * @brief Inverse les deux octets du paramètre. Utile lors de l'écriture des données brutes de l'image dans
 * le fichier de sortie.
*/
uint16_t reverse_bytes(uint16_t num);


/**
 * @brief Ecrit dans le fichier de sortie passé en paramètre le marqueur de fin d'image
*/
extern void write_JPEG_eoi (FILE* file_ptr);


/**
 * @brief Ferme le fichier passé en paramètre.
*/
extern void close_file (FILE* file_ptr);


#endif
