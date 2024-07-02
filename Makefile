main: main.c
	gcc -I/opt/homebrew/Cellar/raylib/5.0/include -o main -O3 -g3 main.c -lraylib -L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib -lm

run: main
	./main
