#include "equipo.h"
#include <unistd.h>
#include <atomic>
#include <semaphore.h>
#include <assert.h> 


direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
	//cout << "pos1: " << pos1.first << " " << pos1.second << "\n";
	//cout << "pos2: " << pos2.first << " " << pos2.second << "\n";
	if (pos2.second > pos1.second) return ABAJO; 
	if (pos2.second < pos1.second) return ARRIBA;
	if (pos2.first > pos1.first) {
		return DERECHA;
	} else if (pos2.first < pos1.first) 
		return IZQUIERDA; 
}

void Equipo::jugador(int nro_jugador) {
	buscar_bandera_contraria(nro_jugador);
	
	this->bandera_contraria_encontrada.lock();
	this->bandera_contraria_encontrada.unlock();
	if(pos_bandera_contraria == make_pair(-1,-1)){
		cout << "No hay bandera contraria" << "\n";
		assert(false);
		return;
	}
	//cout << "la bandera contraria esta en: " << this->pos_bandera_contraria.first << " " << this->pos_bandera_contraria.second << "\n";
	coordenadas pos_actual; 

	int cant_veces_mov_ustedes = 0;
	
	while(!this->belcebu->termino_juego()) {
		//if(equipo == ROJO){
		//	sem_wait(&(belcebu->turno_rojo));
		//	sem_post(&(belcebu->turno_rojo));
		//}
		//else{
		//	sem_wait(&(belcebu->turno_azul));
		//	sem_post(&(belcebu->turno_azul));
		//}
		if(belcebu->termino_juego()){
			if(strat == RR){
				////cout << "El equipo " << equipo << " libera" << "\n";
				for(int i = 0; i < cant_jugadores;i++){
					////cout << "liberamos" << "\n";
					sem_post(&orden_jugadores_rr[i]);
				}
			}
			break;
		}
		////cout << "el petiso es: " << nro_jugador_mas_cercano << "\n";
		switch(this->strat) {
			case(SECUENCIAL):
				{	
					this->tablero.lock();
					
					if(equipo == ROJO){
						sem_wait(&(belcebu->turno_rojo));
						sem_post(&(belcebu->turno_rojo));
					}
					else{
						sem_wait(&(belcebu->turno_azul));
						sem_post(&(belcebu->turno_azul));
					}


					if(se_movio_jugador[nro_jugador] ||  belcebu->termino_juego()){
						////cout << "ups equipo: "<< nro_jugador << "\n";
						tablero.unlock();
						break;
					}
					pos_actual = posiciones[nro_jugador];
					//cout << jugadores_movidos_esta_ronda << " "<< cant_jugadores << " " <<nro_jugador<< "\n";
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					//cout << "direccion deseada: " << direc_deseada << "\n";
					//cout << "mis constantes son: " << ARRIBA << " " << ABAJO << " " << IZQUIERDA << " " << DERECHA << "\n";
					//assert(direc_deseada < 5);
					direccion direc_nueva = direccion_proxiam_posicion(pos_actual, direc_deseada);
					//cout << "direccion nueva: " << direc_nueva << " jugador " << nro_jugador << " en " << pos_actual.first << " " << pos_actual.second << "\n";
					if (belcebu->se_puede_mover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						//this->se_movio_jugador[nro_jugador] = true;
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}else{
						belcebu->aumentar_jugadores_movidos();
						//cout << "AYUDAAAAA, NO ME PUEDO MOVERRREERE" << "\n";
					}

					this->se_movio_jugador[nro_jugador] = true;
					jugadores_movidos_esta_ronda++;
					//cout << jugadores_movidos_esta_ronda << " "<< cant_jugadores << "\n";
					if(jugadores_movidos_esta_ronda == this->cant_jugadores || belcebu->termino_juego()){ 
						
						if(equipo == ROJO){
							//cout<< "Termino ronda " << belcebu->nro_ronda << " equipo " << "ROJO" << "\n";
						}
						else{
							//cout<< "Termino ronda " << belcebu->nro_ronda << " equipo " << "AZUL" << "\n";
						}
						jugadores_movidos_esta_ronda = 0;
						this->se_movio_jugador = vector<bool>(this->cant_jugadores, false);
						belcebu->termino_ronda(equipo);
					}
		
					//cout<<"ya no me toca" << "\n";
					this->tablero.unlock();
					break;
				}
			
			case(RR):
				{
					//cout << "i shouldnt be here" << "\n";
					sem_wait(&orden_jugadores_rr[nro_jugador]);
					

					if(equipo == ROJO){
						sem_wait(&(belcebu->turno_rojo));
						sem_post(&(belcebu->turno_rojo));
					}
					else{
						sem_wait(&(belcebu->turno_azul));
						sem_post(&(belcebu->turno_azul));
					}

					//if(belcebu->de_quien_es_el_turno() != equipo){
					//	//cout << "no me toca" << "\n";
					//	sem_post(&orden_jugadores_rr[nro_jugador]);
					//	break;
					//}
					
					if(belcebu->termino_juego()){
						for(int i = 0; i < cant_jugadores;i++){
							sem_post(&orden_jugadores_rr[i]);
						}
						break;
					}
					if (this->quantum_restante > 0){
						pos_actual = posiciones[nro_jugador];
						direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
						direccion direc_nueva = direccion_proxiam_posicion(pos_actual, direc_deseada);
					
						if (belcebu->se_puede_mover(pos_actual, direc_nueva)){
							belcebu->mover_jugador(direc_nueva, nro_jugador);
							this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
						}
						this->quantum_restante--;
						if(belcebu->termino_juego()){
							for(int i = 0; i < cant_jugadores;i++){
								//cout << "liberamos" << "\n";
								sem_post(&orden_jugadores_rr[i]);
							}
							belcebu->termino_ronda(equipo);
							break;
						}
						if(this->quantum_restante == 0){
							//cout << "terminamos muchachos" << "\n";
							this->quantum_restante = this->quantum;
							belcebu->termino_ronda(equipo);
							sem_post(&orden_jugadores_rr[0]);
						}else{
							sem_post(&orden_jugadores_rr[(nro_jugador+1)%cant_jugadores]);
						}	
					}
					break;
				}
				
			case(SHORTEST):
				{ 
					this->tablero.lock();
					if(equipo == ROJO){
						sem_wait(&(belcebu->turno_rojo));
						sem_post(&(belcebu->turno_rojo));
					}
					else{
						sem_wait(&(belcebu->turno_azul));
						sem_post(&(belcebu->turno_azul));
					}

					
					
					this->nro_jugador_mas_cercano = jugador_mas_cercano();
					
					if(nro_jugador == nro_jugador_mas_cercano && !(belcebu->termino_juego())){
					//cout << "Entré. El jugador mas cercano es: " << nro_jugador_mas_cercano << "\n";

						pos_actual = posiciones[nro_jugador];
						//cout << "soy el petiso, el: " <<nro_jugador<< "\n";
						direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
						direccion direc_nueva = direccion_proxiam_posicion(pos_actual, direc_deseada);
					
						if (belcebu->se_puede_mover(pos_actual, direc_nueva)){
							belcebu->mover_jugador(direc_nueva, nro_jugador);
							this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
						}else{
							belcebu->jugadores_movidos++;
							cout << "ESTO ES IMPOSIBLE" << "\n";
						}
						int viejo = nro_jugador_mas_cercano;
						nro_jugador_mas_cercano = jugador_mas_cercano();
						if(viejo != nro_jugador_mas_cercano){
							cout<< "cambio el mas cercano de " << viejo << " a " << nro_jugador_mas_cercano << "\n";
						}
						belcebu->termino_ronda(equipo);
						
					}

					//cout << "Me fuí. Soy el: " << nro_jugador << "\n";
					this->tablero.unlock();
					break;
				}
			case(USTEDES):
				{ // SHORTEST pero que el jugador mas cercano se pueda mover 1 vez mas cada turno.

					this->tablero.lock(); 
					if(equipo == ROJO){
						sem_wait(&(belcebu->turno_rojo));
						sem_post(&(belcebu->turno_rojo));
					}
					else{
						sem_wait(&(belcebu->turno_azul));
						sem_post(&(belcebu->turno_azul));
					}

					
					
					this->nro_jugador_mas_cercano = jugador_mas_cercano();
					
					if(nro_jugador == nro_jugador_mas_cercano  && !(belcebu->termino_juego())){
						pos_actual = posiciones[nro_jugador];
						//cout << "soy el petiso" << "\n";
						direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
						direccion direc_nueva = direccion_proxiam_posicion(pos_actual, direc_deseada);
					
						if (belcebu->se_puede_mover(pos_actual, direc_nueva)){
							belcebu->mover_jugador(direc_nueva, nro_jugador);
							this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
						}else{
							cout << "ESTO ES IMPOSIBLE" << "\n";
							belcebu->jugadores_movidos++;
						}
						int viejo = nro_jugador_mas_cercano;
						nro_jugador_mas_cercano = jugador_mas_cercano();
						if(viejo != nro_jugador_mas_cercano){
							cout<< "cambio el mas cercano de " << viejo << " a " << nro_jugador_mas_cercano << "\n";
						}

						cant_veces_mov_ustedes++;

						if(cant_veces_mov_ustedes == cant_mov_ustedes || belcebu->termino_juego()){
							cant_veces_mov_ustedes = 0;
							cant_mov_ustedes++;
							belcebu->termino_ronda(equipo);
						}			
					}
					this->tablero.unlock();

					break;
				}
		}
		
	}
	if(strat == RR){
			for(int i = 0; i < cant_jugadores;i++){
				sem_post(&orden_jugadores_rr[i]);
			}
	}
	//cout << "me fui, soy de equipo: " << equipo <<"\n";
}

