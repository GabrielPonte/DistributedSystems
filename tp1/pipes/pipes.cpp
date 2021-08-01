#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#define NUMERO_ARG_ERRADO 1
#define NUMERO_INVALIDO 2
#define ERRO_PIPE 3
#define ERRO_FORK 4
#define ERRO_ESCREVA_PIPE 5
#define ERRO_LEIA_PIPE 6

#define TAMANHO_MAXIMO_MSG 40


// Função para gerar números inteiros aleatorios crescentes
int atualizeN(int N)
{
  if (N == 0) // caso base
    return 1;

  return (N + (rand() % 100 + 1)); // (No + delta) tal que delta varia entre 1 a 100
}


// Função para verificar se um número é primo
// Baseado em https://www.javatpoint.com/prime-number-program-in-cpp
bool verificaPrimo(int n)
{ 

  if (n == 1) return false; // numero 1 não é primo

  for (unsigned int i = 2; i < n / 2; i++)

    if ((n % i) == 0){
      return false;
    }

  return true;
}


// Mensagem da interface caso ocorra algum problema
void msgInterface()
{
  cout << "Forma de uso: ./pipes <qtd_numeros>" << endl;
  cout << "Argumentos:" << endl;
  cout << " - qtd_numeros: quantidade de numeros a serem gerados (1 <= qtd_numeros)" << endl << endl;
  cout << "Digite 0 para encerrar o programa" << endl;
}

// Função para reverter uma string
// Retirado de https://www.geeksforgeeks.org/implement-itoa/
void reverse(char str[], int length)
{
  int start = 0;
  int end = length - 1;
  while (start < end)
  {
    swap(*(str + start), *(str + end));
    start++;
    end--;
  }
}

// Implementacao do itoa()
// Retirado de https://www.geeksforgeeks.org/implement-itoa/
char * itoa(int num, char *str, int base)
{
  int i = 0;
  bool isNegative = false;

  /* Handle 0 explicitely, otherwise empty string is printed for 0 */
  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  // In standard itoa(), negative numbers are handled only with
  // base 10. Otherwise numbers are considered unsigned.
  if (num < 0 && base == 10)
  {
    isNegative = true;
    num = -num;
  }

  // Process individual digits
  while (num != 0)
  {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }

  // If number is negative, append '-'
  if (isNegative)
    str[i++] = '-';

  str[i] = '\0'; // Append string terminator

  // Reverse the string
  reverse(str, i);

  return str;
}



int main(int argc, char ** argv)
{

  // Numero de parametros deve ser igual a 2, caso contrário a condição é satisfeita e o numero está incorreto
  if (argc != 2)
  { 
    cerr << "Numero de argumentos incorreto" << endl;
    msgInterface();
    return NUMERO_ARG_ERRADO;
  }

  // Inicializando uma seed
  srand(time(NULL));

  // Tratamento dos argumentos, converte string pra int
  int n_numbers = atoi(argv[1]);
  if (n_numbers <= 0){
    cerr << "Numero fornecido invalido" << endl;
    msgInterface();
    return NUMERO_INVALIDO;
  }

  // Inicializando o pipe
  // pipe[1] -> pipe[0], logo:
    // pipe[1] para o produtor
    // pipe[0] para o consumidor
  
  
  int fd[2];
  
  if (pipe(fd) == -1) // Pipe nao conseguiu inicializar
  {
    cerr << "Falha ao iniciar o pipe" << endl;
    return ERRO_PIPE;
  }

  // Criando processo filho
  int pid = fork();
  if (pid == -1) // Fork nao conseguiu ser realizado
  {
    cerr << "Falha ao realizar o fork" << endl;
    return ERRO_FORK;
  }

  // ******************************************
  // Processo filho é o produtor
  // ******************************************

  if (pid == 0) // entra no processo filho
  {

    // Fecha a ponta do consumidor
    close(fd[0]);

    // Produzindo números e escrevendo eles
    char msg[TAMANHO_MAXIMO_MSG];
    int num = 0;

    for (unsigned int i = 0; i < n_numbers; i++)
    {
      num = atualizeN(num);
      itoa(num, msg, 10);
      if (write(fd[1], &msg, sizeof(char[TAMANHO_MAXIMO_MSG])) == -1) 
      {
        cerr << "Erro ao escrever no pipe" << endl;
        return ERRO_ESCREVA_PIPE;
      }
    }
    // Fecha a ponta do produtor
    close(fd[1]);
  }

  //consumidor : 0 -> pipe -> 1 PID: !=0 (Pai)
  //produtor : 0 -> pipe -> 1 PID ==0 (Filho)

  // ******************************************
  // Processo pai é o consumidor
  // ******************************************

  else
  {   
    // Fecha a ponta do produtor
    close(fd[1]);
    
    // Receba numeros e faca a verificacao
    for (unsigned int counter = 1; counter <= n_numbers ;counter++)
    {
      int val;
      char msg[TAMANHO_MAXIMO_MSG];
      if (read(fd[0], &msg, sizeof(char[TAMANHO_MAXIMO_MSG])) == -1){
        cerr << "Erro ao ler do pipe" << endl;
        return ERRO_LEIA_PIPE;
      }

      val = atoi(msg);
      if (verificaPrimo(val) == 0){
        cout <<"El " << counter << ": " << val << " nao é primo (False)" << endl;
      }else{
        cout <<"El " << counter << ": " << val << " é primo (True)" << endl;
      }
    }

    // Fecha a ponta do consumidor
    close(fd[0]);
  }

  return 0;
}