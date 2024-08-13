#include "../test/test.h"
#include "../include/buffer.h"

#define NB_TESTS 8


// Test de création du buffer
Test_result create_buffer_test () {
    Buffer* buffer = create_buffer();
    if (buffer != NULL && buffer->content == 0 && buffer->nb_bits_inside == 0)
        return (Test_result){.passed = true, .comment = "PASSED create_buffer_test"};
    return (Test_result){.passed = false, .comment = "FAIL create_buffer_test : Pointeur non alloué ou variable mal initialisée."};
}


// Test d'écriture quand le buffer est plein
Test_result write_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");
    if (file == NULL)
        return (Test_result){.passed = false, .comment = "FAIL write_test : Echec à l'ouverture du fichier"};

    add_to_buffer(0x1234, 16, buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);
    
    if (content[0] == 0x12 && content[1] == 0x34)
        return (Test_result){.passed = true, .comment = "PASSED write_test"};
    return (Test_result){.passed = false, .comment = "FAIL write_test : Valeurs lues non conformes à celles attendues."};
}


// Test du décalage des bits lors de l'insertion d'un nombre dans le buffer partiellement rempli, 
// sachant que l'espace disponible dans le buffer est suffisant
Test_result put_to_buffer_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");
    if (file == NULL)
        return (Test_result){.passed = false, .comment = "FAIL put_to_buffer_test : Echec à l'ouverture du fichier"};
    
    add_to_buffer(0x12, 8, buffer, file);
    add_to_buffer(0X34, 8, buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (content[0] == 0x12 && content[1] == 0x34)
        return (Test_result){.passed = true, .comment = "PASSED put_to_buffer_test"};
    return (Test_result){.passed = false, .comment = "FAIL put_to_buffer_test : Valeurs lues non conformes à celles attendues."};;
}


// Test de l'insertion d'un nombre dans le buffer partiellement rempli sachant que l'espace disponible
// n'est pas suffisant
Test_result put_to_buffer_test_2 () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");
    if (file == NULL)
        return (Test_result){.passed = false, .comment = "FAIL put_to_buffer_test_2 : Echec à l'ouverture du fichier"};
    
    add_to_buffer(0x12, 8, buffer, file);
    add_to_buffer(0X3456, 16, buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (content[0] == 0x12 && content[1] == 0x34 && buffer->content == 0x56)
        return (Test_result){.passed = true, .comment = "PASSED put_to_buffer_test_2"};
    return (Test_result){.passed = false, .comment = "FAIL put_to_buffer_test_2 : Valeurs lues non conformes à celles attendues."};;
}



// Test du byte stuffing dans le cas où les deux octets à écrire dans le fichier valent chacun 0xff.
Test_result byte_stuffing_ffff_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");
    if (file == NULL)
        return (Test_result){.passed = false, .comment = "FAIL byte_stuffing_ffff_test : Echec à l'ouverture du fichier"};
    
    add_to_buffer(0xffff, 16, buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[4];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 4, file);
    if (nb_unit_read != 4)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (((content[0] << 8) | content[1]) == 0xff00 && ((content[2] << 8) | content[3]) == 0xff00)
        return (Test_result){.passed = true, .comment = "PASSED byte_stuffing_ffff_test"};
    return (Test_result){.passed = false, .comment = "FAIL byte_stuffing_ffff_test : Valeurs lues non conformes à celles attendues."};;
}


// Test du byte stuffing dans le cas où seul le premier octet des deux à écrire vaut 0xff.
Test_result byte_stuffing_ffxx_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");
    if (file == NULL)
        return (Test_result){.passed = false, .comment = "FAIL byte_stuffing_ffxx_test : Echec à l'ouverture du fichier"};
    
    add_to_buffer(0xff12, 16, buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (((content[0] << 8) | content[1]) == 0xff00 && buffer->content == 0x12)
        return (Test_result){.passed = true, .comment = "PASSED byte_stuffing_ffxx_test"};
    return (Test_result){.passed = false, .comment = "FAIL byte_stuffing_ffxx_test : Valeurs lues non conformes à celles attendues."};;
}


// Test du byte stuffing dans le cas où seul le second octet des deux à écrire vaut 0xff.
Test_result byte_stuffing_xxff_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");
    if (file == NULL)
        return (Test_result){.passed = false, .comment = "FAIL byte_stuffing_xxff_test : Echec à l'ouverture du fichier"};
    
    add_to_buffer(0x12ff, 16, buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (((content[0] << 8) | content[1]) == 0x12ff && buffer->content == 0 && buffer->nb_bits_inside == 8)
        return (Test_result){.passed = true, .comment = "PASSED byte_stuffing_xxff_test"};
    return (Test_result){.passed = false, .comment = "FAIL byte_stuffing_xxff_test : Valeurs lues non conformes à celles attendues."};;
}


// Test de la fermeture du buffer : écriture dans le fichier s'il n'est pas vide puis libération mémoire.
Test_result end_buffer_test () {
    Buffer* buffer = create_buffer();
    FILE* file = fopen("output_test", "wb+");
    if (file == NULL)
        return (Test_result){.passed = false, .comment = "FAIL end_buffer_test : Echec à l'ouverture du fichier"};
    
    add_to_buffer(0x12, 8, buffer, file);
    end_buffer(buffer, file);

    fseek(file, 0, SEEK_SET);
    uint8_t content[2];
    uint8_t nb_unit_read = fread(content, sizeof(uint8_t), 2, file);
    if (nb_unit_read != 2)
        return (Test_result){.passed=false, .comment="Mauvaise lecture du fichier"};
    fclose(file);

    if (((content[0] << 8) | content[1]) == 0x1200 )
        return (Test_result){.passed = true, .comment = "PASSED end_buffer_test"};
    return (Test_result){.passed = false, .comment = "FAIL end_buffer_test : Valeurs lues non conformes à celles attendues."};;
}


int main () {
    Test_result test_results[NB_TESTS];
    
    test_results[0] = create_buffer_test();
    test_results[1] = write_test();
    test_results[2] = put_to_buffer_test();
    test_results[3] = put_to_buffer_test_2();
    test_results[4] = byte_stuffing_ffff_test();
    test_results[5] = byte_stuffing_ffxx_test();
    test_results[6] = byte_stuffing_xxff_test();
    test_results[7] = end_buffer_test();

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