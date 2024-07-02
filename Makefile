main: main.c
	gcc -I/opt/homebrew/Cellar/raylib/5.0/include -o main main.c -lraylib -L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib

run: main
	./main
