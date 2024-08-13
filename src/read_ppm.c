#include <read_ppm.h>


FILE* open_bin_file(char* file_path) {
    return fopen(file_path, "rb");
}


Ppm_header read_ppm_header(FILE* f) {

    Ppm_header header;

    //Lecture du format
    char type;
    if (fscanf(f, "%*c%c", &type) != 1 || (type != '5' && type != '6'))
        header.format = ERR;
    else
        header.format = (uint8_t)(type - '0');

    //Lecture de la taille et du nombre max
    if (fscanf(f, "%hd %hd %hhd", &(header.h), &(header.v), &(header.max_num)) != 3) 
        header.format = ERR;

    return header;

}


YCbCr rgb_to_ycbcr(RGB rgb) {
    YCbCr color;

    //Valeurs shiftés pour eviter les operations entre floats
    color.Y = (19595*rgb.R + 38469*rgb.G + 7471*rgb.B) >> 16;
    color.Cb = (-11055*rgb.R - 21712*rgb.G + 32768*rgb.B + 8388608) >> 16;
    color.Cr = (32768*rgb.R - 27439*rgb.G - 5328*rgb.B + 8388608) >> 16;
    return color;
}


void assign_ycbcr(Spat_block block, uint8_t pixel_v, uint8_t pixel_h, YCbCr color) {
    block.Y->pixel_value[pixel_v][pixel_h] = color.Y;
    block.Cb->pixel_value[pixel_v][pixel_h] = color.Cb;
    block.Cr->pixel_value[pixel_v][pixel_h] = color.Cr;
}


Spat_MCU* read_bin_ppm(FILE*f, Ppm_header header, Sample_val* sample) {

    Couple16 nb_mcu = {.v = ceil_div(header.v,sample[IND_Y].v*BLOCK_LEN), .h = ceil_div(header.h,sample[IND_Y].h*BLOCK_LEN)};
    
    //Allocation des MCUs
    Spat_MCU* mcus = malloc_mcus(header, sample);

    //Couleur sauvegardées
    YCbCr color;
    YCbCr pad_h = {.Y = 0, .Cb = 0, .Cr = 0};
    YCbCr* pad_v = malloc(sizeof(YCbCr)*header.h);
    uint16_t nb_read;

    //Buffer des pixels lues
    RGB* rgb_buffer = malloc(sizeof(RGB)*header.h);

    //Indices du pixel dans la MCU
    Couple16 id_pixel = {.v = 0, .h = 0};

    for(uint32_t mcu_v = 0; mcu_v < nb_mcu.v; mcu_v++) {
        for(int8_t block_v = 0; block_v < sample[IND_Y].v; block_v++) {
            for(int8_t pixel_v = 0; pixel_v < BLOCK_LEN; pixel_v++) {

                //Lecture de la ligne de pixel dans le buffer
                if (id_pixel.v < header.v) {
                    nb_read = fread(rgb_buffer, sizeof(RGB), header.h, f);
                    if (nb_read != header.h) return NULL;
                }

                for(uint32_t mcu_h = 0; mcu_h < nb_mcu.h; mcu_h++) {
                    for(int8_t block_h = 0; block_h < sample[IND_Y].h; block_h++) {
                        for(int8_t pixel_h = 0; pixel_h < BLOCK_LEN; pixel_h++) { 

                            if (id_pixel.v < header.v && id_pixel.h < header.h) {

                                //Ecriture des couleurs dans les MCUs
                                color = rgb_to_ycbcr(rgb_buffer[id_pixel.h]);
                                assign_ycbcr(mcus[mcu_v*nb_mcu.h + mcu_h].blocks[block_v][block_h], pixel_v, pixel_h, color);
                                
                                //Sauvegarde des couleurs pour le padding
                                if (id_pixel.h == header.h - 1) pad_h = color;
                                if (id_pixel.v == header.v - 1) pad_v[id_pixel.h] = color;
 
                            //Padding des couleurs
                            } else if (id_pixel.h < header.h) {
                                assign_ycbcr(mcus[mcu_v*nb_mcu.h + mcu_h].blocks[block_v][block_h], pixel_v, pixel_h, pad_v[id_pixel.h]);
                            } else {
                                assign_ycbcr(mcus[mcu_v*nb_mcu.h + mcu_h].blocks[block_v][block_h], pixel_v, pixel_h, pad_h);
                            }
                            id_pixel.h++;
                        }
                    }
                }
                id_pixel.h = 0;
                id_pixel.v++;
            }
        }
    }
    free(rgb_buffer);
    free(pad_v);
    return mcus;
}


Spat_MCU* read_bin_pgm(FILE*f, Ppm_header header, Sample_val* sample) {

    Couple16 nb_mcu = {.v = ceil_div(header.v,sample[IND_Y].v*BLOCK_LEN), .h = ceil_div(header.h,sample[IND_Y].h*BLOCK_LEN)};
    
    //Allouer les MCU
    Spat_MCU* mcus = malloc_mcus(header, sample);

    //Couleur sauvegardées
    char L = 0;
    char pad_h = 0;
    char* pad_v = malloc(sizeof(char)*header.h);
    uint16_t nb_read;

    //Buffer des luminances lues
    char* lum_buffer = malloc(sizeof(char)*header.h);

    Couple16 id_pixel = {.v = 0, .h = 0};

    for(uint32_t mcu_v = 0; mcu_v < nb_mcu.v; mcu_v++) {

        for(uint32_t block_v = 0; block_v < sample[IND_Y].v; block_v++) {
            for(uint32_t pixel_v = 0; pixel_v < BLOCK_LEN; pixel_v++) {

                //Lecture de la ligne de pixel dans le buffer
                if (id_pixel.v < header.v) {
                    nb_read = fread(lum_buffer, sizeof(char), header.h, f);
                    if (nb_read != header.h) return NULL;
                }

                for(uint32_t mcu_h = 0; mcu_h < nb_mcu.h; mcu_h++) {
                    for(uint32_t block_h = 0; block_h < sample[IND_Y].h; block_h++) {
                        for(uint32_t pixel_h = 0; pixel_h < BLOCK_LEN; pixel_h++) {
                            
                            if (id_pixel.v < header.v && id_pixel.h < header.h) {

                                //Ecriture des couleurs dans les MCUs
                                mcus[mcu_v*nb_mcu.h + mcu_h].blocks[block_v][block_h].Y->pixel_value[pixel_v][pixel_h] = lum_buffer[id_pixel.h];

                                //Sauvegarde des couleurs pour le padding
                                if (id_pixel.h == header.h - 1) pad_h = L;
                                if (id_pixel.v == header.v - 1) pad_v[id_pixel.h] = L;

                            } else if (id_pixel.h < header.h) {
                                mcus[mcu_v*nb_mcu.h + mcu_h].blocks[block_v][block_h].Y->pixel_value[pixel_v][pixel_h] = pad_v[id_pixel.h];
                            } else {
                                mcus[mcu_v*nb_mcu.h + mcu_h].blocks[block_v][block_h].Y->pixel_value[pixel_v][pixel_h] = pad_h;
                            }
                            id_pixel.h++;
                        }
                    }
                }
                id_pixel.h = 0;
                id_pixel.v++;
            }
        }
    }
    free(lum_buffer);
    free(pad_v);
    return mcus;
}


Spat_MCU* read_bin(FILE* f, Ppm_header header, Sample_val* sample) {

    fseek(f, 1, SEEK_CUR); //Sauter le \n

    if (header.format == PGM) 
        return read_bin_pgm(f,header,sample);
    
    if (header.format == PPM)
         return read_bin_ppm(f,header,sample);
    
    return NULL;

}