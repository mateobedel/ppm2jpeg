#include "../include/buffer.h"


Buffer* create_buffer () {
    Buffer* buffer = malloc(sizeof(Buffer));
    buffer->content = 0;
    buffer->nb_bits_inside = 0;
    return buffer;
}


void free_buffer (Buffer* buffer) {
    free(buffer);
}


uint16_t build_mask (uint8_t size) {
    return (1 << size) - 1;
}


void flush_buffer (Buffer* buffer, FILE* file) {

    //byte stuffing 
    if (buffer->content == 0xffff) {
        buffer->content = 0xff00;
        write_file(&buffer->content, file);
        write_file(&buffer->content, file);

        buffer->content = 0;
        buffer->nb_bits_inside = 0;

    } else if (((buffer->content & 0xff00) | 0x00) == 0xff00) {
        uint8_t save_for_next_buf = buffer->content & 0x00ff;
        buffer->content = 0xff00;
        write_file(&(buffer->content), file);

        buffer->content = save_for_next_buf;
        buffer->nb_bits_inside = 8;

    } else if ((0x00 | (buffer->content & 0x00ff)) == 0x00ff) {
        write_file(&(buffer->content), file);

        buffer->content = 0;
        buffer->nb_bits_inside = 8;

    } else {
        write_file(&(buffer->content), file);

        buffer->content = 0;
        buffer->nb_bits_inside = 0;
    }
}


void add_to_buffer (uint16_t nb_to_add, uint8_t size, Buffer* buffer, FILE* file) {

    // assez de place restante dans le buffer pour y ajouter les bits de nb_to_add
    if (BUFFER_SIZE - buffer->nb_bits_inside >= size) {
        buffer->nb_bits_inside += size;
        buffer->content = (buffer->content << size) | (nb_to_add & build_mask(size));
        if (buffer->nb_bits_inside == BUFFER_SIZE)
            flush_buffer(buffer, file);

    } else {
        uint8_t space_left_in_buf = BUFFER_SIZE - buffer->nb_bits_inside;  
        uint8_t nb_bits_for_next_buf = size - space_left_in_buf; 

        uint16_t save_for_next_buf = nb_to_add & build_mask(nb_bits_for_next_buf);
        uint16_t add_to_current_buf = nb_to_add >> (nb_bits_for_next_buf);

        buffer->content = (buffer->content << space_left_in_buf) | (add_to_current_buf & build_mask(space_left_in_buf)); 
        flush_buffer(buffer, file);

        // Si le buffer a été à moitié rempli par le byte stuffing et que ce qui restait à écrire dedans est d'une taille supérieur à 8
        if ((buffer->nb_bits_inside == 8) && (nb_bits_for_next_buf > 8)) {
            add_to_buffer(save_for_next_buf, size - space_left_in_buf, buffer, file);

        } else {
            buffer->content = (buffer->content << nb_bits_for_next_buf) | (save_for_next_buf & build_mask(nb_bits_for_next_buf));
            buffer->nb_bits_inside += nb_bits_for_next_buf;
        }
    }
}


void end_buffer(Buffer* buffer, FILE* file) {

    if (buffer->nb_bits_inside != 0) {
        // si le buffer n'est pas rempli, on décale tout vers les poids fort et on rempli les poids faibles de 0.
        buffer->content = buffer->content << (BUFFER_SIZE - buffer->nb_bits_inside);

        write_file(&(buffer->content), file);
    }      
    free_buffer(buffer);
}

