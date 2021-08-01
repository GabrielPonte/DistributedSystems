#include <iostream>
#include <signal.h>

#define PID_NAO_ENCONTRADO 1
#define ERRO_DE_SINAL 2

using namespace std;

// Mensagem da interface caso ocorra algum problema
void msgInterface()
{
  cout << "Forma de uso: ./emitaSinal" << endl;
  cout << "Nao e preciso ter nenhum argumento" << endl;
  cout << "Digite o PID apresentado em ./recebaSinal" << endl;
  cout << "Digite um sinal pertencente a esta lista abaixo:" << endl;
  cout << "   1: SIGHUP" << endl;
  cout << "   2: SIGINT " << endl;
  cout << "   3: SIGQUIT" << endl;
  cout << "   4: SIGILL" << endl << endl;
}


int main(void){

    int pidNumber; 
    int sinal; 

    cout << "Numero PID: ";
    cin  >>  pidNumber;

    if (kill(pidNumber,0)!=0){

        cout <<"PID nao encontrada" << endl << endl;
        msgInterface();
        return PID_NAO_ENCONTRADO; 
    }

    while(true){
        cout << "Sinal que deseja emitir: ";
        cin  >> sinal;

        if(sinal >= 1 && sinal <= 4){
            break;
        }else{
            msgInterface();
        }
    
    }


    try{
        if (kill(pidNumber,sinal) != 0){
            throw errno; 
        } 
        cout << "Sinal " << sinal << " enviado para o PID " << pidNumber << ".\n";
        return 0;
    }
    catch(int e){
        cout << "  " << e << "\n" ;
        return ERRO_DE_SINAL;  
    }

}
