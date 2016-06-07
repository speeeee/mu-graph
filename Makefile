all:
	gcc -g -o mu-graph mu-graph.c -framework OpenGL -lglfw3 -framework Cocoa -framework CoreVideo -framework IOKit