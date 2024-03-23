CC=gcc
CFLAGS= -O3 -I.

Raytracer: ray.o
	$(CC) -o Raytracer ray.o
