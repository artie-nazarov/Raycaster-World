# Makfile for the Game Engine
CC=clang
GL=-lglut -lGLU -lGL

engine: engine.c
	$(CC) -o engine $^ $(GL) 

map_loader.o: map_loader.c
	$(CC) -c $^ 

clean:
	rm -f engine map_loader map_loader.o