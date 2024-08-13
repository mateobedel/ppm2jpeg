#ifndef _ENCODE_H_
#define _ENCODE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "qtables.h"
#include "htables.h"
#include "buffer.h"
#include "mcu.h"
#include "dct.h"



/**
 * @brief Structure représentant les informations sur la magnitude d'un nombre.
 * 
 * @param class Classe de magnitude du nombre
 * @param index Indice du nombre dans cette classe de magnitude
 * 
 * class et index sont calculés par la fonction get_magnitude.
*/
typedef struct Magnitude {
    uint8_t class;
    uint16_t index;
} Magnitude;


/**
 * @brief Encodes les MCUs spatiale et écrit l'encodage dans le fichier jpeg de sortie
 * 
 * @param outfile Pointeur sur le fichier jpeg de sortie
 * @param spat_mcus Tableau des MCUs spatiales représentant l'image
 * @param header Header du fichier ppm
 * @param sample Tableau des paramètres d'échantillonnage de chaque composante
*/
void encode_mcus(FILE* outfile, Spat_MCU* spat_mcus, Ppm_header header, Sample_val* sample);




/**
 * @brief Encode une composante d'un block spatial et ecrit l'encodage dans le fichier jpeg de sortie
 * 
 * @param spat_commp Composante spatiale à encoder
 * @param ind_comp Indice de la composante
 * @param prec_DC Tableau des valeurs de DC précédente indexé par composante
 * @param buffer Buffer où écrire les valeurs
 * @param file Pointeur vers fichier de sortie
*/
void encode_component(Spat_component* spat_comp, uint8_t ind_comp, int16_t* prec_DC, Buffer* buffer, FILE* file);



/**
 * @brief Transformation zig-zag de la composante et quantification de ses coefficients.
 * 
 * @param component Composante à réorganiser et quantifier
 * @param is_Y Nature de la composante. Permets de décider quelle table de quantification utiliser.
 * 
 * @return Tableau simple des 64 coefficients de la composante initiale réorganisés par zig-zag et quantifiés. 
*/
int16_t* zig_zag_et_quantification (Freq_component* component, bool is_Y);



/**
 * @brief Calcule la classe de magnitude à laquelle un nombre donné appartient ainsi que son index dans cette classe.
 * 
 * @param number Nombre dont on souhaite récupérer ses informations de magnitude.
 * 
 * @return Structure contenant les informations de magnitude du nombre.
*/
Magnitude get_magnitude (int16_t number);



/**
 * @brief Fonction d'encodage complet d'un coefficients DC : DPCM, encodage huffman de la classe de magnitude, mise en buffer.
 * 
 * @param prec_DC Valeur du coefficient DC du bloc précédent afin de calculer et encoder la différence avec le coefficient actuel.
 * @param DC Coefficient DC que l'on souhaite encoder.
 * @param is_Y Nature de la composante à laquelle appartient DC. Permets de décider quelle table de huffman utiliser.
 * @param buffer buffer des données binaires de la sortie
 * @param file fichier de sortie
 * 
 * @return Coefficient DC actuel pour qu'il puisse être récupéré pour l'encodage du prochain coefficient DC.
*/
int16_t encode_DC (int16_t prec_DC, int16_t DC, bool is_Y, Buffer* buffer, FILE* file);



/**
 * @brief Fonction d'encodage complet des coefficients AC d'un bloc : RLE, huffmaan, mise en buffer.
 * 
 * @param array Tableau des coefficients du bloc à encoder (organisé en zig-zag et quantifié). Le premier coefficient est ignoré (coef DC).
 * @param is_Y Nature de la composante à laquelle appartient les coefficients AC. Permets de décider quelle table de huffman utiliser.
 * @param buffer buffer des données binaires de la sortie
 * @param file fichier de sortie
*/
void encode_AC (int16_t* array, bool is_Y, Buffer* buffer, FILE* file);



#endif