#include "handle_args.h"


char* str_save(char* s) {
    uint16_t str_len = strlen(s) + 1;
    char* new_s = malloc(str_len);
    if (new_s == NULL) return NULL;
    memcpy(new_s, s, str_len);
    return new_s;
}


void print_help() {
    printf("\n");
    printf( fTITLE "NOM\n" fRESET);
    printf("\t" fBOLD "ppm2jpeg" fRESET " - Encodeur de fichier pgm/ppm en fichier jpeg\n");
    printf("\t[!] Le nombre magique doit être P5 ou P6 avec un maximum de niveau de couleur de 255.\n\n");

    printf( fTITLE "SYNOPSYS\n" fRESET);
    printf(fBOLD "\t./ppm2jpeg" fRESET " [Chemin d'entrée]\n");
    printf(fBOLD "\t./ppm2jpeg --outfile=" fRESET "[Chemin de sortie] [Chemin d'entrée]\n" );
    printf(fBOLD "\t./ppm2jpeg --sample=" fRESET "[Sous-échantillonage] [Chemin d'entrée]\n\n");

    printf( fTITLE "EXEMPLES\n" fRESET);
    printf(fBOLD "\t./ppm2jpeg" fRESET " shaun_the_sheep.ppm\n");
    printf("\t\tGénèrera le fichier shaun_the_sheep.jpg\n\n");

    printf(fBOLD "\t./ppm2jpeg" fRESET " --outfile=../chemin/complique/mouton.jpg ./images/shaun_the_sheep.ppm\n");
    printf("\t\tGénèrera mouton.jpg à partir de shaun_the_sheep.ppm dans le répertoire ./images/, et l'écri dans ../chemin/complique/.\n\n");

    printf(fBOLD "\t./ppm2jpeg" fRESET " --sample=2x2,1x1,1x1 shaun_the_sheep.ppm\n");
    printf("\t\tGénèrera le fichier shaun_the_sheep.jpg pour lequel les composantes Cb et Cr ont été sous-échantillonnées.\n\n");
}


bool verify_sample(Sample_val sample[3]) {

    bool less_than_ten = sample[IND_Y].h*sample[IND_Y].v + sample[IND_CB].h*sample[IND_CB].v + sample[IND_CR].h*sample[IND_CR].v <= 10;

    bool between_one_and_four = (1 <= sample[IND_Y].h && sample[IND_Y].h <= 4) && (1 <= sample[IND_Y].v && sample[IND_Y].v <= 4)
                             && (1 <= sample[IND_CB].h && sample[IND_CB].h <= 4) && (1 <= sample[IND_CB].v && sample[IND_CB].v <= 4)
                             && (1 <= sample[IND_CR].h && sample[IND_CR].h <= 4) && (1 <= sample[IND_CR].v && sample[IND_CR].v <= 4);
                             
    bool divide_lum = (sample[IND_Y].h%sample[IND_CB].h == 0 && sample[IND_Y].v%sample[IND_CB].v == 0)
                   && (sample[IND_Y].h%sample[IND_CR].h == 0 && sample[IND_Y].v%sample[IND_CR].v == 0);

    bool divide_block = (BLOCK_LEN % sample[IND_Y].h == 0 && BLOCK_LEN % sample[IND_Y].v == 0)
                     && (BLOCK_LEN % sample[IND_CB].h == 0 && BLOCK_LEN % sample[IND_CB].v == 0)
                     && (BLOCK_LEN % sample[IND_CR].h == 0 && BLOCK_LEN % sample[IND_CR].v == 0);

    return less_than_ten && between_one_and_four && divide_lum && divide_block;
}


bool split_sample(char* str_sample, Sample_val sample[3]) {

    //Lecture de l'echantillonage
    uint8_t samples_scanned = sscanf(str_sample, "%hhdx%hhd,%hhdx%hhd,%hhdx%hhd", 
                                &(sample[IND_Y].h),  &(sample[IND_Y].v),
                                &(sample[IND_CB].h), &(sample[IND_CB].v),
                                &(sample[IND_CR].h), &(sample[IND_CR].v));

    if (!(samples_scanned == 6) || !verify_sample(sample)) {
        printf(fWARN "[!] Format ou valeurs de sous-echantillonage incorrects. (Attendu : --sample=(h1xv1,h2xv2,h3xv3))\n" fRESET);
        free(str_sample);
        return false;
    }

    free(str_sample);
    return true;
}


bool read_args(int argc, char* argv[], char** file_path, char** outfile, Sample_val sample[3]) {

    if (argc == 1 || argc > 4) {
        printf(fWARN "[!] Usage : ./ppm2jpeg [Entrée]\n" fRESET "    Tapez ./ppm2jpeg --help pour plus d'info.\n" );
        return false;
    }

    //Lecture des arguments
    for (uint8_t i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--help", 6) == 0) {
            print_help();
            return false;

        } else if (strncmp(argv[i], "--outfile=", 10) == 0) {
            *outfile = str_save(argv[i] + 10);

        } else if (strncmp(argv[i], "--sample=", 9) == 0) {
            char* str_sample = str_save(argv[i] + 9);
            if (!split_sample(str_sample, sample))  
                return false;
                
        } else {
            *file_path = str_save(argv[i]);
        }
    }

    if (*file_path == NULL) {
        printf(fWARN "[!] Usage : ./ppm2jpeg [Entrée]\n" fRESET "    Tapez ./ppm2jpeg --help pour plus d'info.\n" );
        return false;
    }

    return true;
}


void free_args(char** file_path, char** outfile) {
    if (*file_path != NULL) free(*file_path);
    if (*outfile != NULL) free(*outfile);
    return;
}
