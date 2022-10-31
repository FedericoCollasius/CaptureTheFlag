#include <sys/unistd.h>
#include <assert.h>     /* assert */
#include "gameMaster.h"
 #include <sys/wait.h>


bool gameMaster::es_posicion_valida(coordenadas pos) {
	if(pos.first <= 0 || pos.first >= x || pos.second <= 0 || pos.second >= y) {
		// cout << "Posicion invalida" << "\n";
	}
	// Los bordes no cuentan?
	return (pos.first > 0) && (pos.first < x) && (pos.second > 0) && (pos.second < y);
	
}

bool gameMaster::es_color_libre(color color_tablero){
	bool bandera;
	if(turno == ROJO){
		bandera = (color_tablero == BANDERA_AZUL);
	}
	else{
		bandera = (color_tablero == BANDERA_ROJA);
	}
    return color_tablero == VACIO || color_tablero == INDEFINIDO || bandera;
}

color gameMaster::en_posicion(coordenadas coord) {
	return tablero[coord.first][coord.second];
}

int gameMaster::getTamx() {
	return x;
}

int gameMaster::getTamy() {
	return y;
}

int gameMaster::distancia(coordenadas c1, coordenadas c2) { 
    return abs(c1.first-c2.first)+abs(c1.second-c2.second);
}

gameMaster::gameMaster(Config config) {
	assert(config.x>0); 
	assert(config.y>0); // Tamaño adecuado del tablero

    this->x = config.x;
	this->y = config.y;
	// cout << "tablero de tamaño " << x << "x" << y << "\n";

	assert((config.bandera_roja.first == 1)); // Bandera roja en la primera columna
	assert(es_posicion_valida(config.bandera_roja)); // Bandera roja en algún lugar razonable

	assert((config.bandera_azul.first == x-1)); // Bandera azul en la primera columna
	assert(es_posicion_valida(config.bandera_azul)); // Bandera roja en algún lugar razonable

	assert(config.pos_rojo.size() == config.cantidad_jugadores);
	assert(config.pos_azul.size() == config.cantidad_jugadores);
	for(auto &coord : config.pos_rojo) {
		assert(es_posicion_valida(coord)); // Posiciones validas rojas
	}		

	for(auto &coord : config.pos_azul) {
		assert(es_posicion_valida(coord)); // Posiciones validas rojas
	}		

	
	this->jugadores_por_equipos = config.cantidad_jugadores;
	this->pos_bandera_roja = config.bandera_roja;
	this->pos_bandera_azul = config.bandera_azul;
    this->pos_jugadores_rojos = config.pos_rojo;
    this->pos_jugadores_azules = config.pos_azul;
	// Seteo tablero
	tablero.resize(x);
    for (int i = 0; i < x; ++i) {
        tablero[i].resize(y);
        fill(tablero[i].begin(), tablero[i].end(), VACIO);
    }
    

    for(auto &coord : config.pos_rojo){
		// cout << "Posicion roja: " << coord.first << " " << coord.second << "\n";
        assert(es_color_libre(tablero[coord.first][coord.second])); //Compruebo que no haya otro jugador en esa posicion
        tablero[coord.first][coord.second] = ROJO; // guardo la posicion
    }

    for(auto &coord : config.pos_azul){
		// cout << "Posicion azul: " << coord.first << " " << coord.second << "\n";
        assert(es_color_libre(tablero[coord.first][coord.second]));
        tablero[coord.first][coord.second] = AZUL;
    }

    tablero[config.bandera_roja.first][config.bandera_roja.second] = BANDERA_ROJA;
    tablero[config.bandera_azul.first][config.bandera_azul.second] = BANDERA_AZUL;
	this->turno = ROJO;
	// dibujame();
    // cout << "SE HA INICIALIZADO GAMEMASTER CON EXITO" << "\n";
    
	// Acomodar semaforos turno rojo turno azul.
	sem_init(&turno_azul, 0, 0);
	sem_init(&turno_rojo, 0, 1);
}

void gameMaster::mover_jugador_tablero(coordenadas pos_anterior, coordenadas pos_nueva, color colorEquipo){
	//cout<< "assert?? " << tablero[pos_nueva.first][pos_nueva.second] << " " << colorEquipo << " en " << pos_nueva.first << " " << pos_nueva.second <<   "\n";
    assert(es_color_libre(tablero[pos_nueva.first][pos_nueva.second]));
    tablero[pos_anterior.first][pos_anterior.second] = VACIO; 
    tablero[pos_nueva.first][pos_nueva.second] = colorEquipo;
}


