//Este programa foi utilizado para facilitar a obtençao de dados para ser plotados no gráfico pedido no trabalho 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<string>

using namespace std;
#define TAMANHO_VETOR_SOMADOR 10e9


int main () {
    int parameters[20] = {1,2,4,8,16,32,64,128,256};
    char buffer[256];

    cout << "Tamanho vetor"<< TAMANHO_VETOR_SOMADOR <<"\n";

    	
	for (int i=0; i<9; i++) {
        string nomeComando = "./spinlock "+to_string(parameters[i]);
        const char *command = nomeComando.c_str();
        system(command);
        cout <<  "\n";
    }

    return 0; 






}
