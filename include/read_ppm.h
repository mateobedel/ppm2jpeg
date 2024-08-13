#ifndef _READ_PPM_H_
#define _READ_PPM_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "mcu.h" 
#include "handle_args.h"

/**
 * @brief Correspond au format du fichier ppm (ERR si le fichier n'est pas reconnu)
 */
#define ERR 0
#define PGM 5
#define PPM 6

typedef struct Spat_MCU Spat_MCU;
typedef struct Spat_block Spat_block;
typedef struct Sample_val Sample_val;
typedef struct Couple16 Couple16;

/**
 * @brief Structure d'un header d'un fichier ppm lu
 *
 * @param format vaut ERR, PGM ou PPM, correspond au format du fichier ppm
 * @param h Correspond à la taille horizontale en pixels du fichier (largeur)
 * @param v Correspond à la taille verticale en pixels du fichier (hauteur)
 * @param max_num Le nombre maximum des données lues (doit valoir 255 = 1 octets)
 */
typedef struct Ppm_header {
    uint8_t format;
    uint16_t h;
    uint16_t v;
    uint8_t max_num;
} Ppm_header;


/**
 * @brief Structure représentant les valeurs Y, Cb et Cr d'un pixel
*/
typedef struct YCbCr {
    uint8_t Y;
    uint8_t Cb;
    uint8_t Cr;
} YCbCr;


/**
 * @brief Structure représentant les valeurs R, G, B d'un pixel
*/
typedef struct RGB {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB;


/**
 * @brief Ouvre en binaire un fichier ppm
 *
 * @param file_path Chemin du fichier ppm lu
 * @return le fichier lu
 */
FILE* open_bin_file(char* file_path);


/**
 * @brief Lis le header d'un fichier ppm 
 *
 * @param f Le fichier à lire
 * @return Une structure de header correspondant aux données lues
 */
Ppm_header read_ppm_header(FILE* f);


/**
 * @brief Calcule les valeurs YCbCr d'un pixel à partir de ses valeurs R, G et B.
 * 
 * @return Structure YCbCr du pixel
*/
YCbCr rgb_to_ycbcr(RGB rgb);


/**
 * @brief Assigne une couleur YCbCr à un pixel des composantes d'un block
 * 
 * @param block le block correspondant
 * @param pixel_v , @param pixel_h les coordonées du pixel dans les composantes
 * @param color la couleur à assigner
*/
void assign_ycbcr(Spat_block block, uint8_t pixel_v, uint8_t pixel_h, YCbCr color);

/**
 * @brief Lis les données binaire d'un fichier ppm pour construire une liste de MCUs, effectue le padding des MCUs
 * 
 * @param f le fichier ppm de lecture
 * @param header le header du fchier ppm
 * @param sample les valeurs de sous-echantillonage
 * 
 * @return La liste de MCUs correspondantes
*/
Spat_MCU* read_bin_ppm(FILE*f, Ppm_header header, Sample_val* sample);


/**
 * @brief Lis les données binaire d'un fichier pgm pour construire une liste de MCUs, effectue le padding des MCUs
 * 
 * @param f le fichier pgm de lecture
 * @param header le header du fchier pgm
 * @param sample les valeurs de sous-echantillonage
 * 
 * @return La liste de MCUs correspondantes
*/
Spat_MCU* read_bin_pgm(FILE*f, Ppm_header header, Sample_val* sample);


/**
 * @brief Lis les données des pixels d'un fichier ppm/pgm et construit une liste de MCU, effectue le padding des MCUs
 *
 * @param f Le fichier pgm à lire
 * @param header Le header du fichier ppm
 * @param sample Les valeurs du sous-echantillonage
 * 
 * @return La liste de MCUs correspondantes
 */
Spat_MCU* read_bin(FILE* f, Ppm_header header, Sample_val* sample);

#endif