int gameMaster::mover_jugador(direccion dir, int nro_jugador) {

	// Chequear que la movida sea valida
	coordenadas posicion_vieja = (turno == ROJO ? pos_jugadores_rojos[nro_jugador] : pos_jugadores_azules[nro_jugador]);
	coordenadas posicion_nueva = proxima_posicion(posicion_vieja, dir);

	// Que no se puedan mover 2 jugadores a la vez
	assert(es_posicion_valida(posicion_nueva));
	//cout<< "assert?? " << tablero[posicion_nueva.first][posicion_nueva.second] << " " << turno << " en " << posicion_nueva.first << " " << posicion_nueva.second <<
	// " soy " << nro_jugador  << " y estaba en " << posicion_vieja.first << " " << posicion_vieja.second<<"\n";
	// cout << posicion_vieja.first << " " << posicion_vieja.second << " == " << pos_actual.first << " " << pos_actual.second << "\n";
	mover_jugador_tablero(posicion_vieja, posicion_nueva, turno);
	// cout << "movi " << nro_jugador << "\n";
	(turno == ROJO ? pos_jugadores_rojos : pos_jugadores_azules)[nro_jugador] = posicion_nueva;
	jugadores_movidos++;
	// cout << "Jugadores por equipos:" << jugadores_por_equipos << "\n"; 
	// cout << "Jugadores movidos:" << jugadores_movidos << "\n"; 
	// permiso_para_jugar.unlock();

    // Setear la variable ganador
	// cout << "dibujame como a vos te gusta" << "\n";
	//dibujame();
	//sleep(0.5);
	coordenadas bandera_contraria = (turno == ROJO ? pos_bandera_azul : pos_bandera_roja);
	if (posicion_nueva == bandera_contraria) {
		// cout << "gane amigo" << "\n";	
		ganador = turno;
	}  // Asumimos que tiene que sentarse sobre la bandera, no acercarse

    // Devolver acorde a la descripción
	return (termino_juego() ? 0 : nro_ronda);
}


void gameMaster::termino_ronda(color equipo) {
	// cout << turno << " " << equipo << "\n";
	assert(turno == equipo);

	// FIXME: Hacer chequeo que hayan terminado todos los jugadores del equipo o su quantum (via mover_jugador)
	switch(strat) {
			case(SECUENCIAL):
				// cout << jugadores_movidos << " ?? " << jugadores_por_equipos << "\n";
				assert(jugadores_movidos == jugadores_por_equipos || termino_juego());
				break;
			case(RR):
				// assert(jugadores_movidos == quantum);
				// Lo asevera el jugador antes de terminar
				// Resetear semaforos y Q. 
				break;
			case(SHORTEST):
				assert(jugadores_movidos == 1);
				break;
			case(USTEDES):
				assert(jugadores_movidos == 5);
				break;
			default:
				assert(false);
		}

	// Cambiar el turno
	nro_ronda++;
	jugadores_movidos=0;
	if(nro_ronda > 10000) { ganador = EMPATE; }
	turno = (color)(ROJO+AZUL-turno);
	if(termino_juego()){
		sem_post(&(turno_rojo));
		sem_post(&(turno_azul));
	} else {
		sem_wait(&(turno == AZUL ? turno_rojo : turno_azul));
		sem_post(&(turno == ROJO ? turno_rojo : turno_azul)); // Libera al que le toque
	}
}

bool gameMaster::termino_juego() {
	return ganador != INDEFINIDO;
}

coordenadas gameMaster::proxima_posicion(coordenadas anterior, direccion movimiento) {
	// Calcula la proxima posición a moverse (es una copia) 
	switch(movimiento) {
		case(ARRIBA):
			anterior.second--; 
			break;

		case(ABAJO):
			anterior.second++;
			break;

		case(IZQUIERDA):
			anterior.first--;
			break;

		case(DERECHA):
			anterior.first++;
			break;
	}
	// cout << "proxima_posicion: " << anterior.first << " " << anterior.second << "\n";
	return anterior; // está haciendo una copia por constructor
}

bool gameMaster::bandera_azul(coordenadas cord){
	return (cord == pos_bandera_azul);
}
bool gameMaster::bandera_roja(coordenadas cord){
	return (cord == pos_bandera_roja);
}
void gameMaster::dibujame(){
	for(int i=0; i<x; i++){
		for(int j=0; j<y; j++){
			if(tablero[j][i] == 0){
				// cout << "b" << " ";
			}else if(tablero[j][i] == 1){
				// cout << "r" << " ";
			}else if(tablero[j][i] == 2){
				// cout << "?" << " ";
			}else if(tablero[j][i] == 3){
				// cout << "." << " ";
			}else if(tablero[j][i] == 4){
				// cout << "?" << " ";
			}else if(tablero[j][i] == 5){
				// cout << "R" << " ";
			}else if(tablero[j][i] == 6){
				// cout << "B" << " ";
			}
		}
		// cout << "\n";
	}
	for(int i=0; i<x; i++){
		// cout << "--";
	}
	// cout << "\n";
}

bool gameMaster::se_puede_mover(coordenadas pos_actual, direccion direc_nueva){
	 coordenadas pos_nueva = proxima_posicion(pos_actual, direc_nueva);
	 // cout << "posicion: " << pos_nueva.first << " " <<pos_nueva.second <<" posicion valida: " << es_posicion_valida(pos_nueva) << " color libre: " << es_color_libre(tablero[pos_nueva.first][pos_nueva.second]) << "\n";
	 // cout << tablero[pos_nueva.first][pos_nueva.second] << "\n";
	 return es_posicion_valida(pos_nueva) && es_color_libre(tablero[pos_nueva.first][pos_nueva.second]);
}

void gameMaster::aumentar_jugadores_movidos(){
	jugadores_movidos++;
}



