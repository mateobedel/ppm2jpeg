#include "encode.h"

//Indices précalculés du tableau d'un parcours zig-zag d'un block
uint8_t zigzag_index[BLOCK_LEN][BLOCK_LEN] = {
    {0,  1,  5,  6,  14, 15, 27, 28},
    {2,  4,  7,  13, 16, 26, 29, 42},
    {3,  8,  12, 17, 25, 30, 41, 43},
    {9,  11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
};


void encode_component(Spat_component* spat_comp, uint8_t ind_comp, int16_t* prec_DC, Buffer* buffer, FILE* file) {

    //Calcule de la DCT
    Freq_component* freq_comp = loeffler_DCT(spat_comp);

    // Zig-zag & Quantifications
    int16_t* zig_zag;
    zig_zag = zig_zag_et_quantification(freq_comp , (ind_comp==IND_Y));
    free_freq_component(freq_comp);

    //Encodage DC et AC & Ecriture dans le buffer
    prec_DC[ind_comp] = encode_DC(prec_DC[ind_comp], zig_zag[0], (ind_comp==IND_Y), buffer, file);
    encode_AC(zig_zag,  (ind_comp==IND_Y), buffer, file);

    free(zig_zag);  
}


void encode_mcus (FILE* outfile, Spat_MCU* spat_mcus, Ppm_header header, Sample_val* sample) {

    uint32_t nb_mcus = ceil_div(header.v,sample[IND_Y].v*BLOCK_LEN) * ceil_div(header.h,sample[IND_Y].h*BLOCK_LEN);
    
    Buffer* buffer = create_buffer();
    int16_t prec_DC[3] = {0, 0, 0};


    //Encodage des composantes
    for (uint32_t m = 0; m < nb_mcus; m++) {

        for (uint8_t i = 0; i < sample[IND_Y].v; i++) 
            for (uint8_t j = 0; j < sample[IND_Y].h; j++) 
                encode_component(spat_mcus[m].blocks[i][j].Y, IND_Y, prec_DC, buffer, outfile);
     
        if (header.format == PPM) {

            for (uint8_t i = 0; i < sample[IND_CB].v; i++) 
                for (uint8_t j = 0; j < sample[IND_CB].h; j++) 
                    encode_component(spat_mcus[m].blocks[i][j].Cb, IND_CB, prec_DC, buffer, outfile);
                
            for (uint8_t i = 0; i < sample[IND_CR].v; i++) 
                for (uint8_t j = 0; j < sample[IND_CR].h; j++) 
                    encode_component(spat_mcus[m].blocks[i][j].Cr, IND_CR, prec_DC, buffer, outfile);
        }
    }

    end_buffer(buffer, outfile);
}


int16_t* zig_zag_et_quantification (Freq_component* component, bool is_Y) {

    int16_t* tab = malloc(64 * sizeof(int16_t));
    uint8_t* quantification_table = is_Y ? quantification_table_Y : quantification_table_CbCr;

    for (uint8_t i = 0; i < BLOCK_LEN; i++) {
        for (uint8_t j = 0; j < BLOCK_LEN; j++) {
            tab[zigzag_index[i][j]] = component->pixel_value[i][j] / quantification_table[zigzag_index[i][j]];
        }
    }
    return tab;
}


Magnitude get_magnitude (int16_t number) {
    Magnitude magnitude;

    if (number > 0) {
        for (int m = 1; m < 12; m++){
            if (number <= (1<<m) - 1) {
                magnitude.class = m;
                break;
            }
        }
    } else {
        for (int m = 0; m < 12; m++){
            if (number >= 1 - (1 << m)) {
                magnitude.class = m;
                break;
            }  
        }
    }

    uint16_t range = (magnitude.class == 0) ? 1 : ((1<<magnitude.class) - (1<<(magnitude.class-1))) * 2;
                                  
    if (number > 0) 
        magnitude.index = number;
    else
        magnitude.index = range + number - 1;
        
    return magnitude;
}


int16_t encode_DC (int16_t prec_DC, int16_t DC, bool is_Y, Buffer* buffer, FILE* file) {

    int16_t value_to_encode = DC - prec_DC;
    // Récupération des information concernant la classe de magnitude de la valeur à encoder, 
    // ainsi que sur le code de huffman de sa classe de magnitude
    Magnitude mag = get_magnitude(value_to_encode);
    Code h_code = is_Y ? htables_dic_DC_Y[mag.class] : htables_dic_DC_CbCr[mag.class];

    add_to_buffer(h_code.code, h_code.len, buffer, file);
    add_to_buffer(mag.index, mag.class, buffer, file);

    return DC;
}


void encode_AC (int16_t* array, bool is_Y, Buffer* buffer, FILE* file) {

    uint8_t prev_nb_zero = 0;

    for (uint8_t i = 1; i < 64; i++) {
        // si le coefficient actuel est un 0, on le compte et on passe à l'itération suivante
        if (array[i] == 0) {
            prev_nb_zero += 1;
            continue;
        }

        Magnitude mag = get_magnitude(array[i]);

        // Lorsque l'on encode un coefficient non nul, si le nombre de 0 rencontrés depuis le précédent coefficient 
        // non nul est supérieur à 16, on doit écrire autant de fois que nécessaire le symbole ZRL.
        if (prev_nb_zero >= 16) {
            Code h_code = is_Y ? htables_dic_AC_Y[0xF0] : htables_dic_AC_CbCr[0xF0];            
            uint8_t nb_16_zeros = prev_nb_zero/16;
            prev_nb_zero %= 16;

            for (uint8_t i = 0; i < nb_16_zeros; i++) 
                add_to_buffer(h_code.code, h_code.len, buffer, file);
        }

        uint8_t rle_symb = (prev_nb_zero << 4) | (mag.class & 0x0F); 
        prev_nb_zero = 0;

        Code rle_h_code = is_Y ? htables_dic_AC_Y[rle_symb] : htables_dic_AC_CbCr[rle_symb];

        add_to_buffer(rle_h_code.code, rle_h_code.len, buffer, file);
        add_to_buffer(mag.index, mag.class, buffer, file);
    }

    // si on a fini de parcourir lee tableau des coefficients mais que le nombre de 0 rencontrés depuis le dernier
    // coefficient non nul est supérieur à 0, on écrit le symbole end of block.
    if (prev_nb_zero != 0) {
        Code h_code = is_Y ? htables_dic_AC_Y[0x00] : htables_dic_AC_CbCr[0x00];
        add_to_buffer(h_code.code, h_code.len, buffer, file);
    }
}