Equipo::Equipo(gameMaster *belcebu, color equipo, 
	estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones) {
	this->belcebu = belcebu;
	this->equipo = equipo;
	this->contrario = (equipo == ROJO)? AZUL: ROJO;
	this->bandera_contraria = (equipo==ROJO)? BANDERA_AZUL: BANDERA_ROJA;
	this->strat = strat;
	this->quantum = quantum;
	if(strat == RR) {
		quantum_restante = quantum;
	} else {
		quantum_restante = 0;
	}
	this->cant_jugadores = cant_jugadores;
	this->posiciones = posiciones;
	pos_bandera_contraria = make_pair(-1,-1);

	this->belcebu->strat = strat;

	// Caso SECUENCIAL
	jugadores_movidos_esta_ronda = 0; 
	vector<bool> v(cant_jugadores,false);
	this->se_movio_jugador = v;

	// Caso RR 
	this->orden_jugadores_rr = vector<sem_t>(this->cant_jugadores);
    for (int i = 0; i < this->cant_jugadores; i++) {
        sem_init(&this->orden_jugadores_rr[i], 0, 0);
    }
	sem_post(&orden_jugadores_rr[0]);

	// Caso SHORTEST
	this->nro_jugador_mas_cercano = -1;

	// Caso USTEDES
	cant_mov_ustedes = 1; 
}

