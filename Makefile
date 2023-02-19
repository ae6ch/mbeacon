all: mbeacon recv

mbeacon: mbeacon.c
	gcc mbeacon.c -o mbeacon

recv: recv.c
	gcc recv.c -o recv
