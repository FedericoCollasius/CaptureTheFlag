#include <iostream>
#include <thread>
#include <sys/unistd.h>
#include <vector>
#include <mutex>
#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"

using namespace std;

const estrategia strat = USTEDES;
const int quantum = 6;

int main(){

    vector<string> vectorsito;
    for (int i = 0; i < 10000; i++)
    {
        cout << "corriendo test " << i << endl;

        Config config (to_string(i) + ".csv");

    
        gameMaster belcebu = gameMaster(config);
    
        belcebu.strat = strat;//culpa peron
	    // Creo equipos (lanza procesos)

	    Equipo rojo(&belcebu, ROJO, strat, config.cantidad_jugadores, quantum, config.pos_rojo);
	    Equipo azul(&belcebu, AZUL, strat, config.cantidad_jugadores, quantum, config.pos_azul);


        rojo.comenzar();
        azul.comenzar();
	    rojo.terminar();
	    azul.terminar();	
        //belcebu.play();

        cout << "Bandera capturada por el equipo "<< belcebu.ganador << ". Felicidades!" << "\n";
    }
}

