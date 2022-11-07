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

const estrategia strat = SHORTEST;
const int quantum = 6;

// Modificamos el main.cpp y config.cpp para poder correr varios tests de una
// Para poder hacerlo, se debe correr primero el archivo armarTests.py en la carpeta config
int main(){
    for (int i = 0; i < 1000; i++) { 
        Config config (to_string(i) + ".csv");
        gameMaster belcebu = gameMaster(config);

        belcebu.setear_strat(strat);
	    // Creo equipos (lanza procesos)
        cout << "corriendo test " << i << endl;


	    Equipo rojo(&belcebu, ROJO, strat, config.cantidad_jugadores, quantum, config.pos_rojo);
	    Equipo azul(&belcebu, AZUL, strat, config.cantidad_jugadores, quantum, config.pos_azul);
        struct timespec start, finish, delta;
        clock_gettime(CLOCK_REALTIME, &start);
        rojo.comenzar();
        azul.comenzar();
	    rojo.terminar();
	    azul.terminar();	
        //belcebu.play();

        cout << "Bandera capturada por el equipo "<< belcebu.ganador << ". Felicidades!" << "\n";
    }
}