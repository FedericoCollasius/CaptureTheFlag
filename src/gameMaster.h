#ifndef GAMEMASTER_H
#define GAMEMASTER_H
#include <tuple>
#include <cstdio>
#include <vector>
#include <mutex>
#include <semaphore.h>
#include "definiciones.h"
#include "config.h"

using namespace std;

class gameMaster {
private:
    // Atributos Privados
    int nro_ronda = 0;
    int x, y, jugadores_por_equipos;
    vector<vector<color>> tablero;
    vector<coordenadas> pos_jugadores_azules, pos_jugadores_rojos;
    coordenadas pos_bandera_roja, pos_bandera_azul;
    color turno;
    
    uint jugadores_movidos = 0;

    // Variables de sincronizacion
    mutex permiso_para_jugar;

    // Métodos privados
    color obtener_coordenadas(coordenadas coord);
    void mover_jugador_tablero(coordenadas pos_anterior, coordenadas pos_nueva, color colorEquipo);
 
public:
    // Atributos públicos
    gameMaster(Config config);
    void termino_ronda(color equipo); 
    int mover_jugador(direccion dir, int nro_jugador);
    color ganador = INDEFINIDO;

    // Esta estaba privadas, ver si esta bien ponerlas aca
    /**/ 
    estrategia strat;
    /**/
    
    // Métodos públicos
    bool termino_juego();
	int getTamx();
	int getTamy();
    static int distancia(coordenadas pair1, coordenadas pair2);
    sem_t turno_rojo, turno_azul; // FIXME: Al principio necesito entrar como azul, luego puedo hacerlo por el método termino_ronda....
    color en_posicion(coordenadas coord);
    bool es_posicion_valida(coordenadas pos);
    bool es_color_libre(color color_tablero);
    coordenadas proxima_posicion(coordenadas anterior, direccion movimiento); // Calcula la proxima posición a moverse 


    void dibujame();
    void aumentar_jugadores_movidos();	
    bool bandera_roja(coordenadas cord);
    bool bandera_azul(coordenadas cord);
    bool se_puede_mover(coordenadas pos_nueva, direccion direc_nueva);
    color de_quien_es_el_turno();
};

#endif // GAMEMASTER_H
