// -----------------------------------------
// BLANCA ABRIL GONZÁLEZ
// SISTEMAS CONCURRENTES Y DISTRIBUIDOS (2A)
// EL PROBLEMA DEL BARBERO DURMIENTE
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

// ---------------------------------------------------------------------
// variables compartidas

int num_clientes = 4; // numero fijo de clientes que entran
mutex mtx;

// --------------------------------------------------------------------
// monitor Barberia + variables condicion

class Barberia : public HoareMonitor
{
private:
CondVar sala_espera_clientes, barbero, silla;

public:
Barberia();
void cortarPelo(int cliente);
void siguienteCliente();
void finCliente();
};

//---------------------------------------------------------------------

Barberia::Barberia(){
  sala_espera_clientes = newCondVar();
  silla = newCondVar();
  barbero = newCondVar();
}


//----------------------------------------------------------------------

void Barberia::cortarPelo(int cliente){
  mtx.lock();
  cout << "El cliente " << cliente << " entra en la barberia." << endl;
  mtx.unlock();

  if(sala_espera_clientes.empty()){
    if(barbero.empty()){
      mtx.lock();
      cout << "El barbero esta ocupado." << endl;
      mtx.unlock();
      sala_espera_clientes.wait();
    }else{
      mtx.lock();
      cout << "El cliente " << cliente << " despierta al barbero." << endl;
      mtx.unlock();
      barbero.signal();
    }
  }else{
    mtx.lock();
    cout << "El cliente " << cliente << " espera su turno en la sala. " << endl;
    mtx.unlock();
    sala_espera_clientes.wait();
  }

    mtx.lock();
    cout << "Es el turno del cliente " << cliente << endl;
    mtx.unlock();
    silla.wait();

}

//----------------------------------------------------------------------

void  Barberia::siguienteCliente(){
  if(sala_espera_clientes.empty() && silla.empty()){
    mtx.lock();
    cout << "Barbero: No hay clientes en la sala de espera." << endl;
    mtx.unlock();
    barbero.wait();
  }else{
    mtx.lock();
    cout << "Avisa al siguiente cliente" << endl;
    mtx.unlock();
    sala_espera_clientes.signal();
  }
}

//---------------------------------------------------------------------

void Barberia::finCliente(){
  mtx.lock();
  cout << "Barbero: He terminado de cortar el pelo." << endl;
  mtx.unlock();
  silla.signal();
}


//----------------------------------------------------------------------
// fuera del monitor
//----------------------------------------------------------------------

void cortarPeloACliente(){
  chrono::milliseconds duracion_cortar_pelo( aleatorio<50,200>());
  mtx.lock();
  cout << "El barbero esta cortando el pelo (" << duracion_cortar_pelo.count() << " milisegundos)."
  << endl << flush;
  mtx.unlock();
  this_thread::sleep_for(duracion_cortar_pelo);
  mtx.lock();
  cout << "El barbero ha terminado de cortar el pelo" << endl << flush;
  mtx.unlock();
}


void esperarFueraBarberia(int cliente){
    chrono::milliseconds duracion_esperar_fuera( aleatorio<50,200>());
    mtx.lock();
    cout << "El cliente " << cliente << " sale de la barbería (" << duracion_esperar_fuera.count( ) << " milisegundos)." << endl << flush;
    mtx.unlock();
    this_thread::sleep_for(duracion_esperar_fuera);
    mtx.lock();
    cout << "El cliente " << cliente << " termina la espera. " << endl << flush;
    mtx.unlock();
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del barbero

void funcion_hebra_barbero(MRef<Barberia> barberia)
{
  while (true){
    // entre nuevo cliente para cortar el pelo
    barberia->siguienteCliente();
    // cortar el pelo al cliente
    cortarPeloACliente();
    // anunciar al cliente que ha terminado de cortar el pelo
    barberia->finCliente();
  }
}


//----------------------------------------------------------------------
// función que ejecuta la hebra del cliente
void funcion_hebra_cliente(int cliente, MRef<Barberia> barberia)
{
  while (true){
    barberia->cortarPelo(cliente);
    esperarFueraBarberia(cliente);
  }
}

//----------------------------------------------------------------------

int main()
{
  MRef<Barberia> barberia = Create<Barberia> ();
  thread hebra_barbero ( funcion_hebra_barbero, barberia );
  thread hebras_clientes[num_clientes];
  for(int i=0; i<num_clientes; i++){
     hebras_clientes[i] = thread(funcion_hebra_cliente,i,barberia);
  }
  hebra_barbero.join();

  for(int i=0; i<num_clientes; i++){
    hebras_clientes[i].join();
  }

}
