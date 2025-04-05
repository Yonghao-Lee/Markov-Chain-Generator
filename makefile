CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGETS = tweets_generator snakes_and_ladders

all: $(TARGETS)

tweets_generator: tweets_generator.c linked_list.c markov_chain.c
	$(CC) $(CFLAGS) -o $@ $^

snakes_and_ladders: snakes_and_ladders.c linked_list.c markov_chain.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)