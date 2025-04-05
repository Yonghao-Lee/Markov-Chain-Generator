#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"
#include <errno.h>

#define BASE_10 10
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY (-1)
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {
    {13, 4},
    {85, 17},
    {95, 67},
    {97, 58},
    {66, 89},
    {87, 31},
    {57, 83},
    {91, 25},
    {28, 50},
    {35, 11},
    {8, 30},
    {41, 62},
    {81, 43},
    {69, 32},
    {20, 39},
    {33, 70},
    {79, 99},
    {23, 76},
    {15, 47},
    {61, 14}
};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to; // cell which ladder leads to, if there is one
    int snake_to; // cell which snake leads to, if there is one
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;
// A static/global variable that only this .c file sees.
static const Cell *g_prev_cell = NULL;

/**
 * Reset the "previous cell" pointer before printing a new random sequence.
 */

int compare_cells(const void *data1, const void *data2) {
    if (!data1 || !data2) {
        return -1;
    }
    const Cell *c1 = (const Cell*)data1;
    const Cell *c2 = (const Cell*)data2;
    return (c1->number - c2->number);
}

void print_cell(const void *data) {
    const Cell *new_cell = (const Cell *)data;

    if (g_prev_cell == NULL) {
        // First cell
        printf("[%d]", new_cell->number);
    } else {
        if (g_prev_cell->ladder_to == new_cell->number) {
            printf(" -ladder to-> [%d]", new_cell->number);
        } else if (g_prev_cell->snake_to == new_cell->number) {
            printf(" -snake to-> [%d]", new_cell->number);
        } else {
            // Normal step
            printf(" -> [%d]", new_cell->number);
        }
    }

    // Update the global pointer for the next cell
    g_prev_cell = new_cell;
}


void *copy_cell(const void *data) {
    if (!data) return NULL;
    const Cell *orig = (const Cell*)data;
    Cell *copy = malloc(sizeof(Cell));
    if (!copy) {
        printf(ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    *copy = *orig;
    return copy;
}

void free_cell(void *data) {
    free(data);
}

bool is_terminal_cell(const void *data) {
    if (!data) return false;
    const Cell *cell = (const Cell*)data;
    return (cell->number == BOARD_SIZE);
}



/**
 * allocates memory for cells on the board and initalizes them
 * @param cells Array of pointer to Cell, represents game board
 * @return EXIT_SUCCESS if successful, else EXIT_FAILURE
 */
int create_board(Cell* cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(cells[j]);
            }
            printf(ALLOCATION_ERROR_MESSAGE);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell){i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

int add_cells_to_database(MarkovChain* markov_chain, Cell* cells[BOARD_SIZE])
{
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        Node* tmp = add_to_database(markov_chain, cells[i]);
        if (tmp == NULL)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int set_nodes_frequencies(MarkovChain* markov_chain, Cell* cells[BOARD_SIZE])
{
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain, cells[i])->data;
        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain,
                                             cells[index_to])->data;
            int res = add_node_to_frequency_list(from_node, to_node);
            if (res == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*)(from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain,
                                                 cells[index_to])->data;
                int res = add_node_to_frequency_list(from_node, to_node);
                if (res == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int fill_database_snakes(MarkovChain* markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (add_cells_to_database(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    if (set_nodes_frequencies(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

bool err_parsing_msg(const char *endptr) {
    if (errno == ERANGE) {
        printf("Error: Value out of range.\n");
        return false;
    } else if (*endptr != '\0') {
        printf("Error: Invalid character '%c' found in input.\n", *endptr);
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    char *endptr;
    errno = 0;
    unsigned int seed = (unsigned int)strtol(argv[1], &endptr, BASE_10);
    if (!err_parsing_msg(endptr)) {
        return EXIT_FAILURE;
    }
    srand(seed);

    errno = 0;
    int num_paths = (int)strtol(argv[2], &endptr, BASE_10);
    if (!err_parsing_msg(endptr)) {
        return EXIT_FAILURE;
    }

    // Create the MarkovChain
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    markov_chain->database = malloc(sizeof(LinkedList));
    if (!markov_chain->database) {
        printf(ALLOCATION_ERROR_MESSAGE);
        return EXIT_FAILURE;
    }
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;

    // Assign function pointers
    markov_chain->print_func  = print_cell;
    markov_chain->comp_func   = compare_cells;
    markov_chain->copy_func   = copy_cell;
    markov_chain->free_data   = free_cell;
    markov_chain->is_last     = is_terminal_cell;

    // Build the database with our board
    if (fill_database_snakes(markov_chain) == EXIT_FAILURE) {
        free(markov_chain->database);
        return EXIT_FAILURE;
    }

    // Generate random paths
    for (int i = 0; i < num_paths; i++) {
        printf("Random Walk %d: ", i + 1);
        MarkovNode *start_node = markov_chain->database->first->data;
        // Optionally, pick a truly random start:
        // MarkovNode *start_node = get_first_random_node(&markov_chain);
        generate_random_sequence(markov_chain, start_node, MAX_GENERATION_LENGTH);
        printf("\n");
    }

    // Clean up
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}