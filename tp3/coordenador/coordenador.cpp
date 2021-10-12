#include <coordenador.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <cstring>

using namespace std;

struct message
{
  int type;
  int id;
  int fd;
};

enum command
{
  comando_print_fila,
  comando_print_historico,
  comando_quit,
  comando_invalido,
};

command traduzaComando(string inputCommand)
{
  if (inputCommand == "1")
    return comando_print_fila;
  else if (inputCommand == "2")
    return comando_print_historico;
  else if (inputCommand == "3")
    return comando_quit;
  else
    return comando_invalido;
}

message traduzaMensagem(char buffer[TAMANHO_MAXIMO_MSG + 1], int fd)
{
  message ret;
  int messageType;
  int i = 2;
  messageType = (int)buffer[0] - '0';
  char tmp[TAMANHO_MAXIMO_MSG - 1];
  cout << "> Mensagem: " << buffer << endl;
  while ((buffer[i] != SEPARADOR) && (buffer[i] != '\0'))
  {
    tmp[i - 2] = buffer[i];
    i++;
  }
  tmp[i - 2] = '\0';
  ret.fd = fd;
  ret.type = messageType;
  ret.id = atoi(tmp);
  return ret;
}

void getGrant(char msg[TAMANHO_MAXIMO_MSG + 1], int id)
{
  //msg[0] = '0' + MESSAGE_GRANT;
  msg[0] = '0' + MESSAGE_GRANT;
  msg[1] = '|';
  string strId = to_string(id);
  char charId[TAMANHO_MAXIMO_MSG - 1];
  strcpy(charId, strId.c_str());
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

int main(int argc, char **argv)
{
  bool flag = true;
  vector<thread> threads;
  deque<message> messageQueue;
  mutex messageQueueMutex;
  deque<message> queue;
  mutex queueMutex;

  
  // Baseado em: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
  threads.push_back(thread([&]() {
    int opt = 1;
    int sock, addressSize, newConnectionFd, maxClients = MAXIMO_CLIENTES, activity, maxFd;
    int clientSocket[MAXIMO_CLIENTES];
    struct sockaddr_in address;
    char buffer[TAMANHO_MAXIMO_MSG + 1];

    // conjunto de descritores de socket
    fd_set readfds;

    // Zera descritores para clientes
    for (int i = 0; i < MAXIMO_CLIENTES; i++)
    {
      clientSocket[i] = 0;
    }

    // Abrir socket
    // SOCK_STREAM se refere a TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
      cerr << "Falha ao abrir o socket." << endl;
      return ERRO_ABRIR_SOCKET;
    }

    // Configura socket para aceitar múltiplas conexões
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
      cerr << "Falha ao configurar o socket." << endl;
      return ERRO_CONFIGURAR_SOCKET;
    }

    // Abrindo porta para conexões
    addressSize = sizeof(sockaddr);
    address.sin_family = AF_INET;          // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Todas as interfaces de rede
    address.sin_port = htons(SOCKET_PORT); // Porta
    //int bindStatus = 0;
    int bindStatus = bind(sock, (struct sockaddr *)&address, (socklen_t)addressSize);
    if (bindStatus != 0)
    {
      cerr << "Erro ao realizar bind na porta " << SOCKET_PORT << "." << endl;
      return ERRO_SOCKET_BIND;
    }

    if (listen(sock, 3) < 0)
    {
      cerr << "Erro ao especificar máximo de conexões na fila." << endl;
      return ERRO_SOCKET_LISTEN;
    }

    // Configurando timeout do select
    // Baseado em: https://www.gta.ufrj.br/ensino/eel878/sockets/selectman.html
    struct timeval tv;
    tv.tv_sec = 1;

    // Loop principal
    while (flag)
    {

      // limpar set de sockets
      FD_ZERO(&readfds);

      // Adicionar o master na lista de sockets
      FD_SET(sock, &readfds);
      maxFd = sock;
      // Adiciona os sockets filho
      for (int i = 0; i < MAXIMO_CLIENTES; i++)
      {
        if (clientSocket[i] > 0)
          FD_SET(clientSocket[i], &readfds);
        if (clientSocket[i] > maxFd)
          maxFd = clientSocket[i];
      }

      // Aguarda por tv segundos por atividade em algum socket
      activity = select(maxFd + 1, &readfds, NULL, NULL, &tv);
      if ((activity < 0) && (errno != EINTR))
        cout << "Erro no comando select." << endl;

      // nova conexão
      if (FD_ISSET(sock, &readfds))
      {
        if ((newConnectionFd = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addressSize)) < 0) // aceitar nova conexao
        {
          cerr << "Erro ao aceitar nova conexão." << endl;
          return ERRO_ACEITAR_SOCKET;
        }
        // conexao foi aceita
        for (int i = 0; i < MAXIMO_CLIENTES; i++)
          if (clientSocket[i] == 0)
          {
            clientSocket[i] = newConnectionFd;
            break;
          }
      }

      // Input/Output em outro socket
      for (int i = 0; i < MAXIMO_CLIENTES; i++)
        if (FD_ISSET(clientSocket[i], &readfds))
        {
          int valread;
          // Desconectar socket
          if ((valread = read(clientSocket[i], buffer, TAMANHO_MAXIMO_MSG + 1)) == 0)
          {
            close(clientSocket[i]);
            clientSocket[i] = 0;
          }
          // Mandou mensagem
          else
          {
            message tmp = traduzaMensagem(buffer, clientSocket[i]);
            messageQueueMutex.lock();
            messageQueue.push_back(tmp); // adicione mensagem na fila
            messageQueueMutex.unlock();
          }
        }
    }
    return OK;
  }));

  // Thread Mutex
  threads.push_back(thread([&]() {
    bool locked = false;
    bool work = false;
    bool queueWork = false;
    message msg;
    while (flag)
    {
      // Retira a mensagem da fila
      messageQueueMutex.lock();
      if (messageQueue.size() > 0)
      {
        msg = messageQueue.front();
        messageQueue.pop_front();
        work = true;
      }
      messageQueueMutex.unlock();

      // Se a mensagem foi retirada -> work = true
      if (work)
      {
        switch (msg.type)
        {
        case MESSAGE_REQUEST:
          queueMutex.lock();
          queue.push_back(msg); // adicione na fila
          queueMutex.unlock();
          break;
        case MESSAGE_RELEASE:
          if (!locked)
          {
            cerr << "Erro unlocked lock" << endl;
            return ERRO_UNLOCK_LOCK;
          }
          locked = false;
          break;
        default:
          cerr << "Mensagem " << msg.type << " desconhecida" <<endl;
          return ERRO_MENSAGEM_DESCONHECIDA;
        }
        work = false;
      }

      // Queue
      if (!locked)   // se locked for false
      {
        queueMutex.lock();
        if (queue.size() > 0) // adquirir mensagem e deletar da fila
        {
          msg = queue.front();
          queue.pop_front();
          queueWork = true;
        }
        queueMutex.unlock();
        if (queueWork) // se alguma mensagem foi adquirida
        {
          // Enviar permissão 
          char grant[TAMANHO_MAXIMO_MSG + 1];
          getGrant(grant,msg.id);
          cout << "> Mensagem: " << grant << endl;
          send(msg.fd, grant, TAMANHO_MAXIMO_MSG + 1, 0);
          // lock
          locked = true;
          queueWork = false;

        }
      }
    }
    return OK;
  }));

  // Thread Interface
  threads.push_back(thread([&]() {
    string strComando;
    cout << "Coordenador acordou..." << endl;
    while (flag)
    {
      getline(cin, strComando);
      switch (traduzaComando(strComando))
      {
      case comando_print_fila:
        cout << "Fila de pedidos:" << endl;
        queueMutex.lock();
        if (queue.size() == 0)
          cout << "Não tem nenhum pedido na fila." << endl;
        else
          for (int i = 0; i < queue.size(); i++)
            cout << "  - " << i + 1 << "º: " << queue[i].id << endl;
        queueMutex.unlock();
        break;

      case comando_print_historico:
        cout << "Disponivel em breve..." << endl;
        break;

      case comando_quit:
        cout << "Encerrando programa..." << endl;
        flag = false;
        break;

      default:
        cout << "Comando \"" << strComando << "\" inválido!" << endl;
        break;
      }
    }
    return OK;
  }));

  for (int i = 0; i < threads.size(); i++)
    threads[i].join();

  return OK;
}