#ifndef _CODE_H_
#define _CODE_H_

#include <stdint.h>

/**
 * @brief Structure des codes des symboles de huffman
 *
 * @param code represente le code du caractère
 * @param len longueur en bit du code
 */
typedef struct Code {
    uint16_t code;
    uint8_t len;
} Code;

#endif
