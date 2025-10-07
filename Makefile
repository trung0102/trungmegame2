all:
	g++ -I src/include -L src/lib -o main main.cpp src/bot.cpp src/AI.cpp -lmingw32 -lSDL3 -lSDL3_image
 
run: all
	./main.exe

clean:
	del main.exe