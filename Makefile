# Ryan Harang 04/30/2024

CC=gcc
CFLAGS=-g -Wall
FILES=Simulation.c ListMethods.c PriorityQueue.c Queue.c
DERIV=${FILES:.c=.o}
DEPEND=$(DERIV)

all: Simulation
Simulation: $(DEPEND)
	$(CC) -g -o Simulation $(CFLAGS) $(DERIV)

clean:
	rm -f $(DERIV) Simulation