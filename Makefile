all:
	g++ -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL3

run: all
	./main.exe

clean:
	del main.exe