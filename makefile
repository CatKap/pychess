
pylib: engine.c pychess.c AIfunc.c
	gcc -g -I/usr/include/python3.10/ -lpython3.10  -shared -fPIC pychess.c engine.c AIfunc.c -o pychess.so
	
engine.o: engine.c
	gcc -fPIC -c engine.c

test: chesstest.c engine.c AIfunc.c
	gcc -lm -Wall -I/usr/include/python3.10/ -lpython3.10 -g AIfunc.c engine.c chesstest.c -o test.bin 
	
