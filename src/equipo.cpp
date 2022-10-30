#include "equipo.h"
/**/
#include <unistd.h>
#include <atomic>
#include <semaphore.h>
#include <assert.h> 
/**/


direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
	//cout << "pos1: " << pos1.first << " " << pos1.second << endl;
	//cout << "pos2: " << pos2.first << " " << pos2.second << endl;
	if (pos2.second > pos1.second) return ABAJO; //cambie esto porque estaba mal
	if (pos2.second < pos1.second) return ARRIBA;
	if (pos2.first > pos1.first) {
		return DERECHA;
	} else if (pos2.first < pos1.first) 
		return IZQUIERDA;
}

direccion Equipo::direccion_proxima_posicion(coordenadas posActual, direccion direc_deseada) {
	direccion direc_nueva = direc_deseada;
	direccion opuesta = (direc_deseada == ARRIBA) ? ABAJO : (direc_deseada == ABAJO) ? ARRIBA : (direc_deseada == IZQUIERDA) ? DERECHA : IZQUIERDA; //buen copilot
	//cout << "me quiero mover a: " << direc_deseada << endl;
	//cout << "estoy en " << posActual.first << " " << posActual.second << endl;
	if(belcebu->sePuedeMover(posActual, direc_nueva)) {
		//cout << "me puedo mover a: " << direc_nueva << endl;
		return direc_nueva;
	}
	direc_nueva = (direccion)((direc_deseada + 1) % 4);
	if(belcebu->sePuedeMover(posActual, direc_nueva) && direc_nueva != opuesta) {
		//cout << "me puedo mover a: " << direc_nueva << endl;
		return direc_nueva;
	}
	direc_nueva = (direccion)((direc_deseada + 2) % 4);
	if(belcebu->sePuedeMover(posActual, direc_nueva) && direc_nueva != opuesta) {
		//cout << "me puedo mover a: " << direc_nueva << endl;
		return direc_nueva;
	}
	if(belcebu->sePuedeMover(posActual, opuesta)) {
		//cout << "me puedo mover a: " << opuesta << endl;
		return opuesta;
	}
	assert(true);
	assert(false);
	return ARRIBA;
	//bool direc[4] = {false, false, false, false};
	// Elijo la proxima direccion en caso de no poder moverme.
	//while(!belcebu->sePuedeMover(posActual, direc_nueva) && direc_nueva != direc_deseada){
	//	if(direc_deseada == ARRIBA || direc_deseada == IZQUIERDA){
	//		direc_nueva = (direccion) ((direc_deseada - 1) % 4);
	//	} else {
	//		direc_nueva = (direccion) ((direc_deseada + 1) % 4);
	//	}
	//}
}


/*
*/

void Equipo::jugador(int nro_jugador) {
	/**/
	this->bandera_contraria_encontrada.lock();
	this->bandera_contraria_encontrada.unlock();
	/**/
	coordenadas pos_actual = posiciones[nro_jugador];
	
	while(!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
		if(equipo == ROJO){
			sem_wait(&(belcebu->turno_rojo));
			sem_post(&(belcebu->turno_rojo));
		}
		else{
			sem_wait(&(belcebu->turno_azul));
			sem_post(&(belcebu->turno_azul));
		}
		if(belcebu->termino_juego())break;
		switch(this->strat) {
			//SECUENCIAL,RR,SHORTEST,USTEDES
			case(SECUENCIAL): // No espera a nadie. 
				{	
					if(se_movio_jugador[nro_jugador]){
						//sleep(5);
						break;
					}
					this->tablero.lock();
					pos_actual = posiciones[nro_jugador];
					//cout << jugadores_movidos_esta_ronda << " "<< cant_jugadores << " " <<nro_jugador<< endl;
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
					if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}else{
						cout << "AYUDAAAAA, NO ME PUEDO MOVERRREERE" << endl;
					}

					this->se_movio_jugador[nro_jugador] = true;
					jugadores_movidos_esta_ronda++;
					//cout << jugadores_movidos_esta_ronda << " "<< cant_jugadores << endl;
					if(jugadores_movidos_esta_ronda == this->cant_jugadores){
						
						cout<< "Termino ronda equipo " << equipo << endl;
						belcebu->termino_ronda(equipo);
						jugadores_movidos_esta_ronda = 0;
						this->se_movio_jugador = vector<bool>(this->cant_jugadores, false);
						
						
					}
					
					//sleep(1);
					//cout<<"ya no me toca" << endl;
					this->tablero.unlock();
					break;;
				}
			
			case(RR): // Espera a que le toque nro = movidos. 
				{
				cout << "i shouldnt be here" << endl;
				sem_wait(&orden_jugadores_rr[nro_jugador]);
				if (this->quantum_restante > 0){
					this->tablero.lock();
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
				
					if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}
					this->quantum_restante--;
					if(this->quantum_restante == 0){
						this->quantum_restante = this->quantum;
						belcebu->termino_ronda(equipo);
					}
					this->tablero.unlock();	
				}
				sem_post(&orden_jugadores_rr[(nro_jugador+1)%cant_jugadores]);
				break;
				}
				
			case(SHORTEST):
				{ 
					
				cout << "i shouldnt be here" << endl;
					this->tablero.lock();
					if(nro_jugador == nro_jugador_mas_cercano){
						direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
						direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
					
						if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
							belcebu->mover_jugador(direc_nueva, nro_jugador);
							this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
						}
					}
					belcebu->termino_ronda(equipo);
					this->tablero.unlock();
				}
			case(USTEDES):
				{//
				
				cout << "i shouldnt be here" << endl;
				// ...
				//
					break;
				}
			default:
			
				{
				cout << "i shouldnt be here" << endl;

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

	
	//orden_jugadores_rr.resize(cant_jugadores);

	this->orden_jugadores_rr = vector<sem_t>(this->cant_jugadores);
    //initialize vector of semaphores
    for (int i = 0; i < this->cant_jugadores; i++) {
        sem_init(&this->orden_jugadores_rr[i], 0, 0);
    }
	sem_post(&orden_jugadores_rr[0]);


	//for(int i =0; i < cant_jugadores; i++){
	//	orden_jugadores_rr.push_back(std::mutex());
	//}
	//mutex m;
	//vector<mutex> vectorisito(cant_jugadores, m);
	//orden_jugadores_rr = vectorisito;
	//this->orden_jugadores_rr[0].unlock(); 



	vector<bool> v(cant_jugadores,false);
	this->se_movio_jugador = v;

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

buscar_bandera_contraria(); 
	// Creamos los jugadores
	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i)); 
	}

	// van a volar signals a rolete
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
		pos_bandera_contraria = make_pair(9,1);
	} else {//seteo valores por simplicidad, no me hinchen
		pos_bandera_contraria = make_pair(10,10);
	}

	/**/ 
	// Dejar que empiezen a jugar todos.
	for (int pp = 0; pp < cant_jugadores; pp++) {
		bandera_contraria_encontrada.unlock();
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