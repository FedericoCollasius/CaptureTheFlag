#include "equipo.h"
/**/
#include <unistd.h>
#include <atomic>
#include <semaphore.h>
#include <assert.h> 
/**/


direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
	if (pos2.first > pos1.first) return ABAJO;
	if (pos2.first < pos1.first) return ARRIBA;
	if (pos2.second > pos1.second) {
		return DERECHA;
	} else if (pos2.second < pos1.second) 
		return IZQUIERDA;
}


/*
*/

void Equipo::jugador(int nro_jugador, datos_conjuntos_de_equipo & datos_equipo) {
	/**/
	datos_equipo.bandera_contraria_encontrada.lock();
	/**/
	coordenadas pos_actual = posiciones[nro_jugador];
	
	while(!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
		switch(this->strat) {
			//SECUENCIAL,RR,SHORTEST,USTEDES
			case(SECUENCIAL): // No espera a nadie. 
				{
					datos_equipo.tablero.lock();
					direccion direc_nueva = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_deseada = direc_nueva;
					
					// Elijo la proxima direccion en caso de no poder moverme.
					while(!belcebu->sePuedeMover(pos_actual, direc_nueva) && direc_nueva != direc_deseada){
						if(direc_deseada == ARRIBA || IZQUIERDA){
							direc_nueva = (direccion) ((direc_deseada - 1) % 4);
						} else {
							direc_nueva = (direccion) ((direc_deseada + 1) % 4);
						}
					}
				
					if (belcebu->sePuedeMover(pos_actual, direc_nueva));
					belcebu->mover_jugador(direc_nueva, nro_jugador);
					jugadores_movidos_esta_ronda++;
					datos_equipo.tablero.unlock();
					break;
				}
			
			case(RR): // Espera a que le toque nro = movidos. 
				{/**/
				/*
				if(nro_jugador==(jugadores_movidos_esta_ronda+1)%cant_jugadores){
					//if(equipo == ROJO){
					//	belcebu->mover_jugador(apuntar_a(posActual, pos_bandera_contraria), nro_jugador);
					//} else {
					//	belcebu->mover_jugador(apuntar_a(posActual, pos_bandera_contraria), nro_jugador);
					belcebu->mover_jugador(apuntar_a(pos_actual, pos_bandera_contraria), nro_jugador);
					jugadores_movidos_esta_ronda++;
				}
				*/
				//}
				/**/
					break;
				}

			case(SHORTEST):
				{ 
					belcebu->mover_jugador(apuntar_a(pos_actual, pos_bandera_contraria),nro_jugador_mas_cercano);
					break;
				}
			case(USTEDES):
				{//
				// ...
				//
					break;
				}
			default:
				{
					break;
				}
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
	//jugadores_movidos_esta_ronda = 0; 
	/**/
}

void Equipo::comenzar() {
	// Arranco cuando me toque el turno 
	// TODO: Quien empieza ? 
	//
	// ...	
	//
	
	datos_equipo.bandera_contraria_encontrada.unlock();
	datos_equipo.tablero.unlock();


	// Creamos los jugadores
	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i, ref(datos_equipo))); 
		//Cerramos pipes?????????????????????????????
	}

	buscar_bandera_contraria(); // van a volar signals a rolete


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
	if( equipo == ROJO) {
		pos_bandera_contraria = make_pair(0,0);
	} else {//seteo valores por simplicidad, no me hinchen
		pos_bandera_contraria = make_pair(9,9);
	}

	/**/ 
	// Dejar que empiezen a jugar todos.
	for (int pp = 0; pp < cant_jugadores; pp++) {
		datos_equipo.bandera_contraria_encontrada.unlock();
	}
	/**/
}
