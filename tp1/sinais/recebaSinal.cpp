#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

int definaTipoWait(void){

    int tipoWait = -1;

    while (tipoWait != 0 && tipoWait != 1){

        cout << "Digite 0 para busy wait ou 1 para blocking wait: ";
        cin >> tipoWait;
    }

    return tipoWait; 
}

void signalHandler(int signalNumber) {
    
   switch(signalNumber) {
    case 1:
        // sleep(2);
        cout << "Sinal recebido: SIGHUP" << endl;
        exit(signalNumber);  
        break; 
    case 2:
        // sleep(2);
        cout << "Sinal recebido: SIGINT" << endl;
        exit(signalNumber);  
        break; 
    case 3:
        // sleep(2);
        cout << "Sinal recebido: SIGQUIT" << endl;
        exit(signalNumber);  
        break; 
    case 4:
        // sleep(2);
        cout << "Sinal recebido: SIGILL" << endl;
        exit(signalNumber);  
        break; 
    }
}

int main (void) {
    
   int tipoWait; 

   pid_t pidNumber = getpid();

   signal(SIGHUP, signalHandler);  
   signal(SIGINT, signalHandler);  
   signal(SIGQUIT, signalHandler);   
   signal(SIGILL, signalHandler);  

   cout << "Numero PID: " << pidNumber << "\n\n "; 
   tipoWait = definaTipoWait();

   if (tipoWait == 0){
        //Busy wait
        while(true){
            cout << "Sinal ainda nao foi recebido -- busy wait\n";
        }

    }else{

        //Blocking Wait
        while(true) {
            cout << "Sinal ainda nao foi recebido -- blocking wait\n";
            sleep(2);
        }

    }
    
    return 0;
}