#include "../test/test.h"
#include "../include/write_jpeg.h"

#define NB_TESTS 8


// Test d'ouvertue du fichier
Test_result open_file_test () {
    char outfile_path[] = "invader.ppm";
    FILE* file = open_file(outfile_path, NULL);
    if (file == NULL)
        return (Test_result){.passed=false, .comment="FAIL open_file_test : Echec de l'ouverture du fichier"};
    return (Test_result){.passed=true, .comment="PASSED open_file_test"};
}


// Test de conversion du chemin vers le fichiers source PPM en chemin vers fichier de sortie (jpg)
Test_result convert_ppm_path_test () {
    char path[] = "test.ppm";
    convert_ppm_path_to_jpeg(path);
    if (strcmp(path, "test.jpg") == 0)
        return (Test_result){.passed=true, .comment="PASSED convert_ppm_path_test"};
    return (Test_result){.passed=false, .comment="FAIL convert_ppm_path_test : Le chemin n'a pas pu être correctement converti."};
}


// Test de conversion du chemin vers le fichiers source PGM en chemin vers fichier de sortie (jpg)
Test_result convert_pgm_path_test () {
    char path[] = "test.pgm";
    convert_ppm_path_to_jpeg(path);
    if (strcmp(path, "test.jpg") == 0)
        return (Test_result){.passed=true, .comment="PASSED convert_pgm_path_test"};
    return (Test_result){.passed=false, .comment="FAIL convert_pgm_path_test : Le chemin n'a pas pu être correctement converti."};
}


// Test reverse_bytes
Test_result reverse_bytes_test () {
    if (reverse_bytes(0x1234) == 0x3412)
        return (Test_result){.passed=true, .comment="PASSED reverse_bytes_test"};
    return (Test_result){.passed=false, .comment="FAIL reverse_bytes_test : Inversion des bytes échouées."};
}


