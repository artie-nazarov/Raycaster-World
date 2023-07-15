# Makfile for the Game Engine
CC=clang
GL=-lglut -lGLU -lGL

engine: engine.c
	clang -o engine $^ $(GL)

clean:
	rm -f engine