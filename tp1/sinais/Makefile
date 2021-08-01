all:
	g++ -g -I. -c -o emitaSinal.o emitaSinal.cpp
	g++ emitaSinal.o -o emitaSinal
	g++ -g -I. -c -o recebaSinal.o recebaSinal.cpp
	g++ recebaSinal.o -o recebaSinal
clean:
	rm -f emitaSinal
	rm -f recebaSinal
	rm -f *.o
