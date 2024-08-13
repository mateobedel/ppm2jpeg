#include "mcu.h"


uint16_t ceil_div(uint16_t x, uint16_t y) {
    return x/y + (x % y != 0);
}


Spat_component* malloc_component() {

    Spat_component* C = malloc(sizeof(Spat_component));
    C->pixel_value = malloc(sizeof(uint8_t*)*BLOCK_LEN);

    for (uint8_t l=0; l<BLOCK_LEN; l++) 
       C->pixel_value[l] = malloc(sizeof(uint8_t)*BLOCK_LEN); 

    return C;
}


Spat_MCU* malloc_mcus(Ppm_header header, Sample_val* sample) {

    Couple16 nb_mcu = {.v = ceil_div(header.v,sample[IND_Y].v*BLOCK_LEN), .h = ceil_div(header.h,sample[IND_Y].h*BLOCK_LEN)};
    
    //Malloc des mcus
    Spat_MCU* mcus = malloc(sizeof(Spat_MCU)*nb_mcu.v*nb_mcu.h); 
    
    //Malloc des blocks
    for(uint32_t m = 0; m < nb_mcu.v*nb_mcu.h; m++) {
        mcus[m].blocks = malloc(sizeof(Spat_block*)*sample[IND_Y].v);

        for(uint8_t i=0; i<sample[IND_Y].v; i++) {
            mcus[m].blocks[i] = malloc(sizeof(Spat_block)*sample[IND_Y].h); 

           for(uint8_t j=0; j<sample[IND_Y].h; j++) {

                //Malloc des composantes
                mcus[m].blocks[i][j].Y = malloc_component();
                
                if (header.format == PPM) {

                    mcus[m].blocks[i][j].Cb = malloc_component();
                    mcus[m].blocks[i][j].Cr = malloc_component();

                } else {

                    mcus[m].blocks[i][j].Cr = NULL;
                    mcus[m].blocks[i][j].Cb = NULL;
                }
            }
        } 
    } 
    return mcus;    
}


uint8_t calc_moy_group(Spat_component* C, Couple16 gp, Couple16 p_gp) {

    uint16_t moyenne_gp = 0; //Moyenne des pixels du groupe
    Couple16 pos_gp; //Position du pixel dans le groupe

    for(pos_gp.v = 0; pos_gp.v < gp.v; pos_gp.v++) 
        for(pos_gp.h = 0; pos_gp.h < gp.h; pos_gp.h++) 
            moyenne_gp += C->pixel_value[p_gp.v + pos_gp.v][p_gp.h + pos_gp.h];
        
    return moyenne_gp/(gp.v*gp.h);
}


void downsample_component(Spat_component* ech_component, Spat_component** component, Couple16 gp, int8_t i, int8_t j, Sample_val sample) {

    //Indice du groupe dans le block
    Couple16 ind_gp; 

    for(ind_gp.v=0; ind_gp.v < BLOCK_LEN/gp.v; ind_gp.v++) { 
        for(ind_gp.h=0; ind_gp.h < BLOCK_LEN/gp.h; ind_gp.h ++) {

            //Position du groupe dans le block
            Couple16 p_gp = {.v = ind_gp.v*gp.v, .h = ind_gp.h *gp.h}; 

            //Position du pixel echantilloné
            Couple16 ech_p = {.v = (i%gp.v)*(BLOCK_LEN/gp.v) + p_gp.v/gp.v, 
                              .h = (j%gp.h)*(BLOCK_LEN/gp.h) + p_gp.h/gp.h};

            uint8_t moy_group = calc_moy_group(*component, gp, p_gp);

            ech_component->pixel_value[ech_p.v][ech_p.h] = moy_group;
        }           
    }
    
    if (i >= sample.v || j >= sample.h) {
        free_spat_component(*component);
        *component = NULL;
    }
}


