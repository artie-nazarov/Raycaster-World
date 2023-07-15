# Makfile for the Game Engine
CC=clang
GL=-lglut -lGLU -lGL

engine: engine.c
	$(CC) -o engine $^ $(GL)

map_loader: map_loader.c
	$(CC) -o map_loader $^ 

clean:
	rm -f engine map_loader