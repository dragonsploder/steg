CC=gcc
IN=main.c
NAME=steg

all:
	$(CC) $(IN) -g -lm -o $(NAME)
