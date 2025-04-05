# Generic Markov Chain Generator

This project implements a generic Markov chain generator in C that can be used to create random sequences based on transition probabilities learned from input data.

## Features

- **Generic implementation**: Can work with any data type, not just strings
- **Two example applications**:
  - Tweet generator: Creates random tweets based on an input corpus
  - Snakes and Ladders simulator: Simulates random game paths for a Snakes and Ladders board

## Project Structure

- `markov_chain.h/c`: The generic Markov chain implementation
- `tweets_generator.c`: Application that generates random tweets
- `snakes_and_ladders.c`: Application that simulates random Snakes and Ladders games
- `linked_list.h/c`: Implementation of linked list used by the Markov chain

## How It Works

### Core Markov Chain

The project provides a generic implementation of Markov chains that can work with any data type. The implementation:

1. Builds a database (stored as a linked list) of unique items
2. For each item, maintains a frequency list of items that can follow it
3. Can generate random sequences by starting with a random item and then selecting subsequent items based on the learned probabilities

### Tweet Generator

The Tweet Generator:
1. Reads words from an input corpus file
2. Builds a Markov chain where each node represents a unique word
3. For each word, records the frequencies of words that follow it in the corpus
4. Generates random tweets by starting with a non-terminal word and following the chain

### Snakes and Ladders Simulator

The Snakes and Ladders simulator:
1. Creates a game board with cells 1-100, including ladders and snakes
2. Builds a Markov chain where each node represents a cell on the board
3. Transitions between cells follow game rules:
   - If a cell has a ladder or snake, move to the designated cell (100% probability)
   - Otherwise, simulate a dice roll (equal probability to move 1-6 cells forward)
4. Generates random game paths by starting at cell 1 and following the chain

## Usage

### Building the Project

```bash
make all    # Builds both applications
```

Or build them individually:

```bash
make tweets_generator       # Builds only the tweet generator
make snakes_and_ladders     # Builds only the snakes and ladders simulator
```

### Tweet Generator

```bash
./tweets_generator <seed> <num_tweets> <corpus_file> [num_words_to_read]
```

- `seed`: Random seed for reproducible results
- `num_tweets`: Number of tweets to generate
- `corpus_file`: Path to the input corpus file
- `num_words_to_read` (optional): Maximum number of words to read from the corpus

Example:
```bash
./tweets_generator 42 5 data/justdoit_tweets.txt 1000
```

### Snakes and Ladders Simulator

```bash
./snakes_and_ladders <seed> <num_paths>
```

- `seed`: Random seed for reproducible results
- `num_paths`: Number of game paths to simulate

Example:
```bash
./snakes_and_ladders 42 3
```

## Generic Programming Approach

This project demonstrates generic programming in C through:

1. **Void pointers**: Using `void*` to store any data type
2. **Function pointers**: Requiring type-specific functions for:
   - Printing elements
   - Comparing elements
   - Copying elements
   - Freeing elements
   - Determining termination conditions

This approach allows the Markov chain implementation to remain completely agnostic about the data it's working with, while still providing full functionality.

## Examples

### Tweet Generator Output

```
Tweet 1: Just do it because you can make it happen.
Tweet 2: Don't wait for the perfect moment take the moment and make it perfect.
Tweet 3: Yesterday you said tomorrow so just do it.
```

### Snakes and Ladders Output

```
Random Walk 1: [1] -> [2] -> [3] -> [7] -> [13] -snake to-> [4] -> [5] -> [6] -> [10] -> [12] -> [16] -> [21] -> [25] -> [29] -> [35] -snake to-> [11] -> [15] -ladder to-> [47] -> [51] -> [55] -> [58] -> [61] -snake to-> [14] -> [20] -ladder to-> [39] -> [41] -ladder to-> [62] -> [65] -> [69] -snake to-> [32] -> [33] -> [35] -snake to-> [11] -> [16] -> [18] -> [23] -ladder to-> [76] -> [82] -> [84] -> [87] -snake to-> [31] -> [32] -> [38] -> [39] -> [42] -> [47] -> [53] -> [54] -> [55] -> [56] -> [58] -> [61] -snake to-> [14] -> [16] -> [17] -> [18]
```

## Extending the Project

To use the generic Markov chain with your own data type:

1. Create functions to:
   - Print your data
   - Compare two instances of your data
   - Make a deep copy of your data
   - Free memory allocated for your data
   - Determine when a sequence should terminate
2. Initialize a MarkovChain with these functions
3. Populate the database with your data
4. Generate random sequences
