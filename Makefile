# Makfile for the Game Engine
CC=clang
GL=-lglut -lGLU -lGL
LIBS=-lm

engine: engine.c
	$(CC) -o engine $^ $(GL) $(LIBS)

map_loader.o: map_loader.c
	$(CC) -c $^ 

clean:
	rm -f engine map_loader map_loader.o