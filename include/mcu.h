#ifndef _MCU_H_
#define _MCU_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "read_ppm.h"
#include "handle_args.h"

//Hauteur ou largeur d'un block
#define BLOCK_LEN 8

//Indice des valeurs de sous-echantillonages
#define IND_Y 0
#define IND_CB 1
#define IND_CR 2


typedef struct Ppm_header Ppm_header;
typedef struct Sample_val Sample_val;


/**
 * @brief Structure d'un couple unsigned de 16bits
 */
typedef struct Couple16 {
    uint16_t v;
    uint16_t h;
} Couple16;


/**
 * @brief Structure d'une composante d'un bloc dans le domaine spatial
 *
 * @param pixel_value Matrice des valeurs des pixels d'une composante echantilloné (Y, Cb ou Cr)
 */
typedef struct Spat_component {
    uint8_t** pixel_value;
} Spat_component;


/**
 * @brief Structure d'une composante d'un bloc dans le domaine fréquentiel
 *
 * @param pixel_value Matrice des valeurs des pixels d'une composante echantilloné (Y, Cb ou Cr)
 */
typedef struct Freq_component {
    int16_t** pixel_value;
} Freq_component;


/**
 * @brief Structure d'un bloc d'une MCU dans le domaine spatial
 *
 * @param Y pointe vers un bloc échantillonné pour la luminance
 * @param Cb pointe vers un bloc échantillonné pour la différence de chrominance bleue
 * @param Cr pointe vers un bloc échantillonné pour la différence de chrominance rouge
 */
typedef struct Spat_block {
    Spat_component* Y;    
    Spat_component* Cb;
    Spat_component* Cr;
} Spat_block;


/**
 * @brief Structure d'une MCU
 *
 * @param blocks Matrice des blocs d'une MCU dans le domaine spatial
 */
typedef struct Spat_MCU {
    Spat_block** blocks;
} Spat_MCU;


/**
 * @brief Return le plafond de la division entre x et y
 */
uint16_t ceil_div(uint16_t x, uint16_t y);


/**
 * @brief Alloue une composante d'un block d'une MCU
 * 
 * @return l'allocation d'une composante
 */
Spat_component* malloc_component();


/**
 * @brief Alloue l'espace nécessaire pour stocker les MCUs représentant l'image (pour chaque MCU, allouer
 * la structure des composantes, pour chaque composante, allouer le tableau double contenant les valeurs 
 * des pixels)
 * 
 * @param header Informations de header du fichier d'entrée. Utile au calcul du nombre de MCU et pour avoir
 * accès au type de l'image.
 * @param sample Paramètres d'échantillonnage des composantes.
*/
Spat_MCU* malloc_mcus(Ppm_header header, Sample_val* sample);


/**
 * @brief Calcule la moyenne des pixels d'un groupe de pixels. Utile au sous-échantillonnage.
 * 
 * @param C composante spatiale dans laquelle se trouve le groupe dont on calcule la moyenne.
 * @param gp Informations sur le groupe (taille) de pixels dont on calcule la moyenne.
 * @param p_gp position du groupe dans le bloc.
 * 
 * @return Moyenne du groupe de pixels.
*/
uint8_t calc_moy_group(Spat_component* C, Couple16 gp, Couple16 p_gp);


/**
 * @brief Sous-échantillonne une composantes
 * 
 * @param ech_component Composante du sous-échantillonnage
 * @param component Pointeur vers la composante à échantillonner
 * @param gp Taille du groupe de pixel
 * @param i Indice vertical du bloc
 * @param j Indice horizontal du bloc
 * @param sample Informations de sous-échantillonage
*/
void downsample_component(Spat_component* ech_component, Spat_component** component, Couple16 gp, int8_t i, int8_t j, Sample_val sample);


/**
 * @brief Sous-échantillone les MCUs de l'image
 * 
 * @param mcus Liste des mcus de l'image
 * @param header Informations de l'image à encoder (taille, type)
 * @param sample Informations de sous-échantillonage
*/
void downsample(Spat_MCU* mcus, Ppm_header header, Sample_val* sample);



/**
 * @brief Libère une composante spatiale d'un block
 *
 * @param component la composante à free
 */
void free_spat_component(Spat_component* component);


/**
 * @brief Libère une composante frequentielle d'un block
 *
 * @param component la composante à free
 */
void free_freq_component(Freq_component* component);


/**
 * @brief Libère une liste de MCU spatiale sous-echantillonée
 *
 * @param mcus liste de MCU spatiale sous-echantillonée
 * @param header le header du fichier ppm
 */
void free_spat_mcus(Spat_MCU* mcus, Ppm_header header, Sample_val* sample);


/**
 * @brief Affiche une mcu spatiale
 */
void print_spat_MCU(Spat_MCU mcu, Sample_val* sample, bool is_downsampled, Ppm_header header);


#endif