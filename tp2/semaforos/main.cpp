#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <math.h>
using namespace std;

// Método que imprime a forma de uso do script
void msgInterface()
{
  cout << "Forma de uso: ./main < Np > < Nc > < N > < M >" << endl;
  cout << "Argumentos:" << endl;
  cout << " - Np: número de produtores (ex: 5)" << endl;
  cout << " - Nc: número de consumidores (ex: 5)" << endl;
  cout << " - N: tamanho máximo da memória compartilhada (ex: 10)" << endl;
  cout << " - M (opcional): quantidade de números que devem ser gerados (padão: 10E5)" << endl;
}

// Função para gerar números aleatórios entre 1 e 10^7
int gereNum(){
  return rand() % (int)10e7 + 1;
}

// Função para verificar se um número é primo
bool verificaPrimo(int n)
{
  if (n == 1) return false; // numero 1 nao é primo

  for (unsigned int i = 2; i < sqrt(n); i++)
    if ((n % i) == 0)
      return false;
  return true;
}

// Implementação de semáforos retirada do RIPTutorial da C++ Pedia.
// Link: https://riptutorial.com/cplusplus/example/30142/semaphore-cplusplus-11
class Semaphore
{
public:
  Semaphore(int count_ = 0)
      : count(count_)
  {
  }

  inline void notify()
  {
    unique_lock<mutex> lock(mtx);
    count++;
    cv.notify_one();
  }
  inline void wait()
  {
    unique_lock<mutex> lock(mtx);
    while (count == 0)
      cv.wait(lock);
    count--;
  }

private:
  mutex mtx;
  condition_variable cv;
  int count;
};


// Implementação de um contador com mutex
class contadorMutex{
public:
  contadorMutex(){
    this->contador = 0;
  }

  //int getCount()
  int recebaContador(){
    mutex.lock();
    int val = this->contador;
    mutex.unlock();
    return val;
  }

  void incrementeContador(){
    mutex.lock();
    this->contador++;
    mutex.unlock();
  }

  void decrementeContador(){
    mutex.lock();
    this->contador--;
    mutex.unlock();
  }

private:
  std::mutex mutex;
  int contador;
};


int main(int argc, char **argv){

  // Tratando argumentos
  if ((argc != 4) && (argc != 5))
  {
    cerr << "Número inválido de argumentos!" << endl;
    msgInterface();
    return 1;
  }

  // Inicializando uma seed
  srand(time(NULL));

  // atoi é para converter string para int
  int M;
  int Np = atoi(argv[1]);
  int Nc = atoi(argv[2]);
  int N = atoi(argv[3]);
  if (argc > 4)
    M = atoi(argv[4]);
  else
    M = (int)10e5;

  contadorMutex contadorProdutor;
  contadorMutex contadorConsumidor;

  vector<int> data;
  vector<thread> produtores;
  vector<thread> consumidores;

  Semaphore mutex(1);
  Semaphore empty(N);
  Semaphore full(0);

  for (unsigned int i = 0; i < Np; i++)
  {
    produtores.push_back(thread([&]() {
      while (contadorConsumidor.recebaContador() < M)
      {
        contadorConsumidor.incrementeContador();
        empty.wait();
        mutex.wait();
        data.push_back(gereNum());
        mutex.notify();
        full.notify();
      }
    }));
  }

  for (unsigned int i = 0; i < Nc; i++)
  {
    consumidores.push_back(thread([&]() {
      while (contadorConsumidor.recebaContador() < M)
      {
        contadorConsumidor.incrementeContador();
        full.wait();
        mutex.wait();
        int num = data.back();
        data.pop_back();
        mutex.notify();
        empty.notify();
        if (verificaPrimo(num)){
          cout << "O número " << num << " é primo" << endl;
        }else{
          cout << "O número " << num << " não é primo" << endl;
        }
      }
    }));
  }

  for (unsigned int i = 0; i < Np; i++)
    produtores[i].join();
  for (unsigned int i = 0; i < Nc; i++)
    consumidores[i].join();
  return 0;
}