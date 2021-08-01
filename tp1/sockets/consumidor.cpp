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

// Mensagem da interface 
void msgInterface()
{
  cout << "Forma de uso: ./consumidor" << endl;
  cout << "Nao e preciso ter nenhum argumento" << endl << endl;
  cout << "Digite 0 para encerrar o programa" << endl;
}


int main(int argc, char **argv)
{
  
  // SOCK_STREAM se refere a TCP
  int sock = socket(PF_INET, SOCK_STREAM, 0); // criar socket

  if (sock == -1)
  {
    cerr << "Erro ao abrir o socket." << endl;
    return ERRO_ABRIR_SOCKET;
  }

  // Abrindo porta para aguardar conexões
  // Servidor 
  sockaddr_in address;
  int addressSize = sizeof(sockaddr);
  address.sin_family = AF_INET;          // IPv4
  address.sin_addr.s_addr = INADDR_ANY;  // Todas as interfaces de rede
  address.sin_port = htons(SOCKET_PORT); // Porta
  int bindStatus = 0;
  bindStatus = bind(sock, (const sockaddr *)&address, (socklen_t)addressSize); // associates a local address with a socket


  if (bindStatus != 0)
  {
    cerr << "Erro ao realizar bind na porta " << SOCKET_PORT << endl;
    return ERRO_BIND_SOCKET;
  }

  // Aguarda dados do produtor
  cout << "Consumidor: aguardando produtor enviar dados..." << endl;
  listen(sock, 0);

  // Realiza a conexão com o produtor
  int connectionFileDescriptor = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addressSize);
  if (connectionFileDescriptor == -1)
  {
    cerr << "Erro ao abrir conexao com o produtor" << endl;
    return ERRO_ACEITAR_SOCKET;
  }
  cout << "Conexao com produtor realizada com sucesso" << endl;

  // Loop principal
  ssize_t socketStatus;
  char msg[TAMANHO_MAXIMO_MSG];
  int val;
  bool result;
  while (true)
  {
    // Adquire dados do produtor
    socketStatus = recv(connectionFileDescriptor, &msg, sizeof(char[TAMANHO_MAXIMO_MSG]), 0);

    if (socketStatus == -1)
    {
      cerr << "Erro ao tentar ler dados enviados pelo produtor" << endl;
      return ERRO_RECEBER_SOCKET;
    }
		
    val = atoi(msg);
    
    // Verifica condição de parada
    if (val == 0){
      break;
    }
    // Verifica se val é primo
    result = verificaPrimo(val);

    // Envia o resultado para o produtor
    socketStatus = send(connectionFileDescriptor, &result, sizeof(bool), 0);

    if (socketStatus == -1)
    {
      cerr << "Erro ao tentar enviar dados para o produtor" << endl;
      return ERRO_ENVIAR_SOCKET;
    }

  }

  // Quando o numero recebido for igual a 0, o programa para e o socket é finalizado
  close(sock);
  cout << "Conexao com o produtor encerrada" << endl;

  return 0;
}
