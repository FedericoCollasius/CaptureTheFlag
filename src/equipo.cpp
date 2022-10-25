#include "equipo.h"
/**/
#include <unistd.h>
#include <atomic>
#include <semaphore.h>
/**/


direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
	if (pos2.first > pos1.first) return ABAJO;
	if (pos2.first < pos1.first) return ARRIBA;
	if (pos2.second > pos1.second) {
		return DERECHA;
	} else if (pos2.second < pos1.second) 
		return IZQUIERDA;
}


void Equipo::jugador(int nro_jugador) {
	/**/
	if (equipo == ROJO) {
		sem_wait(&bandera_roja_encontrada);
	} else {
		sem_wait(&bandera_azul_encontrada);
	}
	/**/

	while(!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
		switch(this->strat) {
			//SECUENCIAL,RR,SHORTEST,USTEDES
			case(SECUENCIAL): // No espera a nadie. 
				// 
				// 
				//
				break;
			
			case(RR): // Espera a que le toque nro = movidos. 
				/**/
				if(nro_jugador==(jugadores_movidos_esta_ronda+1)%cant_jugadores){
					if(equipo == ROJO){
						belcebu->mover_jugador(apuntar_a(posActualJugador, pos_bandera_azul), nro_jugador);
					} else {
						belcebu->mover_jugador(apuntar_a(posActualJugador, pos_bandera_roja), nro_jugador);
					}
				}
				/**/
				break;

			case(SHORTEST): 
				//
				//	...
				//
				break;
			case(USTEDES):
				//
				// ...
				//
				break;
			default:
				break;
		}	
		// Termino ronda ? Recordar llamar a belcebu...
		// OJO. Esto lo termina un jugador... 
		//
		// ...
		//
	}
	
}
/**/
// Variables globales para simplificar el uso de memoria compartida
atomic<int> jugadores_movidos_esta_ronda(0);
atomic<int> nro_jugador_mas_cercano_rojo(0);
atomic<int> nro_jugador_mas_cercano_azul(0);
sem_t bandera_roja_encontrada, bandera_azul_encontrada;
// Sacamos la parte atomica de las banderas porque no es necesario que sea atomica para el problema que planteamos (no hay dos equipos que quieran modificar la misma bandera a la vez) 
coordenadas pos_bandera_azul, pos_bandera_roja;
vector<coordenadas> posiciones_azul, posiciones_rojo;
/**/

Equipo::Equipo(gameMaster *belcebu, color equipo, 
		estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones) {
	this->belcebu = belcebu;
	this->equipo = equipo;
	this->contrario = (equipo == ROJO)? AZUL: ROJO;
	this->bandera_contraria = (equipo==ROJO)? BANDERA_AZUL: BANDERA_ROJA;
	this->strat = strat;
	this->quantum = quantum;
	this->quantum_restante = quantum;
	this->cant_jugadores = cant_jugadores;
	/**/
	//this->posiciones = posiciones;
	vector<int[2]> los_tubos;
	for (int i = 0; i < cant_jugadores; i++) {
		pipe(los_tubos[i]);
	}
	if (equipo == ROJO) {
		sem_init(&bandera_roja_encontrada, 1337/*nonzero*/, cant_jugadores);
		posiciones_rojo = posiciones;
	} else {
		sem_init(&bandera_azul_encontrada, 1337/*nonzero*/, cant_jugadores);
		posiciones_azul = posiciones;
	}
	/**/
}

void Equipo::comenzar() {
	// Arranco cuando me toque el turno 
	// TODO: Quien empieza ? 
	/**/
	(equipo == ROJO ? belcebu->turno_rojo : belcebu->turno_azul).lock(); // Espero mi turno. 
	//
	// ...	
	//
	// Creamos los jugadores
	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i)); 
		//Cerramos pipes?????????????????????????????
	}

	buscar_bandera_contraria(); // van a volar signals a rolete
	terminar();
}

void Equipo::terminar() {
	for(auto &t:jugadores){
		t.join();
	}	
}

coordenadas Equipo::buscar_bandera_contraria() {
	//
	// ...
	//

	/**/ 
	// Dejar que empiezen a jugar todos.
	for (int pp = 0; pp < cant_jugadores; pp++) {
		if (equipo == ROJO) {
			sem_post(&bandera_roja_encontrada);
		} else {
			sem_post(&bandera_azul_encontrada);
		}
	}
	/**/
}
