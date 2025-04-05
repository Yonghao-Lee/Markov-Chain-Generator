#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>    // For printf(), sscanf()
#include <stdlib.h>   // For exit(), malloc()
#include <stdbool.h>  // for bool

// Don't change the macros!
#define ALLOCATION_ERROR_MESSAGE "Allocation failure: Failed to allocate" \
                                 "new memory\n"

// Typedefs for function pointers:
typedef void   (*print_func)(const void *data);
typedef int    (*comp_func)(const void *data1, const void *data2);
typedef void   (*free_data_func)(void *data);
typedef void*  (*copy_func)(const void *data);
typedef bool   (*is_last_func)(const void *data);

/***************************/
/*        STRUCTS          */
/***************************/

typedef struct MarkovNodeFrequency {
    struct MarkovNode *markov_node;
    int frequency;
} MarkovNodeFrequency;

typedef struct MarkovNode {
    void *data;
    MarkovNodeFrequency *frequency_list;
    size_t freq_size;      // How many valid entries are in frequency_list
    size_t freq_capacity;  // How many entries were allocated
} MarkovNode;

typedef struct MarkovChain {
    LinkedList *database;

    // Function pointers
    print_func     print_func;
    comp_func      comp_func;
    free_data_func free_data;
    copy_func      copy_func;
    is_last_func   is_last;
} MarkovChain;

/***************************/
/*   Function Declarations */
/***************************/

/**
 * If data_ptr is in markov_chain->database, return the Node that wraps it.
 * Otherwise, return NULL.
 */
Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * If data_ptr is in markov_chain->database, return it. Otherwise create a new
 * node in the database and return the new node.
 */
Node *add_to_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * Add second_node to the freq list of the first_node, updating frequency
 * or allocating more space if needed.
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node);

/**
 * Free markov_chain and all of its contents from memory.
 */
void free_database(MarkovChain **chain_ptr);

/**
 * Return a random node from markov_chain that is NOT a terminal state.
 */
MarkovNode *get_first_random_node(MarkovChain* markov_chain);

/**
 * Return a weighted-random next node from cur_markov_node->frequency_list.
 */
MarkovNode *get_next_random_node(MarkovNode *cur_markov_node);

/**
 * Generate and print a random chain (like “Random Walk”).
 */
void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode *first_node, int max_length);

int add_node_to_freqlist_helper(MarkovChain *markov_chain, Node *firstnode);
void reset_sequence_printing(void);

#endif /* _MARKOV_CHAIN_H */
