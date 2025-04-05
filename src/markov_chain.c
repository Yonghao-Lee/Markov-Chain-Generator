#include "markov_chain.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Returns the node that wraps data_ptr if it exists in database;
 * otherwise returns NULL.
 */
Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr) {
    if (!markov_chain || !markov_chain->database) {
        return NULL;
    }
    LinkedList *database = markov_chain->database;
    Node *current = database->first;

    while (current) {
        MarkovNode *mnode = (MarkovNode *)current->data;
        if (markov_chain->comp_func(mnode->data, data_ptr) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * If data_ptr is in the database, return that node. Otherwise allocate
 * a new MarkovNode for it, append it to the list, and return the newly created node.
 */
Node *add_to_database(MarkovChain *markov_chain, void *data_ptr) {
    if (!markov_chain || !markov_chain->database) {
        return NULL;
    }
    LinkedList* database = markov_chain->database;

    // Check if data_ptr is already in the database
    Node *cur = database->first;
    while (cur) {
        MarkovNode *mnode = (MarkovNode *)cur->data;
        if (markov_chain->comp_func(mnode->data, data_ptr) == 0) {
            return cur; // Found existing node
        }
        cur = cur->next;
    }

    // Not found => create a new Node
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed in add_to_database()\n");
        return NULL;
    }

    // Allocate a MarkovNode
    MarkovNode *mnode = malloc(sizeof(MarkovNode));
    if (!mnode) {
        printf("Memory allocation failed in add_to_database()\n");
        free(new_node);
        return NULL;
    }
    // Initialize MarkovNode
    mnode->data = markov_chain->copy_func(data_ptr);
    if (!mnode->data) {
        printf("Memory allocation failed in add_to_database() for mnode->data\n");
        free(mnode);
        free(new_node);
        return NULL;
    }
    mnode->frequency_list = NULL;
    mnode->freq_size = 0;
    mnode->freq_capacity = 0;

    // Link MarkovNode to Node
    new_node->data = mnode;
    new_node->next = NULL;

    // Append to the linked list
    if (!database->first) {
        database->first = new_node;
        database->last  = new_node;
    } else {
        database->last->next = new_node;
        database->last = new_node;
    }

    return new_node;
}

/**
 * Helper to check if a Node is non-terminal or not.
 */
int add_node_to_freqlist_helper(MarkovChain *markov_chain, Node *firstnode) {
    if (markov_chain->is_last(((MarkovNode*)firstnode->data)->data)) {
        return 0; // Terminal node
    }
    return 1; // Non-terminal node
}

/**
 * Add second_node to the frequency list of first_node.
 * If it already exists, increment frequency; else expand the array if needed.
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node) {
    if (!first_node || !second_node || !first_node->data || !second_node->data) {
        return EXIT_FAILURE;
    }

    // If there's no frequency_list yet, allocate initial capacity:
    if (!first_node->frequency_list) {
        first_node->freq_capacity = 2; // start small
        first_node->freq_size = 0;
        first_node->frequency_list = malloc(first_node->freq_capacity *
                                            sizeof(MarkovNodeFrequency));
        if (!first_node->frequency_list) {
            printf("Memory allocation failed in add_node_to_frequency_list()\n");
            return EXIT_FAILURE;
        }
    }

    // Check if second_node is already in the frequency_list
    for (size_t i = 0; i < first_node->freq_size; i++) {
        if (first_node->frequency_list[i].markov_node == second_node) {
            first_node->frequency_list[i].frequency++;
            return EXIT_SUCCESS;
        }
    }

    // Not found => need to insert
    if (first_node->freq_size == first_node->freq_capacity) {
        size_t new_capacity = first_node->freq_capacity * 2;
        MarkovNodeFrequency *new_list = realloc(
            first_node->frequency_list,
            new_capacity * sizeof(MarkovNodeFrequency)
        );
        if (!new_list) {
            printf("Memory allocation failed in add_node_to_frequency_list()\n");
            return EXIT_FAILURE;
        }
        first_node->frequency_list = new_list;
        first_node->freq_capacity  = new_capacity;
    }

    // Insert at freq_size
    first_node->frequency_list[first_node->freq_size].markov_node = second_node;
    first_node->frequency_list[first_node->freq_size].frequency   = 1;
    first_node->freq_size++;

    return EXIT_SUCCESS;
}

/**
 * Free the entire database (all Nodes, MarkovNodes, frequency lists, etc.)
 */
void free_database(MarkovChain **chain_ptr)
{
    if (!chain_ptr || !(*chain_ptr))
    {
        return; // Nothing to free
    }
    MarkovChain *chain = *chain_ptr;         // the MarkovChain* on the heap
    LinkedList *database = chain->database;  // also on the heap

    if (database != NULL)
    {
        Node *current_node = database->first;
        while (current_node != NULL)
        {
            Node *next_node = current_node->next;

            // Free the MarkovNode
            MarkovNode *markov_node = current_node->data;
            if (markov_node)
            {
                // free the frequency list
                free(markov_node->frequency_list);
                markov_node->frequency_list = NULL;

                // free the user data (Cell*)
                if (markov_node->data)
                {
                    chain->free_data(markov_node->data);
                    markov_node->data = NULL;
                }

                // free the MarkovNode struct
                free(markov_node);
            }

            // free the linked-list node itself
            free(current_node);
            current_node = next_node;
        }

        // free the LinkedList struct
        free(database);
        chain->database = NULL;
    }

    // Finally, free the MarkovChain struct itself
    free(chain);
    *chain_ptr = NULL;
}


/**
 * Return length of database
 */
int get_chain_length(const LinkedList *database) {
    if (!database) {
        return 0;
    }
    int length = 0;
    Node *cur = database->first;
    while (cur) {
        length++;
        cur = cur->next;
    }
    return length;
}

/**
 * Return a random number in [0, max_number)
 */
int get_random_number(int max_number) {
    return rand() % max_number;
}

/**
 * Return a random node (by index) that is NOT a terminal node.
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain) {
    if (!markov_chain || !markov_chain->database ||
        !markov_chain->database->first) {
        return NULL;
    }
    int total_length = get_chain_length(markov_chain->database);
    if (total_length == 0) {
        return NULL;
    }

    Node *current_node = NULL;
    do {
        int random_index = get_random_number(total_length);
        current_node = markov_chain->database->first;
        for (int i = 0; i < random_index; i++) {
            current_node = current_node->next;
        }
    } while (markov_chain->is_last(((MarkovNode*)current_node->data)->data));

    return (MarkovNode*)current_node->data;
}

/**
 * Weighted-random next node from cur_markov_node->frequency_list
 */
MarkovNode* get_next_random_node(MarkovNode* cur_markov_node) {
    if (!cur_markov_node || !cur_markov_node->frequency_list) {
        return NULL;
    }

    // Sum of frequencies
    int total_frequency = 0;
    for (size_t i = 0; i < cur_markov_node->freq_size; i++) {
        total_frequency += cur_markov_node->frequency_list[i].frequency;
    }
    if (total_frequency == 0) {
        return NULL;
    }

    int random_num = get_random_number(total_frequency);
    int cumulative = 0;
    for (size_t i = 0; i < cur_markov_node->freq_size; i++) {
        cumulative += cur_markov_node->frequency_list[i].frequency;
        if (cumulative > random_num) {
            return cur_markov_node->frequency_list[i].markov_node;
        }
    }
    return NULL; // theoretically never happens
}
static const void *prev_data = NULL;

void reset_sequence_printing() {
    prev_data = NULL;
}
/**
 * Generate and print a random sequence from first_node,
 * or until we hit a terminal node, up to max_length steps.
 */
void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode  *first_node,
                              int          max_length)
{
    // Step 1: Reset the static pointer to ensure correct formatting
    reset_sequence_printing();

    if (!first_node || !first_node->frequency_list)
    {
        return;
    }

    MarkovNode *current_node = first_node;
    int step_count = 0;

    while (current_node && step_count < max_length)
    {
        // Print the current node
        markov_chain->print_func(current_node->data);

        // Stop if terminal node
        if (markov_chain->is_last(current_node->data))
        {
            break;
        }

        // Move on to the next node
        MarkovNode *next_node = get_next_random_node(current_node);
        if (!next_node)
        {
            break; // No next node available
        }

        current_node = next_node;
        step_count++;

        // Check if max_length is reached
        if (step_count == max_length)
        {
            printf(" ->");
        }
    }

    // End the sequence
    printf("\n");
}