#include <iostream>
#include <signal.h>

#define PID_NAO_ENCONTRADO 1
#define ERRO_DE_SINAL 2

using namespace std;

int main(void){

    int pidNumber; 
    int sinal; 

    cout << "Numero PID: ";
    cin  >>  pidNumber;

    if (kill(pidNumber,0)!=0){

        cout <<"PID nao encontrada.\n";
        return PID_NAO_ENCONTRADO; 
    }

    cout << "Sinal a emitir: ";
    cin >> sinal;


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
