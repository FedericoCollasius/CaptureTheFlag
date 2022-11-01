#ifndef EQUIPO_H
#define EQUIPO_H

#include <semaphore.h>
#include <vector>
#include <thread>
#include <atomic>
#include "definiciones.h"
#include "gameMaster.h"

using namespace std;

class Equipo {
	private:

		// Atributos Privados 
		gameMaster *belcebu; 
		color contrario, equipo, bandera_contraria;
		estrategia strat;
		int cant_jugadores, quantum, quantum_restante;
		vector<thread> jugadores;

		vector<coordenadas> posiciones; 
		coordenadas pos_bandera_contraria; 

		// Variables de sincronizacion
		mutex tablero; 
		mutex bandera_contraria_encontrada;
		
		// Caso SECUENCIAL
		vector<bool> se_movio_jugador;
		int jugadores_movidos_esta_ronda;
		
		// Caso RR
		vector<sem_t> orden_jugadores_rr;
		
		// Caso SHORTEST
		int nro_jugador_mas_cercano;

		// Caso USTEDES
		int cant_mov_ustedes; 

		// Métodos privados 
		direccion apuntar_a(coordenadas pos2, coordenadas pos1);
		void jugador(int nro_jugador);
		void buscar_bandera_contraria(int nro_jugador);

		direccion direccion_proxiam_posicion(coordenadas posActual, direccion direc_deseada);
		int jugador_mas_cercano();
		
	public:
		Equipo(gameMaster *belcebu, color equipo, 
				estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones);
		void comenzar();
		void terminar();
};

#endif // EQUIPO_H
