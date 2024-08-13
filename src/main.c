#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "handle_args.h"
#include "read_ppm.h"
#include "encode.h"
#include "write_jpeg.h"
#include "mcu.h"


int main(int argc, char* argv[]) {


    //  =============== Lecture des arguments ===============
    char* file_path = NULL;
    char* outfile_path = NULL;
    Sample_val sample[3] = {{.h = 1, .v = 1}, {.h = 1, .v = 1}, {.h = 1, .v = 1}};

    if (!read_args(argc, argv, &file_path, &outfile_path, sample)) {
        free_args(&file_path, &outfile_path);
        return EXIT_FAILURE;
    }



    //  =============== Lecture du ppm ===============
    // - Ouverture du fichier
    FILE* f = open_bin_file(file_path);
    if (f == NULL) {
        printf("[!] Erreur à l'ouverture du fichier.\n");
        free_args(&file_path, &outfile_path);
        return EXIT_FAILURE;
    }

    // - Lecture du header
    Ppm_header header = read_ppm_header(f);
    if (header.format == ERR || header.max_num != 255) {
        printf(fWARN "[!] Erreur à la lecture du header (format de header non prit en compte/incorrect).\n" fRESET);
        free_args(&file_path, &outfile_path);
        return EXIT_FAILURE;
    }

    // - Lecture des données
    Spat_MCU* spat_mcus = read_bin(f,header,sample);
    if (spat_mcus == NULL) {
        printf(fWARN "[!] Erreur à la lecture du fichier \n" fRESET);
        return EXIT_FAILURE;
    }
    fclose(f);


    // =============== Ecriture du fichier jpeg ===============
    // - Ecriture du header
    FILE* outfile = open_file(file_path, outfile_path);
    if (outfile == NULL) {
        printf(fWARN "[!] Impossible d'ouvrir le fichier. \n" fRESET);
        return EXIT_FAILURE;
    }
    write_JPEG_header(outfile, header, sample);

    // - Encodage
    bool downsampling =  !(sample[IND_Y].v == sample[IND_CB].v && sample[IND_Y].h == sample[IND_CB].h
                        && sample[IND_Y].v == sample[IND_CR].v && sample[IND_Y].h == sample[IND_CR].h);
    if (header.format == PPM && downsampling) 
        downsample(spat_mcus,header,sample);



    encode_mcus(outfile, spat_mcus, header, sample);

    // - Ecriture du marqueur de fin
    write_JPEG_eoi(outfile);
    close_file(outfile);


    //=============== Free des données ===============
    free_args(&file_path, &outfile_path);
    free_spat_mcus(spat_mcus, header, sample);


    return EXIT_SUCCESS;
}
