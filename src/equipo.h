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

		int cant_jugadores_que_ya_jugaron = 0;
		/**/
		vector<coordenadas> posiciones; 
		coordenadas pos_bandera_contraria; 
		// sem_t bandera_contraria_encontrada;
		// Quizas jugadores_movidos_esta_ronda tenga que ser atomica por el caso secuencial. 
		atomic<int> jugadores_movidos_esta_ronda{0};
		int nro_jugador_mas_cercano;
		/**/
		
		vector<mutex> orden_jugadores_rr;
		vector<bool> se_movio_jugador;

		// Métodos privados 
		direccion apuntar_a(coordenadas pos2, coordenadas pos1);
		void jugador(int nro_jugador, datos_conjuntos_de_equipo & datitos);
		coordenadas buscar_bandera_contraria();
		direccion direccion_proxima_posicion(coordenada posActual, direccion direc_deseada);
		mutex tablero; 
		mutex bandera_contraria_encontrada;
		//
		// ...
		//
	public:
		Equipo(gameMaster *belcebu, color equipo, 
				estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones);
		void comenzar();
		void terminar();
		// crear jugadores

};



#endif // EQUIPO_H
