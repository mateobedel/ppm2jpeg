#include "huffman.h"


Node* init_node(uint8_t symbol) {
    Node* node = malloc(sizeof(Node));
    node->symbol = symbol;
    node->left = NULL;
    node->right = NULL;
    return node;
}


Node* build_huffman(uint8_t symbols[], uint8_t nb_symbols_length[], uint8_t nb_symbol) {

    Node* root = init_node(0);
    uint16_t nb_nodes = 2;
    
    Node** nodes_at_last_level = malloc(sizeof(Node*));
    Node** nodes_at_level = malloc(sizeof(Node*)*nb_nodes);
    nodes_at_last_level[0] = root;
    
    for (uint8_t i = 0; i < NB_LEVEL; i++) { //Pour chaque étage
        
        for (uint8_t j = 0; j < nb_nodes; j++) {// Pour chaque noeud

            //Initialisation de la feuille contenant le symbole ou un noeud
            if (nb_symbols_length[i] - j  > 0) {
                nodes_at_level[j] = init_node(*(symbols++)); //symbole
                nb_symbol--;
            } else {
                nodes_at_level[j] = init_node(0); //noeud interne
            }

            //On relie les étages de noeuds entre-eux
            uint8_t nb_last_symb = (i == 0 ? 0 : nb_symbols_length[i-1]);
            if (j%2 == 0)   
                nodes_at_last_level[j/2 + nb_last_symb]->left = nodes_at_level[j];
            else 
                nodes_at_last_level[j/2 + nb_last_symb]->right = nodes_at_level[j];
                
            //Si tout les symbole ont été ajoutés, on return l'arbre
            if (nb_symbol == 0) {
                free(nodes_at_last_level);  
                free(nodes_at_level);
                return root;
            }
        }

        nb_nodes = (nb_nodes-nb_symbols_length[i])*2;

        free(nodes_at_last_level);
        nodes_at_last_level = nodes_at_level;
        nodes_at_level = malloc(sizeof(Node*)*nb_nodes);
    }

    free(nodes_at_last_level);
    free(nodes_at_level);
    return root;
}


void free_huffman(Node* root) {
    if (root == NULL) return;
    free(root);
}


void print_tree(Node* root, int level) {
    if (root == NULL) return;
    print_tree(root->right, level+1);
    for (uint8_t i = 0; i < level; i++ ) 
        printf("-");
    printf( "%d\n", root->symbol);
    print_tree (root->left, level+1) ;
}


void traverse_huffman(Node* root, Code *table, uint16_t code, uint8_t prof) {

    //Si le noeud est une feuille on rempli le dictionnaire
    if (!(root->left) && !(root->right)) { 
        Code s_code = {.code = code, .len = prof};
        table[root->symbol] = s_code;
    }

    //On ajoute un 0 à code en allant dans le noeud de gauche
    if (root->left)
        traverse_huffman(root->left, table, code<<1, prof+1);

    //On ajoute un 1 à code en allant dans le noeud de droite
    if (root->right)
        traverse_huffman(root->right, table, code<<1|1, prof+1);
}


Code* huffman_table(Node* root, uint16_t len) {
    Code* table = calloc(sizeof(Code),len);
    traverse_huffman(root, table, 0, 0);
    return table;
    
}


void print_huffman_table(Code* table, uint16_t len) {
    printf("{");
    for(uint16_t i=0; i < len; i++) {
        printf("{.code=%d,.len=%d}",table[i].code,table[i].len);
        if (i != len -1) printf(",\n");
    }
    printf("}");
}