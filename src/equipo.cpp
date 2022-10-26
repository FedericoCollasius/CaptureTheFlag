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
	sem_wait(&bandera_contraria_encontrada);
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
						belcebu->mover_jugador(apuntar_a(posiciones[nro_jugador], pos_bandera_contraria), nro_jugador);
					} else {
						belcebu->mover_jugador(apuntar_a(posiciones[nro_jugador], pos_bandera_contraria), nro_jugador);
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
// Eliminamos variables globales compartidas porque la consigna no lo permite. 
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
	this->posiciones = posiciones;
	
	/**/
	vector<int[2]> los_tubos;
	for (int i = 0; i < cant_jugadores; i++) {
		pipe(los_tubos[i]);
	}
	sem_init(&bandera_contraria_encontrada, 1, cant_jugadores);
	jugadores_movidos_esta_ronda = 0; 
	/**/
}

void Equipo::comenzar() {
	// Arranco cuando me toque el turno 
	// TODO: Quien empieza ? 
	//
	// ...	
	//
	/**/
	// Deberiamos esperar a que el otro equipo encuentre la bandera contaria para ejecutar threads?
	buscar_bandera_contraria(); // van a volar signals a rolete
	/**/
	// Creamos los jugadores
	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i)); 
		//Cerramos pipes?????????????????????????????
	}


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
		sem_post(&bandera_contraria_encontrada);
	}
	/**/
}
