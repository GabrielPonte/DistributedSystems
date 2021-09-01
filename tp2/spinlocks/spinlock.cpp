#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <random>
#include <numeric>
#include <atomic>
#include <thread>
#include <time.h>
#include <iostream>
#include <ctime>
#include <vector>

using namespace std;
#define TAMANHO_VETOR_SOMADOR 10e9

// Método que imprime a forma de uso do script
void msgInterface()
{
  cout << "Forma de uso: ./spinlock numThreads" << endl;
  cout << "Argumentos:" << endl;
  cout << " - numThreads: número de threads (ex: 5)" << endl;
}


atomic_flag lock = ATOMIC_FLAG_INIT;

void sum(vector<signed char>::const_iterator begin, vector<signed char>::const_iterator end, long signed int *soma) {
	long int s = accumulate(begin, end, 0);
	while(lock.test_and_set()) {}
	*soma += s;
	lock.clear();
}

vector<signed char> generateVectorNumbers(){
    vector<signed char> generatedNumbers;
    srand(time(NULL));
	for (int i=0; i<TAMANHO_VETOR_SOMADOR; i++) {
		generatedNumbers.push_back((rand() % 201) - 100);
	}
    
    return generatedNumbers; 

}

int main (int argc, char * argv[]) {
	if (argc != 2) {
		msgInterface();
		return 1;
	}
    
	vector<signed char> numbers= generateVectorNumbers();
	int numThreads = atoi(argv[1]);
	vector<thread> v;
	long signed int soma = 0;
	int piece = (int) TAMANHO_VETOR_SOMADOR/numThreads;

    //iniciando a contagem de tempo 
	auto start = chrono::system_clock::now();

	for (int i=0; i< numThreads; i++) {
		thread thread_obj(sum, numbers.begin() + i*piece, numbers.end() - (numThreads-i-1)*piece, &soma);
		v.push_back((thread&&) (thread_obj));
	}

	for (int i=0; i< numThreads; i++) {
		v[i].join();
	}
	
    //fim da contagem de tempo
   	auto end = chrono::system_clock::now();
    
	chrono::duration<double> elapsed = end-start;

    cout << "Soma Calculada: " << soma << "\n";
    cout << "Quantidade de Threads: " << numThreads << "\n"; 
	cout << "tempo de execução: " << elapsed.count() << "\n";


	return 0;
}
