
pylib: engine.c pychess.c
	gcc -I/usr/include/python3.9/ -lpython3.9  -shared -fPIC pychess.c engine.c -o pychess.so
	
engine.o: engine.c
	gcc -fPIC -c engine.c