void Equipo::comenzar() {
	//this->bandera_contraria_encontrada.unlock();
	//this->tablero.unlock();
	this->bandera_contraria_encontrada.lock();

	for(int i=0; i < cant_jugadores; i++) {
		jugadores.emplace_back(thread(&Equipo::jugador, this, i)); 
	}


}

void Equipo::terminar() {
	for(auto &t:jugadores){
		t.join();
	}	
}

void Equipo::buscar_bandera_contraria(int nro_jugador) {
	int tam_x = belcebu->getTamx(); 
	int tam_y = belcebu->getTamy();
	int recorrido_de_casilleros = (tam_x * tam_y) / cant_jugadores;
	int comienzo = nro_jugador * recorrido_de_casilleros;
	int fin = (nro_jugador + 1) * recorrido_de_casilleros; 
	
	for(int i = comienzo; i < fin; i++){
		if(equipo == ROJO){
			if (belcebu->bandera_azul(make_pair(i/tam_y, i%tam_y))){
				cout << "encontre la bandera azul en " << i/tam_y << " " << i%tam_y << "\n";
				pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
				bandera_contraria_encontrada.unlock();
				return;
			}
		} else {
			if (belcebu->bandera_roja(make_pair(i/tam_y, i%tam_y))){
				pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
				bandera_contraria_encontrada.unlock();
				return; 
			}
		}
	}

	// Caso en el que la division no es exacta, no se encontro el la bandera contraria
	// y el ultimo jugador tiene que recorrer mas casilleros. 
	if (nro_jugador == cant_jugadores -1 && pos_bandera_contraria == make_pair(-1,-1)){
		for(int i = fin; i < tam_x * tam_y; i++){
			if(equipo == ROJO){
				if (belcebu->bandera_azul(make_pair(i/tam_y, i%tam_y))){
					pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
					bandera_contraria_encontrada.unlock();
					return;
				}
			} else {
				if (belcebu->bandera_roja(make_pair(i/tam_y, i%tam_y))){
					pos_bandera_contraria = make_pair(i/tam_y, i%tam_y);
					bandera_contraria_encontrada.unlock();
					return; 
				}
			}
		}	
	}

}

