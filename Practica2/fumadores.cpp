// -----------------------------------------
// BLANCA ABRIL GONZÁLEZ
// SISTEMAS CONCURRENTES Y DISTRIBUIDOS (2A)
// -----------------------------------------


#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"
#include "HoareMonitor.h"

using namespace std ;
using namespace HM ;


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

// variables compartidas
const int NUM_FUMADORES = 3; // fumadores
mutex mtx;



// --------------------------------------------------------------------
// monitor Estanco + variables condicion

class Estanco : public HoareMonitor
{
private:
  int ingrediente_mostrador; // ingrediente disponible en el mostrador
  CondVar estanquero_espera; // hebra estanquero espera
  CondVar fumadores[NUM_FUMADORES]; //hebra fumadores
public:
  Estanco();
  void ponerIngrediente(int ingre); // pone el ingrediente en el ingrediente_mostrador
  void obtenerIngrediente(int ingre); // fumador espera a que su ingrediente este listo, de mientras se encuentra bloqueado
  void esperarRecogidaIngrediente(); // espera hasta que el fumador coge el ingrediente
};

// iniciar hebra estanquero y hebras fumadores
Estanco::Estanco(){
  ingrediente_mostrador = -1; // el mostrador esta vacio
  estanquero_espera = newCondVar();
  for (int i = 0; i < NUM_FUMADORES; i++) {
    fumadores[i] = newCondVar();
  }
}


void Estanco::ponerIngrediente(int ingre){
  while(ingrediente_mostrador != -1){
    estanquero_espera.wait();
  }

  ingrediente_mostrador = ingre; // se coloca en el mostrador el ingrediente creado
  mtx.lock();
  cout << "El estanquero ha colocado el ingrediente " << ingre << " en el mostrador" << endl;
  mtx.unlock();
  fumadores[ingre].signal(); // desbloquea hebra fumador del ingrediente
}

void Estanco::obtenerIngrediente(int ingre){
  if(ingrediente_mostrador != ingre){
    fumadores[ingre].wait();
  }

    mtx.lock();
    cout << "El fumador " << ingre << " ha recogido su ingrediente para comenzar a fumar " << endl;
    mtx.unlock();
    ingrediente_mostrador = -1;
    estanquero_espera.signal();

}

void Estanco::esperarRecogidaIngrediente(){
    while(ingrediente_mostrador != -1){
      //mtx.lock();
      //cout << "El estanquero esta esperando a que se retire el ingrediente " << endl;
      //mtx.unlock();
      estanquero_espera.wait();
  }
}

// Procedimiento estanquero crea un ingrediente aleatorio

int producirIngrediente(){
  // genera un ingrediente aleatorio
  int ingrediente_aleat(aleatorio<0,2>());
  // espera un tiempo aleatorio
  mtx.lock();
  cout << "El estanquero comienza la preparacion del ingrediente " << ingrediente_aleat << endl;
  mtx.unlock();
  this_thread::sleep_for( chrono::milliseconds( aleatorio<50,200>()));
  // devuelves el ingrediente generado
  return ingrediente_aleat;
}
//

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(MRef<Estanco> estanco)
{
  int ingrediente;
  while (true) {
    // genera un ingrediente aleatorio y además hace un retraso aleatorio
    ingrediente = producirIngrediente();
    // coloca el ingrediente en el mostrador
    estanco->ponerIngrediente(ingrediente);
    // espera a que mostrador quede vacio
    estanco->esperarRecogidaIngrediente();
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
void funcion_hebra_fumador(int num_fumador, MRef<Estanco> estanco)
{
   while( true ){
    estanco->obtenerIngrediente(num_fumador);
    fumar(num_fumador);
   }
}

//----------------------------------------------------------------------

int main()
{
  MRef<Estanco> estanco = Create<Estanco> ();
  thread estanquero_espera;
  thread fumadores[NUM_FUMADORES];
  for(int i=0; i< NUM_FUMADORES; i++){
    fumadores[i] = thread(funcion_hebra_fumador,estanco,i);
  }
  estanquero_espera = thread(funcion_hebra_estanquero,estanco);

  for(int i=0; i < NUM_FUMADORES; i++){
    fumadores[i].join();
  }

  estanquero_espera.join();
}
