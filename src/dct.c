#include "dct.h"

//Transformée Chen-DCT sous force matricielle
int16_t chen_DCT_mat[8][4] = {
    {C4,  C4,  C4,  C4},
    {C1,  C3,  C5,  C7},
    {C2,  C6, -C6, -C2},
    {C3, -C7, -C1, -C5},
    {C4, -C4, -C4,  C4},
    {C5, -C1,  C7,  C3},
    {C6, -C2,  C2, -C6},
    {C7, -C5,  C3, -C1}
};


Freq_component* chen_DCT(Spat_component* composante) {

    //Allocation composante frequentiel
    Freq_component* freq_composante = malloc(sizeof(Freq_component));
    freq_composante->pixel_value = malloc(sizeof(int16_t*) * BLOCK_LEN);

    for (uint8_t i=0; i<BLOCK_LEN; i++) 
        freq_composante->pixel_value[i] = malloc(sizeof(int16_t) * BLOCK_LEN);
    

    //Fast DCT-1D des colonnes
    for(uint8_t x = 0; x < BLOCK_LEN; x++) {

        //Vecteurs des termes spatiaux ayant le même cosinus comme facteur
        int16_t cos_terms[8];
        for(uint8_t k = 0; k < 8; k++)
            cos_terms[k] = (k%2 == 0) ? composante->pixel_value[k/2][x] + composante->pixel_value[7 - k/2][x] - 256
                                      : composante->pixel_value[k/2][x] - composante->pixel_value[7 - k/2][x];

        //Calcul du produit entre la matrice de la transformée et le vecteur
        for(uint8_t i = 0; i < BLOCK_LEN/2; i++) {
            int32_t even_somme = 0;
            int32_t odd_somme = 0;

            for(uint8_t j = 0; j < BLOCK_LEN/2; j++) {
                even_somme += cos_terms[j*2]*chen_DCT_mat[i*2][j];
                odd_somme += cos_terms[j*2+1]*chen_DCT_mat[i*2+1][j];
            }   

            freq_composante->pixel_value[i*2][x] = even_somme >> 12;
            freq_composante->pixel_value[i*2+1][x] = odd_somme >> 12;
        }
    }

    //Fast DCT-1D des lignes
    for(uint8_t y = 0; y < BLOCK_LEN; y++) {

        //Vecteurs des termes de la première DCT-1D ayant le même cosinus comme facteur
        int16_t cos_terms[8];
        for(uint8_t k = 0; k < 8; k++)
            cos_terms[k] = (k%2 == 0) ? freq_composante->pixel_value[y][k/2] + freq_composante->pixel_value[y][7 - k/2]
                                      : freq_composante->pixel_value[y][k/2] - freq_composante->pixel_value[y][7 - k/2];

        //Calcul du produit entre la matrice de la transformée et le vecteur
        for(uint8_t i = 0; i < BLOCK_LEN/2; i++) {
            int32_t even_somme = 0;
            int32_t odd_somme = 0;
            for(uint8_t j = 0; j < BLOCK_LEN/2; j++) {
                even_somme += cos_terms[j*2]*chen_DCT_mat[i*2][j];
                odd_somme += cos_terms[j*2+1]*chen_DCT_mat[i*2+1][j];
            }   


            freq_composante->pixel_value[y][i*2] = even_somme >> 12;
            freq_composante->pixel_value[y][i*2+1] = odd_somme >> 12;
        }
    }
    return freq_composante;
}