direccion Equipo::direccion_proxiam_posicion(coordenadas posActual, direccion direc_deseada) {
	direccion direc_nueva = direc_deseada;
	//cout << "me quiero mover a: " << direc_deseada << "\n";
	direccion opuesta = (direc_deseada == ARRIBA) ? ABAJO : (direc_deseada == ABAJO) ? ARRIBA : (direc_deseada == IZQUIERDA) ? DERECHA : IZQUIERDA; //buen copilot
	
	assert(direc_deseada < 5);
	//cout << "estoy en " << posActual.first << " " << posActual.second << "\n";

	direccion planB = apuntar_a(make_pair(posActual.first, pos_bandera_contraria.second), pos_bandera_contraria);
	//cout << "planB: " << planB << "\n";
	int random = rand() % 2;
	if(random){
		if(belcebu->se_puede_mover(posActual, planB)) {
			//cout << "me puedo mover a: " << planB << "\n";
			return planB;
		}
		if(belcebu->se_puede_mover(posActual, direc_nueva)) {
			//cout << "me puedo mover a: " << direc_nueva << "\n";
			return direc_nueva;
		}
	}else{
		if(belcebu->se_puede_mover(posActual, direc_nueva)) {
			//cout << "me puedo mover a: " << direc_nueva << "\n";
			return direc_nueva;
		}
		if(belcebu->se_puede_mover(posActual, planB)) {
			//cout << "me puedo mover a: " << planB << "\n";
			return planB;
		}
	}

	//int numero = rand() % 2 + 1;
	//int evil_numero = (numero == 1) ? 2 : 1;
	direc_nueva = (direccion)((direc_deseada + 1) % 4);
	if(belcebu->se_puede_mover(posActual, direc_nueva) && direc_nueva != opuesta) {
		//cout << "me puedo mover a: " << direc_nueva << "\n";
		return direc_nueva;
	}
	direc_nueva = (direccion)((direc_deseada + 2) % 4);
	if(belcebu->se_puede_mover(posActual, direc_nueva) && direc_nueva != opuesta) {
		//cout << "me puedo mover a: " << direc_nueva << "\n";
		return direc_nueva;
	}
	
	if(belcebu->se_puede_mover(posActual, opuesta)) {
		//cout<< "esto no me conviene" << "\n";
		//cout << "me puedo mover a: " << opuesta << "\n";
		return opuesta;
	}

	return direc_deseada;
}

int Equipo::jugador_mas_cercano() {
	int distancia_mas_cercana = belcebu->getTamx() + belcebu->getTamy();
	int nro_jugador_mas_cercano = -1;
	
	for(int i = 0; i < posiciones.size(); i++) {
		int distancia_jugador_bandera = belcebu->distancia(posiciones[i], this->pos_bandera_contraria);
		if(distancia_jugador_bandera < distancia_mas_cercana) {
			distancia_mas_cercana = distancia_jugador_bandera;
			nro_jugador_mas_cercano = i; // Checkear qué atributo de jugador es su id. 
		}
	}

	return nro_jugador_mas_cercano;
}

