CC=gcc
CFLAGS=-O2 -ggdb
LDFLAGS=-lglut -lGL -lGLU -lm
EXEC=nparticles

all: $(EXEC)

nparticles: main.o
	$(CC) -o $@ $< $(LDFLAGS)

main.o: main.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm *.o

mrproper: clean
	rm $(EXEC)

