
pylib: engine.c pychess.c AIfunc.c
	gcc -g -I/usr/include/python3.12/ -lpython3.12  -shared -fPIC pychess.c engine.c AIfunc.c -o pychess.so
	
engine.o: engine.c
	gcc -fPIC -c engine.c

test: chesstest.c engine.c AIfunc.c
	gcc -lm -Wall -I/usr/include/python3.11/ -lpython3.11 -g AIfunc.c engine.c chesstest.c -o test.bin 
	