// Test de l'écriture dans le fichier
Test_result write_file_test () {
    FILE* file = fopen("output_test", "wb+");
    uint16_t to_write = 0x1234;
    write_file(&to_write, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (content[0] == 0x12 && content[1] == 0x34)
        return (Test_result){.passed=true, .comment="PASSED ecriture_fichier_test"};
    return (Test_result){.passed=false, .comment="FAIL ecriture_fichier_test : Impossible d'écrire dans le fichier"};
}


// Test du header pour les ficier PGM
Test_result header_gray_test () {
    Ppm_header header = {.format=PGM, .h=64, .v=64, .max_num=255};
    Sample_val sample = {.h=1, .v=1};
    FILE* file = fopen("output_test", "wb+");

    write_JPEG_header(file, header, &sample);
        
    fseek(file, 0, SEEK_SET);
    uint8_t content[304];
    uint16_t nb_unit_read = fread(content, sizeof(uint8_t), 304, file);
    if (nb_unit_read != 304)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    uint8_t expected_result[304] = {
        0xff, 0xd8, 0xff, 0xe0, 0x0, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0xff, 0xdb, 0x0, 0x43, 0x0, 0x5, 0x3, 0x3, 0x5, 0x7, 0xc, 0xf, 0x12, 0x4, 0x4, 0x4, 0x6, 0x8, 0x11, 0x12, 0x11, 
        0x4, 0x4, 0x5, 0x7, 0xc, 0x11, 0x15, 0x11, 0x4, 0x5, 0x7, 0x9, 0xf, 0x1a, 0x18, 0x13, 0x5, 0x7, 0xb, 0x11, 0x14, 
        0x21, 0x1f, 0x17, 0x7, 0xb, 0x11, 0x13, 0x18, 0x1f, 0x22, 0x1c, 0xf, 0x13, 0x17, 0x1a, 0x1f, 0x24, 0x24, 0x1e, 
        0x16, 0x1c, 0x1d, 0x1d, 0x22, 0x1e, 0x1f, 0x1e, 0xff, 0xc0, 0x0, 0xb, 0x8, 0x0, 0x40, 0x0, 0x40, 0x1, 0x1, 0x11, 
        0x0, 0xff, 0xc4, 0x0, 0xd2, 0x0, 0x0, 0x1, 0x5, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0x10, 0x0, 0x2, 0x1, 0x3, 0x3, 0x2, 0x4, 0x3, 0x5, 
        0x5, 0x4, 0x4, 0x0, 0x0, 0x1, 0x7d, 0x1, 0x2, 0x3, 0x0, 0x4, 0x11, 0x5, 0x12, 0x21, 0x31, 0x41, 0x6, 0x13, 0x51, 
        0x61, 0x7, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x8, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 
        0x33, 0x62, 0x72, 0x82, 0x9, 0xa, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 
        0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 
        0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 
        0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 
        0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 
        0xe6, 0xe7, 0xe8, 0xe9, 0xea
    };

    for (int i=0; i < 304; i++)
        if (content[i] != expected_result[i])
            return (Test_result){.passed=false, .comment="FAIL header_gray : Header non conforme"};
    return (Test_result){.passed=true, .comment="PASSED header_gray"};
}


// Test header couleur
Test_result header_col_test () {
    Ppm_header header = {.format=PPM, .h=64, .v=64, .max_num=255};
    Sample_val sample[] = {{.h=1, .v=1}, {.h=1, .v=1}, {.h=1, .v=1}};
    FILE* file = fopen("output_test", "wb+");
    
    write_JPEG_header(file, header, sample);
        
    fseek(file, 0, SEEK_SET);
    uint8_t content[587];
    uint16_t nb_unit_read = fread(content, sizeof(uint8_t), 587, file);
    if (nb_unit_read != 587)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    uint8_t expected_result[587] = {
        0xff, 0xd8, 0xff, 0xe0, 0x0, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0xff, 0xdb, 0x0, 0x84, 0x0, 0x5, 0x3, 0x3, 0x5, 0x7, 0xc, 0xf, 0x12, 0x4, 0x4, 0x4, 0x6, 0x8, 0x11, 0x12, 
        0x11, 0x4, 0x4, 0x5, 0x7, 0xc, 0x11, 0x15, 0x11, 0x4, 0x5, 0x7, 0x9, 0xf, 0x1a, 0x18, 0x13, 0x5, 0x7, 0xb, 
        0x11, 0x14, 0x21, 0x1f, 0x17, 0x7, 0xb, 0x11, 0x13, 0x18, 0x1f, 0x22, 0x1c, 0xf, 0x13, 0x17, 0x1a, 0x1f, 0x24, 
        0x24, 0x1e, 0x16, 0x1c, 0x1d, 0x1d, 0x22, 0x1e, 0x1f, 0x1e, 0x1, 0x5, 0x5, 0x7, 0xe, 0x1e, 0x1e, 0x1e, 0x1e, 
        0x5, 0x6, 0x8, 0x14, 0x1e, 0x1e, 0x1e, 0x1e, 0x7, 0x8, 0x11, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0xe, 0x14, 0x1e, 
        0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 
        0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 
        0x1e, 0xff, 0xc0, 0x0, 0x11, 0x8, 0x0, 0x40, 0x0, 0x40, 0x3, 0x1, 0x11, 0x0, 0x2, 0x11, 0x1, 0x3, 0x11, 0x1, 
        0xff, 0xc4, 0x1, 0xa2, 0x0, 0x0, 0x1, 0x5, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0x1, 0x0, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 
        0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0x10, 0x0, 0x2, 
        0x1, 0x3, 0x3, 0x2, 0x4, 0x3, 0x5, 0x5, 0x4, 0x4, 0x0, 0x0, 0x1, 0x7d, 0x1, 0x2, 0x3, 0x0, 0x4, 0x11, 0x5, 0x12, 
        0x21, 0x31, 0x41, 0x6, 0x13, 0x51, 0x61, 0x7, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x8, 0x23, 0x42, 0xb1, 
        0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x9, 0xa, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 
        0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 
        0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 
        0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 
        0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 
        0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 
        0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 
        0xf9, 0xfa, 0x11, 0x0, 0x2, 0x1, 0x2, 0x4, 0x4, 0x3, 0x4, 0x7, 0x5, 0x4, 0x4, 0x0, 0x1, 0x2, 0x77, 0x0, 0x1, 0x2, 
        0x3, 0x11, 0x4, 0x5, 0x21, 0x31, 0x6, 0x12, 0x41, 0x51, 0x7, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x8, 0x14, 0x42, 
        0x91, 0xa1, 0xb1, 0xc1, 0x9, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0xa, 0x16, 0x24, 0x34, 0xe1, 0x25, 
        0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 
        0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 
        0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 
        0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 
        0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4
    };

    for (int i=0; i < 587; i++)
        if (content[i] != expected_result[i])
            return (Test_result){.passed=false, .comment="FAIL header_col : Header non conforme"};
    return (Test_result){.passed=true, .comment="PASSED header_col"};
}


// Test de l'écriture du marqueur de fin d'image
Test_result write_eoi_test () {
    FILE* file = fopen("output_test", "wb+");
    write_JPEG_eoi(file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (content[0] == 0xff && content[1] == 0xd9)
        return (Test_result){.passed=true, .comment="PASSED write_eoi_test"};
    return (Test_result){.passed=false, .comment="FAIL write_eoi_test : Marqueur de fin d'image non présent"};
}



int main () {
    Test_result test_results[NB_TESTS];
    
    test_results[0] = open_file_test();
    test_results[1] = convert_ppm_path_test();
    test_results[2] = convert_pgm_path_test();
    test_results[3] = reverse_bytes_test();
    test_results[4] = write_file_test();
    test_results[5] = header_gray_test();
    test_results[6] = header_col_test();
    test_results[7] = write_eoi_test();

    printf("\n");
    for (int i=0; i < NB_TESTS; i++) {
        if (test_results[i].passed) 
            printf(fTITLE "%s\n" fRESET, test_results[i].comment);
        else
            printf(fWARN "\n%s\n\n" fRESET, test_results[i].comment);
    }
    printf("\n");

    return 0;
}