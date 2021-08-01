all:
	g++ -g -I. -c -o consumidor.o consumidor.cpp
	g++ consumidor.o -o consumidor
	g++ -g -I. -c -o produtor.o produtor.cpp
	g++ produtor.o -o produtor
clean:
	rm -f consumidor
	rm -f produtor
	rm -f *.o