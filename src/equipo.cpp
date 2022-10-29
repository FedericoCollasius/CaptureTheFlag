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

direccion Equipo::direccion_proxima_posicion(coordenada posActual, direccion direc_deseada) {
	direccion direc_nueva = direc_deseada;
	// Elijo la proxima direccion en caso de no poder moverme.
	while(!belcebu->sePuedeMover(pos_actual, direc_nueva) && direc_nueva != direc_deseada){
		if(direc_deseada == ARRIBA || IZQUIERDA){
			direc_nueva = (direccion) ((direc_deseada - 1) % 4);
		} else {
			direc_nueva = (direccion) ((direc_deseada + 1) % 4);
		}
	}

	return direc_nueva;
}


/*
*/

void Equipo::jugador(int nro_jugador) {
	/**/
	this->bandera_contraria_encontrada.lock();
	/**/
	coordenadas pos_actual = posiciones[nro_jugador];
	
	while(!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
		switch(this->strat) {
			//SECUENCIAL,RR,SHORTEST,USTEDES
			case(SECUENCIAL): // No espera a nadie. 
				{
					this->tablero.lock();
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
				
					if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}

					this->se_movio_jugador[nro_jugador] = true;
					jugadores_movidos_esta_ronda.get_and_inc();
					this->tablero.unlock();
					break;
				}
			
			case(RR): // Espera a que le toque nro = movidos. 
				{
				this->orden_jugadores_rr[nro_jugador].lock();
				if (this->quantum_restante > 0){
					this->tablero.lock();
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
				
					if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}
					this->quantum_restante--;
					this->tablero.unlock();	
				} else {
					// Termino la ronda. 
				}
				this->orden_jugadores_rr[(nro_jugador+1)%cant_jugadores].unlock();
				break;
				}
				
			case(SHORTEST):
				{ 
					this->tablero.lock();
					if(nro_jugador == nro_jugador_mas_cercano){
						direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
						direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
					
						if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
							belcebu->mover_jugador(direc_nueva, nro_jugador);
							this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
						}
					}
					this->tablero.unlock();
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
	
	this->orden_jugadores_rr(cant_jugadores, mutex(0));
	this->orden_jugadores_rr[0].unlock(); 

	this->se_movio_jugador(cant_jugadores, false);

	this->nro_jugador_mas_cercano = -1;

}

void Equipo::comenzar() {
	// Arranco cuando me toque el turno 
	// TODO: Quien empieza ? 
	//
	// ...	
	//
	
	this->bandera_contraria_encontrada.unlock();
	this->tablero.unlock();


	// Creamos los jugadores
	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i)); 
	}

	buscar_bandera_contraria(); // van a volar signals a rolete
	if(this->strat == SHORTEST) {
		this->nro_jugador_mas_cercano = jugador_mas_cercano();
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

int Equipo::jugador_mas_cercano() {
	int distancia_mas_cercana = belcebu->getTamx() + belcebu->getTamy();
	int nro_jugador_mas_cercano = -1;
	for(auto &t:this->posiciones) {
		int distancia_jugador_bandera = belcebu->distancia(t, this->pos_bandera_contraria);
		if(distancia_jugador_bandera < distancia_mas_cercana) {
			distancia_mas_cercana = distancia_jugador_bandera;
			nro_jugador_mas_cercano = t.first; // Checkear que atributo de jugador es su id. 
		}
	}
	return nro_jugador_mas_cercano;
}