#include <iostream>
#include <thread>
#include <sys/unistd.h>
#include <vector>
#include <mutex>
#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"

#include <time.h>


using namespace std;

const estrategia strat = USTEDES;
const int quantum = 6;

enum { NS_PER_SECOND = 1000000000 };

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}


int main(){

    vector<string> vectorsito;
    unsigned long long  milis = 0;

    for (int i = 8; i >= 0; i--)
    {
        Config config (to_string(i) + ".csv");
        gameMaster belcebu = gameMaster(config);

        belcebu.strat = strat;//culpa peron
	    // Creo equipos (lanza procesos)
        //cout << "corriendo test " << i << endl;
        for(int j = 0; j < 40; j++){

	        Equipo rojo(&belcebu, ROJO, strat, config.cantidad_jugadores, quantum, config.pos_rojo);
	        Equipo azul(&belcebu, AZUL, strat, config.cantidad_jugadores, quantum, config.pos_azul);

            struct timespec start, finish, delta;
            clock_gettime(CLOCK_REALTIME, &start);

            rojo.comenzar();
            azul.comenzar();
	        rojo.terminar();
	        azul.terminar();	
            //belcebu.play();

            clock_gettime(CLOCK_REALTIME, &finish);
            sub_timespec(start, finish, &delta);
            milis += delta.tv_sec * 1000;
            milis += delta.tv_nsec / 1000000;

            //cout << "test " << i << " iteracion " << j << " tiempo: " << seconds << " segundos " << nanos << " nanosegundos" << endl;
        }

        //printf("%d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);
        milis /= 40;
        cout << "tarde con " << i+1 << " threads " << milis << " milisegundos"<< endl;
        milis = 0;

        //cout << "Bandera capturada por el equipo "<< belcebu.ganador << ". Felicidades!" << "\n";
    }
    //seconds = seconds/10000;
    //nanos = nanos/10000;
    //cout << "Tiempo promedio: " << seconds << "." << nanos << endl;
    //printf("%d.%.9ld\n", (int)seconds,nanos);
}

