#include "write_jpeg.h"


FILE* open_file (char* default_file_path, char* given_file_path) {
    FILE* file;
    if (given_file_path == NULL) {
        convert_ppm_path_to_jpeg(default_file_path);
        file = fopen(default_file_path, "wb");
    }
    else {
        file = fopen(given_file_path, "wb");
    }
    return file;
}


void convert_ppm_path_to_jpeg (char* ppm_path) {
    char *ptr = strstr(ppm_path, ".ppm");
    if (ptr == NULL)
        ptr = strstr(ppm_path, ".pgm");
    if (ptr != NULL) 
        strcpy(ptr, ".jpg");
}


extern void write_JPEG_header (FILE* outfile, Ppm_header header, Sample_val* sample_val) {
    uint8_t header_first_part[20] = {
        //SOI
        0xff, 0xd8, 
        //APP0
        0xff, 0xe0,
        0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };
    uint8_t* header_sec_part =  (header.format == PGM) 
                                    ?  header_sec_part_gray (header, sample_val) 
                                    :  header_sec_part_col (header, sample_val);

    fwrite(header_first_part, sizeof(uint8_t), 20, outfile);
    fwrite(header_sec_part, sizeof(uint8_t), (header.format == PGM ? 304 : 587), outfile);
    free(header_sec_part);
}


uint8_t* header_sec_part_gray (Ppm_header header, Sample_val* sample_val) {

    uint8_t header_DQT_section[]    = { 0xff, 0xdb, 0x00, 0x43, 0x00 };
    uint8_t header_SOF_section_1[]  = { 0xff, 0xc0, 0x00, 0x0b, 0x08 };
    uint8_t header_SOF_section_2[]  = { 0x01, 0x01 };
    uint8_t header_SOF_section_3[]  = { 0x00 };
    uint8_t header_DHT_section_1[]  = { 0xff, 0xc4, 0x00, 0xd2, 0x00 };
    uint8_t header_DHT_section_2[]  = { 0x10 };
    uint8_t header_SOS_section[]    = { 0xff, 0xda, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x00 };

    uint8_t ech_value = (sample_val->h << 4) | (sample_val->v & 0x0f);
 
    uint8_t height_val[] = {(header.v) >> 8, (header.v) & 0x00ff};
    uint8_t width_val[] = {(header.h) >> 8, (header.h) & 0x00ff};

    uint8_t* header_sec_part = malloc(304 * sizeof(uint8_t));

    memcpy(header_sec_part          , header_DQT_section                , 5 * sizeof(uint8_t));
    memcpy(header_sec_part + 5      , quantification_table_Y            , 64 * sizeof(uint8_t));
    memcpy(header_sec_part + 69     , header_SOF_section_1              , 5 * sizeof(uint8_t));
    memcpy(header_sec_part + 74     , height_val                        , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 76     , width_val                         , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 78     , header_SOF_section_2              , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 80     , &ech_value                        , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 81     , header_SOF_section_3              , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 82     , header_DHT_section_1              , 5 * sizeof(uint8_t));
    memcpy(header_sec_part + 87     , htables_nb_symb_per_lengths[0][0] , 16 * sizeof(uint8_t));
    memcpy(header_sec_part + 103    , htables_symbols[0][0]             , 12 * sizeof(uint8_t));
    memcpy(header_sec_part + 115    , header_DHT_section_2              , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 116    , htables_nb_symb_per_lengths[1][0] , 16 * sizeof(uint8_t));
    memcpy(header_sec_part + 132    , htables_symbols[1][0]             , 162 * sizeof(uint8_t));
    memcpy(header_sec_part + 294    , header_SOS_section                , 10 * sizeof(uint8_t));
    
    return header_sec_part;
}


