#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define NUMERO_INVALIDO 1
#define NUMERO_ARG_ERRADO 2
#define ERRO_ABRIR_SOCKET 3
#define ERRO_BIND_SOCKET 4
#define ERRO_ACEITAR_SOCKET 5
#define ERRO_CONECTAR_SOCKET 6
#define ERRO_RECEBER_SOCKET 7
#define ERRO_ENVIAR_SOCKET 8

// Macros
#define SOCKET_PORT 15000
#define TAMANHO_MAXIMO_MSG 30

// Função para gerar números inteiros aleatorios crescentes
int atualizeN(int N)
{
  if (N == 0) // caso base
    return 1;

  return (N + (rand() % 100 + 1)); // (No + delta) tal que delta varia entre 1 a 100
}

// Mensagem da interface caso ocorra algum problema
void msgInterface()
{
  cout << "Forma de uso: ./produtor <qtd_numeros> <ip>" << endl;
  cout << "Argumentos:" << endl;
  cout << " - qtd_numeros: quantidade de numeros a serem gerados (1 <= qtd_numeros)" << endl;
  cout << " - ip (opcional): IP da máquina onde está o consumidor (caso não informado, vai utilizar 127.0.0.1)" << endl << endl;
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

// Função principal
int main(int argc, char **argv)
{
  // Numero de parametros deve ser igual a 2 ou a 3, caso contrário a condição é satisfeita e o numero está incorreto
  if ((argc != 2) && (argc != 3))
  {
    cerr << "Numero de argumentos incorreto" << endl;
    msgInterface();
    return NUMERO_ARG_ERRADO;
  }

  // Inicializando uma seed
  srand(time(NULL));

  char *ip;
  
  // Tratamento dos argumentos. Converte string pra int
  int qtd_numeros = atoi(argv[1]); // qtd_numeros
  if (qtd_numeros <= 0){
    cerr << "Numero fornecido invalido" << endl;
    msgInterface();
    return NUMERO_INVALIDO;
  }

  if (argc > 2)
    ip = argv[2]; // ip dado pelo usuario
  else
    ip = (char *)"127.0.0.1"; // ip padrao

  // Criar socket

  // SOCK_STREAM se refere a TCP
  int sock = socket(PF_INET, SOCK_STREAM, 0); // abrir socket
  if (sock == -1)
  {
    cerr << "Erro ao abrir o socket." << endl;
    return ERRO_ABRIR_SOCKET;
  }

  // Conectar ao consumidor
  sockaddr_in address;
  int addressSize = sizeof(sockaddr_in);
  address.sin_family = AF_INET; // IPv4
  address.sin_port = htons(SOCKET_PORT); // host to network short
  inet_pton(AF_INET, (const char *)ip, &address.sin_addr); // convert IPv4 and IPv6 addresses from text to binary form

  int conexaoStatus = connect(sock, (const sockaddr *)&address, (socklen_t)addressSize); // conectar ao consumidor
  if (conexaoStatus != 0){
    cerr << "Erro ao se conectar com o consumidor" << endl;
    return ERRO_CONECTAR_SOCKET;
  }

  cout << "Conectado ao consumidor com sucesso" << endl;

  ssize_t socketStatus;
  char msg[TAMANHO_MAXIMO_MSG];
  bool result; // 0 se nao e primo, 1 se e primo
  int num = 0;

  for (unsigned int counter = 1; counter <= qtd_numeros; counter++)
  {
    num = atualizeN(num);
    itoa(num, msg, 10);

    socketStatus = send(sock, &msg, sizeof(char[TAMANHO_MAXIMO_MSG]), 0); // Enviar ao consumidor
    if (socketStatus == -1){
      cerr << "Erro ao enviar mensagem ao consumidor" << endl;
      return ERRO_ENVIAR_SOCKET;
    }

    socketStatus = recv(sock, &result, sizeof(bool), 0); // Receba resultado do consumidor
		if (socketStatus == -1)
    {
      cerr << "Erro ao tentar ler dados enviados pelo consumidor" << endl;
      return ERRO_RECEBER_SOCKET;
    }

    // Imprime os resultados
    if (result == 0){
        cout <<"El " << counter << ": " << num << " nao é primo (False)" << endl;
    }else{
        cout <<"El " << counter << ": " << num << " é primo (True)" << endl;
    }
  }

  // Caso numero igual a 0, entrou no criterio de parada
  num = 0;
  itoa(num, msg, 10);
  send(sock, &msg, sizeof(char[TAMANHO_MAXIMO_MSG]), 0); // enviar mensagem para parar o programa
  close(sock); // feche o socket

  cout << "Conexao com o consumidor encerrada." << endl;

  return 0;
}
