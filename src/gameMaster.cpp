#include <sys/unistd.h>
#include <assert.h>     /* assert */
#include "gameMaster.h"

bool gameMaster::es_posicion_valida(coordenadas pos) {
	return (pos.first > 0) && (pos.first < x) && (pos.second > 0) && (pos.second < y);
}

bool gameMaster::es_color_libre(color color_tablero){
    return color_tablero == VACIO || color_tablero == INDEFINIDO;
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
    for (int i = 0; i < x; +quantum+i) {
        tablero[i].resize(y);
        fill(tablero[i].begin(), tablero[i].end(), VACIO);
    }
    

    for(auto &coord : config.pos_rojo){
        assert(es_color_libre(tablero[coord.first][coord.second])); //Compruebo que no haya otro jugador en esa posicion
        tablero[coord.first][coord.second] = ROJO; // guardo la posicion
    }

    for(auto &coord : config.pos_azul){
        assert(es_color_libre(tablero[coord.first][coord.second]));
        tablero[coord.first][coord.second] = AZUL;
    }

    tablero[config.bandera_roja.first][config.bandera_roja.second] = BANDERA_ROJA;
    tablero[config.bandera_azul.first][config.bandera_azul.second] = BANDERA_AZUL;
	this->turno = ROJO;

    cout << "SE HA INICIALIZADO GAMEMASTER CON EXITO" << endl;
	
    // Insertar código que crea necesario de inicialización
/**/// acomodart semafores turno rojo turno azul
}

void gameMaster::mover_jugador_tablero(coordenadas pos_anterior, coordenadas pos_nueva, color colorEquipo){
    assert(es_color_libre(tablero[pos_nueva.first][pos_nueva.second]));
    tablero[pos_anterior.first][pos_anterior.second] = VACIO; 
    tablero[pos_nueva.first][pos_nueva.second] = colorEquipo;
}


int gameMaster::mover_jugador(direccion dir, int nro_jugador) {

/**/ //Falta ver si el jugador que se esta moviendo es del equipo que se tenga que mover??????????????????????????????????????????????????????????????

	// Chequear que la movida sea valida
/**/vector<coordenadas>* posiciones_equipo_actual = &(turno == ROJO ? pos_jugadores_rojos : pos_jugadores_azules);
/**/coordenadas posicion_vieja = (*posiciones_equipo_actual)[nro_jugador];
/**/coordenadas posicion_nueva = proxima_posicion(posicion_vieja, dir);

	// Que no se puedan mover 2 jugadores a la vez
/**/permiso_para_jugar.lock();
/**/assert(es_posicion_valida(posicion_nueva));
/**/mover_jugador_tablero(posicion_vieja, posicion_nueva, turno);
/**/jugadores_movidos++;
/**/permiso_para_jugar.unlock();

    // setear la variable ganador
/**/coordenadas bandera_contraria = (turno == ROJO ? pos_bandera_azul : pos_bandera_roja);
/**/if (posicion_nueva == bandera_contraria) { ganador = turno; }  // Asumimos que tiene que sentarse sobre la bandera, no acercarse

    // Devolver acorde a la descripción
/**/return (termino_juego() ? 0 : nro_ronda);
}


void gameMaster::termino_ronda(color equipo) {
	// FIXME: Hacer chequeo de que es el color correcto que está llamando
/**/assert(turno == equipo);
	// FIXME: Hacer chequeo que hayan terminado todos los jugadores del equipo o su quantum (via mover_jugador)
/**/switch(strat) {
		//SECUENCIAL,RR,SHORTEST,USTEDES
		case(SECUENCIAL):
/**/		assert(jugadores_movidos == jugadores_por_equipos);
			break;
		case(RR):
/**/		// assert(jugadores_movidos == quantum);
/**/		// Lo asevera el jugador antes de terminar
			break;
		case(SHORTEST):
/**/		assert(jugadores_movidos == 1);
			break;
		case(USTEDES):
/**/		assert(jugadores_movidos == 1);
			break;
		default:
/**/		assert(false);
	}

	// Cambiar el turno
	turno = (color)(ROJO+AZUL-turno);
/**/(turno == ROJO ? turno_rojo : turno_azul).unlock();
	jugadores_movidos=0;
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
	}//
	// ...
	//
}
	return anterior; // está haciendo una copia por constructor
}