uint8_t* header_sec_part_col (Ppm_header header, Sample_val* sample_val) {

    uint8_t header_DQT_section_1[]  = { 0xff, 0xdb, 0x00, 0x84, 0x00 };     // Suivi de table de qt n°0
    uint8_t header_DQT_section_2[]  = { 0x01 };                             // Suivi de table de qt n°1

    uint8_t header_SOF_section_1[]  = { 0xff, 0xc0, 0x00, 0x11, 0x08 };     // Suivi de hauteur, puis largeur
    uint8_t header_SOF_section_2[]  = { 0x03, 0x01 };                       // Suivi de facteur ech composante 1 (Y)
    uint8_t header_SOF_section_3[]  = { 0x00, 0x02 };                       // Suivi de facteur ech composante 2 (Cb)
    uint8_t header_SOF_section_4[]  = { 0x01, 0x03 };                       // Suivi de facteur ech composante 3 (Cr)
    uint8_t header_SOF_section_5[]  = { 0x01 };                             

    uint8_t header_DHT_section_1[]  = { 0xff, 0xc4, 0x01, 0xa2, 0x00 };     // Suivi de tab nb symbole puis tab huf DC_Y
    uint8_t header_DHT_section_2[]  = { 0x01 };                             // Suivi de tab nb symb, puis tab huf DC_CbCr
    uint8_t header_DHT_section_3[]  = { 0x10 };                             // Suivi de tab nb symb, puis tab huf AC_Y
    uint8_t header_DHT_section_4[]  = { 0x11 };                             // Suivi de tab nb symb, puis tab huf AC_CbCr

    uint8_t header_SOS_section[]    = { 0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00 };

    uint8_t ech_value[] = {
        (sample_val[0].h << 4) | (sample_val[0].v & 0x0f), 
        (sample_val[1].h << 4) | (sample_val[1].v & 0x0f), 
        (sample_val[2].h << 4) | (sample_val[2].v & 0x0f)
    };
 
    uint8_t height_val[] = {(header.v) >> 8, (header.v) & 0x00ff};
    uint8_t width_val[] = {(header.h) >> 8, (header.h) & 0x00ff};

    uint8_t* header_sec_part = malloc(587 * sizeof(uint8_t));

    memcpy(header_sec_part      , header_DQT_section_1                  , 5 * sizeof(uint8_t));
    memcpy(header_sec_part + 5  , quantification_table_Y                , 64 * sizeof(uint8_t));
    memcpy(header_sec_part + 69 , header_DQT_section_2                  , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 70 , quantification_table_CbCr             , 64 * sizeof(uint8_t));

    memcpy(header_sec_part + 134, header_SOF_section_1                  , 5 * sizeof(uint8_t));
    memcpy(header_sec_part + 139, height_val                            , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 141, width_val                             , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 143, header_SOF_section_2                  , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 145, ech_value                             , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 146, header_SOF_section_3                  , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 148, ech_value + 1                         , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 149, header_SOF_section_4                  , 2 * sizeof(uint8_t));
    memcpy(header_sec_part + 151, ech_value + 2                         , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 152, header_SOF_section_5                  , 1 * sizeof(uint8_t));

    memcpy(header_sec_part + 153, header_DHT_section_1                  , 5 * sizeof(uint8_t));
    memcpy(header_sec_part + 158, htables_nb_symb_per_lengths[0][0]     , 16 * sizeof(uint8_t));
    memcpy(header_sec_part + 174, htables_symbols[0][0]                 , 12 * sizeof(uint8_t));    

    memcpy(header_sec_part + 186, header_DHT_section_2                  , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 187, htables_nb_symb_per_lengths[0][1]     , 16 * sizeof(uint8_t));
    memcpy(header_sec_part + 203, htables_symbols[0][1]                 , 12 * sizeof(uint8_t));    

    memcpy(header_sec_part + 215, header_DHT_section_3                  , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 216, htables_nb_symb_per_lengths[1][0]     , 16 * sizeof(uint8_t));
    memcpy(header_sec_part + 232, htables_symbols[1][0]                 , 162 * sizeof(uint8_t)); 

    memcpy(header_sec_part + 394, header_DHT_section_4                  , 1 * sizeof(uint8_t));
    memcpy(header_sec_part + 395, htables_nb_symb_per_lengths[1][1]     , 16 * sizeof(uint8_t));
    memcpy(header_sec_part + 411, htables_symbols[1][1]                 , 162 * sizeof(uint8_t)); 

    memcpy(header_sec_part + 573, header_SOS_section                    , 14 * sizeof(uint8_t));   

    return header_sec_part;  
}


uint16_t reverse_bytes(uint16_t num) {
    return ((num) >> 8) | ((num) << 8);
}


extern void write_file (uint16_t* to_write, FILE* file) {
    uint16_t reversed_to_write = reverse_bytes(*to_write);
    fwrite(&reversed_to_write, sizeof(uint16_t), 1, file);
}


extern void write_JPEG_eoi (FILE* file_ptr) {
    uint8_t eoi_mark[] = {0xff, 0xd9};
    fwrite(eoi_mark, sizeof(uint8_t), 2, file_ptr);
}


extern void close_file (FILE* file_ptr) {
    fclose(file_ptr);
}