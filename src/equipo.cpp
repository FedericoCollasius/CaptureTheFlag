#include "equipo.h"
#include <unistd.h>
#include <atomic>
#include <semaphore.h>
#include <assert.h> 


direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
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
	coordenadas pos_actual; 

	int cant_veces_mov_ustedes = 0;
	
	while(!this->belcebu->termino_juego()) {
		if(belcebu->termino_juego()){
			if(strat == RR){
				for(int i = 0; i < cant_jugadores;i++){
					sem_post(&orden_jugadores_rr[i]);
				}
			}
			break;
		}
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
						tablero.unlock();
						break;
					}
					
					pos_actual = posiciones[nro_jugador];
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_nueva = direccion_proxiam_posicion(pos_actual, direc_deseada);
					if (belcebu->se_puede_mover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}else{
						belcebu->aumentar_jugadores_movidos();
					}

					this->se_movio_jugador[nro_jugador] = true;
					jugadores_movidos_esta_ronda++;
				
					if(jugadores_movidos_esta_ronda == this->cant_jugadores || belcebu->termino_juego()){ 
						jugadores_movidos_esta_ronda = 0;
						this->se_movio_jugador = vector<bool>(this->cant_jugadores, false);
						belcebu->termino_ronda(equipo);
					}
					
					this->tablero.unlock();
					break;
				}
			
			case(RR):
				{
					sem_wait(&orden_jugadores_rr[nro_jugador]);
				
					if(equipo == ROJO){
						sem_wait(&(belcebu->turno_rojo));
						sem_post(&(belcebu->turno_rojo));
					}
					else{
						sem_wait(&(belcebu->turno_azul));
						sem_post(&(belcebu->turno_azul));
					}
					
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
								sem_post(&orden_jugadores_rr[i]);
							}
							belcebu->termino_ronda(equipo);
							break;
						}
						if(this->quantum_restante == 0){
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

						pos_actual = posiciones[nro_jugador];
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

					this->tablero.unlock();
					break;
				}
			case(USTEDES):
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
					
					if(nro_jugador == nro_jugador_mas_cercano  && !(belcebu->termino_juego())){
						pos_actual = posiciones[nro_jugador];
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

	this->belcebu->setear_strat(strat);

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
	direccion opuesta = (direc_deseada == ARRIBA) ? ABAJO : (direc_deseada == ABAJO) ? ARRIBA : (direc_deseada == IZQUIERDA) ? DERECHA : IZQUIERDA; //buen copilot
	
	assert(direc_deseada < 5);
	
	direccion planB = apuntar_a(make_pair(posActual.first, pos_bandera_contraria.second), pos_bandera_contraria);

	int random = rand() % 2;
	if(random){
		if(belcebu->se_puede_mover(posActual, planB)) {
			return planB;
		}
		if(belcebu->se_puede_mover(posActual, direc_nueva)) {
			return direc_nueva;
		}
	}else{
		if(belcebu->se_puede_mover(posActual, direc_nueva)) {
			return direc_nueva;
		}
		if(belcebu->se_puede_mover(posActual, planB)) {
			return planB;
		}
	}

	direc_nueva = (direccion)((direc_deseada + 1) % 4);
	if(belcebu->se_puede_mover(posActual, direc_nueva) && direc_nueva != opuesta) {
		return direc_nueva;
	}

	direc_nueva = (direccion)((direc_deseada + 2) % 4);
	if(belcebu->se_puede_mover(posActual, direc_nueva) && direc_nueva != opuesta) {
		return direc_nueva;
	}

	if(belcebu->se_puede_mover(posActual, opuesta)) {
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
			nro_jugador_mas_cercano = i; 
		}
	}

	return nro_jugador_mas_cercano;
}

