all:
	g++ -I src/include -L src/lib -o wolf wolf.cpp -lmingw32 -lSDL2main -lSDL2