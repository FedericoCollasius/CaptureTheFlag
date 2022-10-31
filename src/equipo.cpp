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


	direccion planB = apuntar_a(make_pair(posActual.first, pos_bandera_contraria.second), pos_bandera_contraria);
	//cout << "planB: " << planB << endl;
	int random = rand() % 2;
	if(random){
		if(belcebu->sePuedeMover(posActual, planB)) {
			//cout << "me puedo mover a: " << planB << endl;
			return planB;
		}
		if(belcebu->sePuedeMover(posActual, direc_nueva)) {
			//cout << "me puedo mover a: " << direc_nueva << endl;
			return direc_nueva;
		}
	}else{
		if(belcebu->sePuedeMover(posActual, direc_nueva)) {
			//cout << "me puedo mover a: " << direc_nueva << endl;
			return direc_nueva;
		}
		if(belcebu->sePuedeMover(posActual, planB)) {
			//cout << "me puedo mover a: " << planB << endl;
			return planB;
		}
	}


	//int numero = rand() % 2 + 1;
	//int evil_numero = (numero == 1) ? 2 : 1;
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
		//cout<< "esto no me conviene" << endl;
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
	coordenadas pos_actual; // = posiciones[nro_jugador];//INNECESARIO?
	quantum_restante = 0;
	if(strat == RR) {
		quantum_restante = quantum;
	}
	
	while(!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
		//cout << "wtf amigooo" << endl;
		if(equipo == ROJO){
			sem_wait(&(belcebu->turno_rojo));
			sem_post(&(belcebu->turno_rojo));
		}
		else{
			sem_wait(&(belcebu->turno_azul));
			sem_post(&(belcebu->turno_azul));
		}
		if(belcebu->termino_juego()){
			if(strat == RR){
				//cout << "El equipo " << equipo << " libera" << endl;
				for(int i = 0; i < cant_jugadores;i++){
					//cout << "liberamos" << endl;
					sem_post(&orden_jugadores_rr[i]);
				}
			}
			break;
		}
		//cout << "el petiso es: " << nro_jugador_mas_cercano << endl;
		switch(this->strat) {
			//SECUENCIAL,RR,SHORTEST,USTEDES
			case(SECUENCIAL): // No espera a nadie. 
				{	
					//if(se_movio_jugador[nro_jugador]){
					//	//sleep(5);
					//	break;
					//}
					this->tablero.lock();
					if(se_movio_jugador[nro_jugador] || belcebu->turno != equipo){
						//cout << "ups equipo: "<< nro_jugador << endl;
						tablero.unlock();
						//sleep(5);
						break;
					}
					pos_actual = posiciones[nro_jugador];
					//cout << jugadores_movidos_esta_ronda << " "<< cant_jugadores << " " <<nro_jugador<< endl;
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
					cout << "direccion nueva: " << direc_nueva << " jugador " << nro_jugador << " en " << pos_actual.first << " " << pos_actual.second << endl;
					if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						//this->se_movio_jugador[nro_jugador] = true;
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}else{
						cout << "AYUDAAAAA, NO ME PUEDO MOVERRREERE" << endl;
					}

					this->se_movio_jugador[nro_jugador] = true;
					jugadores_movidos_esta_ronda++;
					//cout << jugadores_movidos_esta_ronda << " "<< cant_jugadores << endl;
					if(jugadores_movidos_esta_ronda == this->cant_jugadores){
						
						if(equipo == ROJO){
							cout<< "Termino ronda " << belcebu->nro_ronda << " equipo " << "ROJO" << endl;
						}
						else{
							cout<< "Termino ronda " << belcebu->nro_ronda << " equipo " << "AZUL" << endl;
						}
						jugadores_movidos_esta_ronda = 0;
						this->se_movio_jugador = vector<bool>(this->cant_jugadores, false);
						belcebu->termino_ronda(equipo);
					}
					
					//sleep(1);
					//cout<<"ya no me toca" << endl;
					this->tablero.unlock();
					break;
				}
			
			case(RR): // Espera a que le toque nro = movidos. 
				{
				//cout << "i shouldnt be here" << endl;
				sem_wait(&orden_jugadores_rr[nro_jugador]);
				if(belcebu->turno != equipo){
					//cout << "no me toca" << endl;
					sem_post(&orden_jugadores_rr[nro_jugador]);
					break;
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
					direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
				
					if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}
					this->quantum_restante--;
					if(belcebu->termino_juego()){
						for(int i = 0; i < cant_jugadores;i++){
							//cout << "liberamos" << endl;
							sem_post(&orden_jugadores_rr[i]);
						}
						belcebu->termino_ronda(equipo);
						break;
					}
					if(this->quantum_restante == 0){
						//cout << "terminamos muchachos" << endl;
						this->quantum_restante = this->quantum;
						belcebu->termino_ronda(equipo);
						sem_post(&orden_jugadores_rr[0]);
						//if(belcebu->termino_juego()){ no se si esto va
						//	for(int i = 0; i < cant_jugadores;i++){
						//		sem_post(&orden_jugadores_rr[i]);
						//	}
						//	break;
						//}
					}else{
						sem_post(&orden_jugadores_rr[(nro_jugador+1)%cant_jugadores]);
					}	
				}
				
				break;
				}
				
			case(SHORTEST):
				{ 
					this->tablero.lock();

					if(nro_jugador == nro_jugador_mas_cercano){
						pos_actual = posiciones[nro_jugador];
						cout << "soy el petiso" << endl;
						direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
						direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
					
						if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
							belcebu->mover_jugador(direc_nueva, nro_jugador);
							this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
						}else{
							cout << "ESTO ES IMPOSIBLE" << endl;
						}
						int viejo = nro_jugador_mas_cercano;
						nro_jugador_mas_cercano = jugador_mas_cercano();
						if(viejo != nro_jugador_mas_cercano){
							cout<< "cambio el mas cercano de " << viejo << " a " << nro_jugador_mas_cercano << endl;
						}
						belcebu->termino_ronda(equipo);
						
					}else{
						//cout << "no soy el petiso, yo soy el " << nro_jugador << endl;
						//sleep(1);
					}
					this->tablero.unlock();
					break;
				}
			case(USTEDES):
				{//
					{
					//if(equipo != belcebu->turno){
					//	//this->tablero.unlock();
					//	break;
					//}
					//this->tablero.lock();
					//if(nro_jugador == nro_jugador_mas_lejano && equipo == belcebu->turno){
					//	pos_actual = posiciones[nro_jugador];
					//	cout << "soy el grandote" << endl;
					//	direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					//	direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
					//
					//	if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
					//		belcebu->mover_jugador(direc_nueva, nro_jugador);
					//		this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					//	}else{
					//		cout << "ESTO ES IMPOSIBLE" << endl;
					//	}
					//	
					//	belcebu->termino_ronda(equipo);
					//	int viejo = nro_jugador_mas_lejano;
					//	nro_jugador_mas_lejano = jugador_mas_lejano();
					//	if(viejo != nro_jugador_mas_lejano){
					//		cout<< "cambio el mas cercano de " << viejo << " a " << nro_jugador_mas_lejano << endl;
					//	}
					//	
					//}else{
					//	//cout << "no soy el petiso, yo soy el " << nro_jugador << endl;
					//	//sleep(1);
					//}
					//this->tablero.unlock();
					}
					//cout << "jugador " << nro_jugador << " equipo " << equipo << endl;
					tablero.lock();
					if(movio_ultimo == nro_jugador){
						tablero.unlock();
						break;
					}else{
						movio_ultimo = nro_jugador;
					}
					if(belcebu->turno != equipo){
						tablero.unlock();
						break;
					}
					pos_actual = posiciones[nro_jugador];
					direccion direc_deseada = apuntar_a(pos_actual, pos_bandera_contraria);
					direccion direc_nueva = direccion_proxima_posicion(pos_actual, direc_deseada);
				
					if (belcebu->sePuedeMover(pos_actual, direc_nueva)){
						belcebu->mover_jugador(direc_nueva, nro_jugador);
						this->posiciones[nro_jugador] = belcebu->proxima_posicion(pos_actual, direc_nueva);
					}else{
						cout << "ESTO ES IMPOSIBLE" << endl;
					}
					contador_pasos--;
					if(contador_pasos == 0){
						contador_pasos = 5;
						belcebu->termino_ronda(equipo);
					}
					tablero.unlock();

					break;
				}
			default:
				{
					cout << "i shouldnt be here" << endl;
					assert(true);
					assert(false);
					break;
				}
		}
		
		// Termino ronda ? Recordar llamar a belcebu...
		// OJO. Esto lo termina un jugador... 
		//
		// ...
		//
	}
	cout << "me fui, soy de equipo: " << equipo <<endl;
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
	nro_jugador_mas_lejano = -1;

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
	}else if(this->strat == USTEDES) {
		this->nro_jugador_mas_lejano = jugador_mas_lejano();
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
		pos_bandera_contraria = make_pair(15,5);
	} else {//seteo valores por simplicidad, no me hinchen
		pos_bandera_contraria = make_pair(13,13);
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
	//FEDERICO LA RE CALCADA CONCHA DE TU MADRE HIJO DE PUTA
	//for(auto &t:this->posiciones) {
	//	int distancia_jugador_bandera = belcebu->distancia(t, this->pos_bandera_contraria);
	//	if(distancia_jugador_bandera < distancia_mas_cercana) {
	//		distancia_mas_cercana = distancia_jugador_bandera;
	//		nro_jugador_mas_cercano = t.first; // Checkear que atributo de jugador es su id. 
	//	}
	//}

	for(int i = 0; i < posiciones.size(); i++) {
		int distancia_jugador_bandera = belcebu->distancia(posiciones[i], this->pos_bandera_contraria);
		if(distancia_jugador_bandera < distancia_mas_cercana) {
			distancia_mas_cercana = distancia_jugador_bandera;
			nro_jugador_mas_cercano = i; // Checkear que atributo de jugador es su id. 
		}
	}



	return nro_jugador_mas_cercano;
}
int Equipo::jugador_mas_lejano() {
	int distancia_mas_lejana = 0;
	int nro_jugador_mas_lejano = -1;
	for(int i = 0; i < posiciones.size(); i++) {
		int distancia_jugador_bandera = belcebu->distancia(posiciones[i], this->pos_bandera_contraria);
		if(distancia_jugador_bandera > distancia_mas_lejana) {
			distancia_mas_lejana = distancia_jugador_bandera;
			nro_jugador_mas_lejano = i; // Checkear que atributo de jugador es su id. 
		}
	}
	return nro_jugador_mas_lejano;
}