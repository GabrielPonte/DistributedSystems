#include <cliente.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstring>

using namespace std;

int connect()
{
  // Criando socket
  // SOCK_STREAM refere a TCP
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    cerr << "Erro ao abrir o socket." << endl;
    return ERRO_ABRIR_SOCKET;
  }

  // Conectando ao consumidor
  sockaddr_in address;
  int addressSize = sizeof(sockaddr_in);
  address.sin_family = AF_INET; // IPv4
  address.sin_port = htons(COORDINATOR_PORT);
  inet_pton(AF_INET, (const char *)COORDINATOR_IP, &address.sin_addr);
  int connStatus = connect(sock, (const sockaddr *)&address, (socklen_t)addressSize);
  if (connStatus != 0)
  {
    cerr << "Erro ao tentar se conectar com o coordenador." << endl;
    return ERRO_CONECTAR_SOCKET;
  }
  cout << "PID " << getpid() << " conectado." << endl;
  return sock;
}

void disconnect(int socketFd)
{
  close(socketFd);
}


// Mensagem que base com o estilo x|PID|0000, sendo x um local vazio, porém será realocado pelo identificador da mensagem
void mensagemBase(int id, char msg[TAMANHO_MAXIMO_MSG + 1])
{
  // Transformar o PID em string para transformar em CHAR
  string strId = to_string(id);
  char charId[TAMANHO_MAXIMO_MSG + 1];
  strcpy(charId, strId.c_str());
  msg[1] = '|';
  int i = 0;
  while (charId[i] != '\0') // enquanto não for vazio
  {
    msg[i + 2] = charId[i];
    i++;
  }
  msg[i + 2] = '|';
  for (int j = i + 3; j < TAMANHO_MAXIMO_MSG; j++) // preencher com zeros
    msg[j] = '0';
  msg[TAMANHO_MAXIMO_MSG] = '\0';
}

void request(int socketFd, int id) // enviar pedido de request ao coordenador
{
  char msg[TAMANHO_MAXIMO_MSG + 1];
  mensagemBase(id, msg); // gera mensagem base do pedido com PID
  msg[0] = MESSAGE_REQUEST; // indica que é um pedido
  send(socketFd, &msg, sizeof(char[TAMANHO_MAXIMO_MSG + 1]), 0); // envia pedido
  ssize_t sockStatus = recv(socketFd, &msg, sizeof(char[TAMANHO_MAXIMO_MSG + 1]), 0); // recebe msg
  if (sockStatus == -1) //msg erro
  {
    cerr << "Erro ao tentar ler dados enviados pelo consumidor." << endl;
    exit(ERRO_RECEBER_SOCKET);
  }
  if (msg[0] == MESSAGE_GRANT) // caso a mensagem recebida for "concedido"
    return;
  else
    exit(ERRO_MENSAGEM_DESCONHECIDA); // nao sabemos que mensagem é essa e damos exit
}

void release(int socketFd, int id)
{
  char msg[TAMANHO_MAXIMO_MSG + 1];
  mensagemBase(id, msg); // gera mensagem base do pedido com PID
  msg[0] = MESSAGE_RELEASE; // indica que é um release
  send(socketFd, &msg, sizeof(char[TAMANHO_MAXIMO_MSG + 1]), 0); // envia mensagem para o coordenador que pode dar release
}

// Função para receber tempo em milisegundos, retirado do site: https://gist.github.com/bschlinker/844a88c09dcf7a61f6a8df1e52af7730
string getTimestamp()
{
  const auto now = chrono::system_clock::now();
  const auto nowAsTimeT = chrono::system_clock::to_time_t(now);
  const auto nowMs = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
  stringstream nowSs;
  nowSs << put_time(localtime(&nowAsTimeT), "%T") << '.' << setfill('0') << setw(3) << nowMs.count();
  return nowSs.str();
}

int main(int argc, char **argv)
{
  int r = atoi(argv[1]); // procedimento repetido r vezes
  int k = atoi(argv[2]); // tempo de sleep
  int n = atoi(argv[3]); // numero de threads

  int pids[n];

  for (int i = 0; i < n; i++)
  {
    pids[i] = fork(); // dar fork
    if (pids[i] < 0) // Falhou
    {
      cerr << "Falhou ao dar fork no processo" << endl;
      return ERRO_FORK;
    }
    else if (pids[i] == 0) // processo filho
    {
      int id = getpid(); // receba PID do processo
      int sock = connect(); // se conecte com o coordenador
      string timeStamp;
      ofstream file;
      for (int j = 0; j < r; j++) // r vezes  para cada PID
      {
        request(sock, id); // realizar pedido para rodar o programa abaixo
        // Pedido concedido
        timeStamp = getTimestamp(); // receba o tempo com milisegundos
        file.open("resultado.txt", ofstream::out | ofstream::app); // abra arquivo resultado.txt
        file << timeStamp + "|" << id << endl; // escreva no arquivo o horario do pedido
        file.close(); // feche o arquivo
        sleep(k); // durma por k segundos
        release(sock, id); // manda mensagem de release para o coordenador
      }
      disconnect(sock); // disconecte o socket
      return OK; // thread rodou r vezes com sucesso
    }
  }

  while (n > 0)
  {
    waitpid(-1,NULL,0);
    n -= 1;
  }
  return OK;
}
