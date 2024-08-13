#ifndef _DCT_H_
#define _DCT_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "mcu.h"

#define M_PI 3.14159265358979323846


// ===== Coefficients de l'algorithme de Chen (décalés de 12 bits vers la gauche) =====
#define C1 2008         /* = 1/2*cos(M_PI/16)                                   */
#define C2 1892         /* = 1/2*cos(2*M_PI/16)                                 */
#define C3 1702         /* = 1/2*cos(3*M_PI/16)                                 */
#define C4 1448         /* = 1/2*cos(4*M_PI/16)                                 */
#define C5 1137         /* = 1/2*cos(5*M_PI/16)                                 */
#define C6 783          /* = 1/2*cos(6*M_PI/16)                                 */
#define C7 399          /* = 1/2*cos(7*M_PI/16)                                 */


// ===== Coefficients de la Loeffler-DCT (décalés de 10 bits vers la gauche) =====
#define KC1 1004        /* = cos(M_PI/16)                                       */
#define KC1_A 1204      /* = sin(M_PI/16) + cos(M_PI/16)                        */
#define KC1_MM 805      /* = -(sin(M_PI/16) - cos(M_PI/16))                     */

#define KC3 851         /* = cos(3*M_PI/16)                                     */
#define KC3_A 1420      /* = sin(3*M_PI/16) + cos(3*M_PI/16)                    */
#define KC3_MM 283      /* = sin(3*M_PI/16) - cos(3*M_PI/16))                   */

#define KC6 554         /* = sqrt(2)*cos(6*M_PI/16)                             */
#define KC6_A 1703      /* = sqrt(2)cos(6*M_PI/16) + sqrt(2)sin(6*M_PI/16)      */
#define KC6_M 784       /* = sqrt(2)*sin(6*M_PI/16) - sqrt(2)*cos(6*M_PI/16)    */

#define SQRT2 1448      /* = sqrt(2)                                            */



/**
 * @brief Implémentation d'une DCT-2D utilisant les coefficients de l'algorithme de Chen.
 * D'après la DCT-1D expliqué dans :
 * https://fr.wikipedia.org/wiki/Transform%C3%A9e_en_cosinus_discr%C3%A8te#DCT-II
 * 
 * @param composante La composante spatiale sur laquelle effectuer la DCT.
 * 
 * @return La composante fréquentielle calculée à part de la composante en paramètre.
*/
Freq_component* chen_DCT(Spat_component* composante) ;


/**
 * @brief Implémentation d'une DCT-2D en utilisant la DCT-1D de Loeffler à 8 points.
 * D'après le graphe de flow présent dans :
 * https://www.researchgate.net/publication/224567774_An_Improved_Scaled_DCT_Architecture
 * 
 * @param composante La composante spatiale sur laquelle effectuer la DCT.
 * 
 * @return La composante fréquentielle calculée à part de la composante en paramètre.
*/
Freq_component* loeffler_DCT(Spat_component* composante) ;


#endif