Freq_component* loeffler_DCT(Spat_component* composante) {

    //===== Allocation des composantes frequentiels =====
    Freq_component* freq_composante = malloc(sizeof(Freq_component));
    freq_composante->pixel_value = malloc(sizeof(int16_t*) * BLOCK_LEN);

    for (uint8_t i=0; i<BLOCK_LEN; i++) 
        freq_composante->pixel_value[i] = malloc(sizeof(int16_t) * BLOCK_LEN);


    // ===== Loeffler DCT-2D =====
    int32_t nodes[BLOCK_LEN]; // Valeurs des noeuds du graph de flow de Loeffler 1D
    int32_t temp_0, temp_1; // Variables temporaires


    // ===== Loeffler DCT-1D des colonnes =====
    for(uint8_t x = 0; x < BLOCK_LEN; x++) {
    
        // --- Etape n°1 --- (Butterfly & offset de 128)
        nodes[0] = composante->pixel_value[0][x] + composante->pixel_value[7][x] - 256;
        nodes[1] = composante->pixel_value[1][x] + composante->pixel_value[6][x] - 256;
        nodes[2] = composante->pixel_value[2][x] + composante->pixel_value[5][x] - 256;
        nodes[3] = composante->pixel_value[3][x] + composante->pixel_value[4][x] - 256;
        nodes[4] = composante->pixel_value[3][x] - composante->pixel_value[4][x];
        nodes[5] = composante->pixel_value[2][x] - composante->pixel_value[5][x];
        nodes[6] = composante->pixel_value[1][x] - composante->pixel_value[6][x];
        nodes[7] = composante->pixel_value[0][x] - composante->pixel_value[7][x];


        // --- Etape n°2 ---
        temp_0 = nodes[0];
        temp_1 = nodes[1];
        nodes[0] = nodes[0] + nodes[3];
        nodes[1] = nodes[1] + nodes[2];
        nodes[2] = temp_1 - nodes[2];
        nodes[3] = temp_0 - nodes[3];
        
        //On pre-calcule temp_1 pour eviter 1 multiplication & 1 addition
        temp_0 = nodes[4];
        temp_1 = KC3*(nodes[4]+nodes[7]); 
        nodes[4] = temp_1 - KC3_MM*nodes[7];
        nodes[7] = temp_1 - KC3_A*temp_0;

        temp_0 = nodes[5];
        temp_1 = KC1*(nodes[5]+nodes[6]);
        nodes[5] = temp_1 - KC1_MM*nodes[6];
        nodes[6] = temp_1 - KC1_A*temp_0;


        // --- Etape n°3 ---
        freq_composante->pixel_value[0][x] = (nodes[0] + nodes[1]);
        freq_composante->pixel_value[4][x] = nodes[0] - nodes[1];

        temp_0 = KC6*(nodes[2]+nodes[3]);
        freq_composante->pixel_value[2][x] = (KC6_M*nodes[3] + temp_0) >> 10;
        freq_composante->pixel_value[6][x] = (temp_0 - KC6_A*nodes[2]) >> 10;

        temp_0 = nodes[4];
        temp_1 = nodes[5];
        nodes[4] = nodes[4] + nodes[6];
        nodes[5] = (nodes[7] - nodes[5]);
        nodes[6] = (temp_0 - nodes[6]);
        nodes[7] = nodes[7] + temp_1;

        // --- Etape n°4 ---
        freq_composante->pixel_value[7][x] = (nodes[7] - nodes[4]) >> 10;
        freq_composante->pixel_value[3][x] = (nodes[5]*SQRT2) >> 20;
        freq_composante->pixel_value[5][x] = (nodes[6]*SQRT2) >> 20;
        freq_composante->pixel_value[1][x] = (nodes[7] + nodes[4]) >> 10;

    }

    // ===== Loeffler DCT-1D des lignes =====
    for(uint8_t y = 0; y < BLOCK_LEN; y++) {

        // --- Etape n°1 ---
        nodes[0] = freq_composante->pixel_value[y][0] + freq_composante->pixel_value[y][7];
        nodes[1] = freq_composante->pixel_value[y][1] + freq_composante->pixel_value[y][6];
        nodes[2] = freq_composante->pixel_value[y][2] + freq_composante->pixel_value[y][5];
        nodes[3] = freq_composante->pixel_value[y][3] + freq_composante->pixel_value[y][4];
        nodes[4] = freq_composante->pixel_value[y][3] - freq_composante->pixel_value[y][4];
        nodes[5] = freq_composante->pixel_value[y][2] - freq_composante->pixel_value[y][5];
        nodes[6] = freq_composante->pixel_value[y][1] - freq_composante->pixel_value[y][6];
        nodes[7] = freq_composante->pixel_value[y][0] - freq_composante->pixel_value[y][7];


        // --- Etape n°2 ---
        temp_0 = nodes[0];
        temp_1 = nodes[1];
        nodes[0] = nodes[0] + nodes[3];
        nodes[1] = nodes[1] + nodes[2];
        nodes[2] = temp_1 - nodes[2];
        nodes[3] = temp_0 - nodes[3];
        
        temp_0 = nodes[4];
        temp_1 = KC3*(nodes[4]+nodes[7]);
        nodes[4] = temp_1 - KC3_MM*nodes[7];
        nodes[7] = temp_1 - KC3_A*temp_0;

        temp_0 = nodes[5];
        temp_1 = KC1*(nodes[5]+nodes[6]);
        nodes[5] = temp_1 - KC1_MM*nodes[6];
        nodes[6] = temp_1 - KC1_A*temp_0;


        // --- Etape n°3 ---
        //Normalisation des coefficients (on divise par 2*2*sqrt(2)*sqrt(2) = 2^3)
        freq_composante->pixel_value[y][0] = (nodes[0] + nodes[1]) >> 3;
        freq_composante->pixel_value[y][4] = (nodes[0] - nodes[1]) >> 3;

        temp_0 = KC6*(nodes[2]+nodes[3]);
        freq_composante->pixel_value[y][2] = (KC6_M*nodes[3] + temp_0) >> 13;
        freq_composante->pixel_value[y][6] = (temp_0 - KC6_A*nodes[2]) >> 13;

        temp_0 = nodes[4];
        temp_1 = nodes[5];
        nodes[4] = nodes[4] + nodes[6];
        nodes[5] = (nodes[7] - nodes[5]);
        nodes[6] = (temp_0 - nodes[6]);
        nodes[7] = nodes[7] + temp_1;

        // --- Etape n°4 ---
        freq_composante->pixel_value[y][7] = (nodes[7] - nodes[4]) >> 13;
        freq_composante->pixel_value[y][3] = (nodes[5]*SQRT2) >> 23;
        freq_composante->pixel_value[y][5] = (nodes[6]*SQRT2) >> 23;
        freq_composante->pixel_value[y][1] = (nodes[7] + nodes[4]) >> 13;
    }

    return freq_composante;
}
