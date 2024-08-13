#include "../test/test.h"
#include "../include/encode.h"

#define NB_TESTS 6

// Test la fonction de ré-organisation et quantification d'une composante
Test_result zz_et_quantif_test () {
    Freq_component* component = malloc(sizeof(Freq_component));
    component->pixel_value = (int16_t**)malloc(8 * sizeof(int16_t*));
    for (int i = 0; i < 8; i++) {
        component->pixel_value[i] = (int16_t*)malloc(8 * sizeof(int16_t));
    }

    // Initialisation des valeurs littérales
    int16_t initial_values[8][8] = {
        {1, 2, 3, 4, 5, 6, 7, 8},
        {9, 10, 11, 12, 13, 14, 15, 16},
        {17, 18, 19, 20, 21, 22, 23, 24},
        {25, 26, 27, 28, 29, 30, 31, 32},
        {33, 34, 35, 36, 37, 38, 39, 40},
        {41, 42, 43, 44, 45, 46, 47, 48},
        {49, 50, 51, 52, 53, 54, 55, 56},
        {57, 58, 59, 60, 61, 62, 63, 64}
    };
    
    // Copier les valeurs littérales dans la matrice pixel_value
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            component->pixel_value[i][j] = initial_values[i][j];
        }
    }

    int16_t* result_zz = zig_zag_et_quantification(component, true);
    int16_t expected_result[] = {
        0, 0, 3, 3, 1, 0, 0, 0, 
        4, 6, 8, 4, 2, 0, 0, 0,
        3, 5, 5, 4, 3, 2, 2, 2,
        7, 4, 2, 0, 0, 0, 0, 1,
        7, 6, 4, 3, 2, 1, 1, 1,
        4, 2, 0, 1, 1, 1, 1, 1,
        3, 3, 2, 1, 1, 0, 1, 1,
        2, 2, 2, 1, 1, 1, 2, 2
    };

    for (int i = 0; i < 64; i++) 
        if (result_zz[i] != expected_result[i])
            return (Test_result){.passed=false, .comment="FAIL zz_et_quantif_test : valeurs non conformes à celles attendues."};
    return (Test_result){.passed=true, .comment="PASSED zz_et_quantif_test"};
}


// Test du calcul de la magnitude pour une valeurs non spécifique
Test_result get_magnitude_test () {
    Magnitude mag = get_magnitude(-5);
    if (mag.class != 3)
        return (Test_result){.passed=false, .comment="FAIL get_magnitude_test : Classe de magnitude incrorecte"};
    else if (mag.index != 2)
        return (Test_result){.passed=false, .comment="FAIL get_magnitude_test : Index dans la classe de magnitude incrorecte"};
    return (Test_result){.passed=true, .comment="PASSED get_magnitude_test"};
}


// Test du calcul de la magnitude pour une valeurs aux bornes d'une classe de magnitude
Test_result get_magnitude_test_2 () {
    Magnitude mag = get_magnitude(-2047);
    if (mag.class != 11)
        return (Test_result){.passed=false, .comment="FAIL get_magnitude_test_2 : Classe de magnitude incrorecte"};
    else if (mag.index != 0)
        return (Test_result){.passed=false, .comment="FAIL get_magnitude_test_2 : Index dans la classe de magnitude incrorecte"};
    return (Test_result){.passed=true, .comment="PASSED get_magnitude_test"};
}


// Test de l'encodage d'un coefficient DC sans qu'il n'y ai eu de coefficient avant
Test_result encode_DC_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");

    int16_t new_DC = encode_DC(0, 3, true, buffer, file);

    if (new_DC == 3 && buffer->content == 15 && buffer->nb_bits_inside == 5)
        return (Test_result){.passed=true, .comment="PASSED encode_DC_test"};
    return (Test_result){.passed=false, .comment="FAIL encode_DC_test : La valeur de DC n'est pas encodée correctement"};
}


// Test de l'encodage d'un coefficient DC lorsqu'il y a déjà eu un coefficient DC encodé
Test_result encode_DC_test_2 () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");

    int16_t new_DC = encode_DC(3, 8, true, buffer, file);

    if (new_DC == 8 && buffer->content == 37 && buffer->nb_bits_inside == 6)
        return (Test_result){.passed=true, .comment="PASSED encode_DC_test_2"};
    return (Test_result){.passed=false, .comment="FAIL encode_DC_test_2 : La valeur de DC n'est pas encodée correctement"};
}


// Test de l'encodage du tableau AC
Test_result encode_AC_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");

    int16_t arr_to_encode[] = {
        0, 0, 1, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 5, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
    uint16_t expected_result[] = {0xcff7, 0xbfe5, 0xaff9, 0};

    encode_AC(arr_to_encode, true, buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[8];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 8, file);
    if (nb_unit_read != 6)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    for (int i = 0; i < 7; i+=2) 
        if (((content[i] << 8) | content[i+1]) != expected_result[i/2])
            return (Test_result){.passed=false, .comment="FAIL encode_AC_test : Les valeurs obtenues ne sont paas les bonnes."};
    
    return (Test_result){.passed=true, .comment="PASSED encde_AC_test"};
}


int main () {
    Test_result test_results[NB_TESTS];
    
    test_results[0] = zz_et_quantif_test();
    test_results[1] = get_magnitude_test();
    test_results[2] = get_magnitude_test_2();
    test_results[3] = encode_DC_test();
    test_results[4] = encode_DC_test_2();
    test_results[5] = encode_AC_test();

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
