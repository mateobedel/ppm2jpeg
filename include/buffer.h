#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include "write_jpeg.h"


#define BUFFER_SIZE 16


/**
 * @brief Structure représentant le buffer. On utilise un entier non signé sur 16 bits comme buffer
 * 
 * @param content Entier signé sur 16 bits représentant le buffer en lui même
 * @param nb_bits_inside Nombre de bits utilisés dans le buffer. Utile au calcul des décalages et 
 * des masques lors de l'insertion de nouveaux bits dans le buffer.
*/
typedef struct Buffer {
    uint16_t content;
    uint8_t nb_bits_inside;
} Buffer;


/**
 * @brief Alloue la place nécessaire au buffer et initialise les variables à 0
*/
Buffer* create_buffer ();


/**
 * @brief Libère la mémoiree associée au buffer
*/
void free_buffer (Buffer* byte_buffer);


/**
 * @brief Construis et retourne le masque permettant de récupérer un certains nombre des bits de 
 * poids faibles d'un nombre non signé sur 16 bits.
 * 
 * @param size Nombre de bits que l'on souhaite récupérer
*/
uint16_t build_mask (uint8_t size);


/**
 * @brief Ecrit le buffer dans le fichier lorsqu'il est plein puis le vide. Gère le byte stuffing
 * 
 * @param byte_buffer Buffer à flush
 * @param file Pointeur vers le fichier où écrire le contenu du buffer
*/
void flush_buffer (Buffer* byte_buffer, FILE* file);


/**
 * @brief Ajoute au buffer 16 bits les bits passés en paramètre. 
 * Si le nombre de bits à ajouter est supérieur à la place restante dans le buffer, les bits pouvant 
 * être insérés le sont, le buffer est vidé et les bits n'ayant pas pu être insérés sont sauvegardés 
 * puis enregistrés dans le buffer une fois celui-ci vidé.
 * 
 * @param nb_to_add Nombre à ajouter au format 8 bits.
 * @param size Nombre effectif dde bits nécessaires pour coder le nombre à ajouter
*/
void add_to_buffer (uint16_t nb_to_add, uint8_t size, Buffer* byte_buffer, FILE* file);


/**
 * @brief Appel à cette fonction lorsque l'écriture des bits dans le fichier est finie. Le buffer est
 * écrit dans le fichier s'il n'est pas vide, puis la mémoire est libérée.
*/
void end_buffer (Buffer* byte_buffer, FILE* file);


#endif