void downsample(Spat_MCU* mcus, Ppm_header header, Sample_val* sample) {
        
    Couple16 dim_mcu = {.v = sample[IND_Y].v*BLOCK_LEN, .h = sample[IND_Y].h*BLOCK_LEN};
    Couple16 nb_mcu = {.v = ceil_div(header.v, dim_mcu.v), .h = ceil_div(header.h, dim_mcu.h)};

    //Taille des groupes des composantes
    Couple16 gp_Cb = {.v = sample[IND_Y].v/sample[IND_CB].v, .h = sample[IND_Y].h/sample[IND_CB].h};
    Couple16 gp_Cr = {.v = sample[IND_Y].v/sample[IND_CR].v, .h = sample[IND_Y].h/sample[IND_CR].h};

    //Iterations sur tout les blocks
    for(uint32_t m = 0; m < nb_mcu.v*nb_mcu.h; m++) {
        for(uint8_t i=0; i<sample[IND_Y].v; i++) {
           for(uint8_t j=0; j<sample[IND_Y].h; j++) {

                Couple16 ech_block_Cb = {.v = i/gp_Cb.v, .h = j/gp_Cb.h};
                Couple16 ech_block_Cr = {.v = i/gp_Cr.v, .h = j/gp_Cr.h};

                downsample_component(mcus[m].blocks[ech_block_Cb.v][ech_block_Cb.h].Cb, &mcus[m].blocks[i][j].Cb, gp_Cb, i, j, sample[IND_CB]);
                downsample_component(mcus[m].blocks[ech_block_Cr.v][ech_block_Cr.h].Cr, &mcus[m].blocks[i][j].Cr, gp_Cr, i, j, sample[IND_CR]);   
            }
        }
    }
}


void free_spat_component(Spat_component* component) {
    for (uint8_t k=0; k<BLOCK_LEN; k++) 
       free(component->pixel_value[k]);
    free(component->pixel_value);
    free(component);
}


void free_freq_component(Freq_component* component) {
    for (uint8_t k=0; k<BLOCK_LEN; k++) 
       free(component->pixel_value[k]);
    free(component->pixel_value);
    free(component);
}


void free_spat_mcus(Spat_MCU* mcus, Ppm_header header, Sample_val* sample) {

    Couple16 dim_mcu = {.v = sample[IND_Y].v*BLOCK_LEN, .h = sample[IND_Y].h*BLOCK_LEN};
    Couple16 nb_mcu = {.v = ceil_div(header.v, dim_mcu.v), .h = ceil_div(header.h, dim_mcu.h)};
    
    for(uint32_t m=0; m<nb_mcu.v*nb_mcu.h; m++) {
        for(uint8_t i=0; i<sample[IND_Y].v; i++) 
            for(uint8_t j=0; j<sample[IND_Y].h; j++) 
                free_spat_component(mcus[m].blocks[i][j].Y);

        if (header.format == PPM) {
            for(uint8_t i=0; i<sample[IND_CB].v; i++) {
                for(uint8_t j=0; j<sample[IND_CB].h; j++) {
                free_spat_component(mcus[m].blocks[i][j].Cb);
                }
            }

            for(uint8_t i=0; i<sample[IND_CR].v; i++) 
                for(uint8_t j=0; j<sample[IND_CR].h; j++) 
                free_spat_component(mcus[m].blocks[i][j].Cr);
        }

        for(uint8_t i=0; i<sample[IND_Y].v; i++) 
            free(mcus[m].blocks[i]);

        free(mcus[m].blocks);
    }
    free(mcus);
    return;
}


void print_spat_MCU(Spat_MCU mcu, Sample_val* sample, bool is_downsampled, Ppm_header header) {
    
    printf("--- Y ---\n");
    for(uint8_t i = 0; i < sample[IND_Y].v; i ++) {
        for(uint8_t k = 0; k < BLOCK_LEN; k++) {
            for(uint8_t j = 0; j < sample[IND_Y].h; j ++) {
                for(uint8_t l = 0; l < BLOCK_LEN; l++) {
                    printf("%02hx ",mcu.blocks[i][j].Y->pixel_value[k][l]);
                }
                printf("\t");
            }
            printf("\n");
        }
        printf("\n");
    }


    if (header.format == PPM) {
        printf("--- Cb ---\n");
        uint8_t ind;
        ind = is_downsampled ? IND_CB : IND_Y;
        for(uint8_t i = 0; i < sample[ind].v; i ++) {
            for(uint8_t k = 0; k < BLOCK_LEN; k++) {
                for(uint8_t j = 0; j < sample[ind].h; j ++) {
                    for(uint8_t l = 0; l < BLOCK_LEN; l++) {
                        printf("%02hx ",mcu.blocks[i][j].Cb->pixel_value[k][l]);
                    }
                    printf("\t");
                }
                printf("\n");
            }
            printf("\n");
        }
    
        printf("--- Cr ---\n");
        ind = is_downsampled ? IND_CR : IND_Y;
        for(uint8_t i = 0; i < sample[ind].v; i ++) {
            for(uint8_t k = 0; k < BLOCK_LEN; k++) {
                for(uint8_t j = 0; j < sample[ind].h; j ++) {
                    for(uint8_t l = 0; l < BLOCK_LEN; l++) {
                        printf("%02hx ",mcu.blocks[i][j].Cr->pixel_value[k][l]);
                    }
                    printf("\t");
                }
                printf("\n");
            }
            printf("\n");
        }
    }
}