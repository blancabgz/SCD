// -----------------------------------------
// BLANCA ABRIL GONZÁLEZ
// SISTEMAS CONCURRENTES Y DISTRIBUIDOS (2A)
// -----------------------------------------

// g++ -std=c++11 -pthread -o fumadores fumadores.cpp Semaphore.cpp Semaphore.h

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;



// variables compartidas + Semáforos
Semaphore mostr_vacio = 1; // Semaforo para indicar si el mostrador esta vacio o no
Semaphore ingr_disp[3]={0,0,0}; // Semaforo para controlar si el ingrediente esta disponible(1) o no (0)

const int NUM_FUMADORES = 3; // fumadores

mutex mtx;


//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

// ---------------------------------------------------------------------
// funcion Producir() que produce un numero aleatorio '0,1,2' con restraso
// aleatorio

  int producir(){
    // genera un ingrediente aleatorio
    int ingrediente_aleat(aleatorio<0,2>());
    // espera un tiempo aleatorio
    this_thread::sleep_for( chrono::milliseconds( aleatorio<50,200>()));
    // devuelves el ingrediente generado
    return ingrediente_aleat;
  }

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(  )
{
  int ingrediente;
  while (true) {
    sem_wait(mostr_vacio);

    // genera un ingrediente aleatorio y además hace un retraso aleatorio
    ingrediente = producir();

    mtx.lock();
      cout << "Estanquero: Ha producido ingrediente " << ingrediente << endl << flush;
    mtx.unlock();
    // el ingrediente pasa a estar disponible
    sem_signal(ingr_disp[ingrediente]);

  }
}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<50,200>() );

   // informa de que comienza a fumar
    mtx.lock();
    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl << flush;
    mtx.unlock();
   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar
    mtx.lock();
    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl << flush;
    mtx.unlock();

}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true ){
      // espera hasta que el ingrediente esta disponible
      sem_wait(ingr_disp[num_fumador]);
      mtx.lock();
        cout << "Fumador: retirado ingrediente: " << num_fumador << endl << flush;
      mtx.unlock();
      sem_signal(mostr_vacio);
      fumar(num_fumador);

   }
}

//----------------------------------------------------------------------

int main()
{
   thread hebraEstanquero(funcion_hebra_estanquero);
   thread fumadores[3];
   for(int i = 0; i<3; i++){
      fumadores[i] = thread (funcion_hebra_fumador,i);
   }
   for(int j=0; j<3; j++ ){
     fumadores[j].join();
   }
    hebraEstanquero.join();
}
