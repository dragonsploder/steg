CC=gcc
IN=main.c
NAME=steg

all:
	$(CC) $(IN) -O3 -lm -o $(NAME)
