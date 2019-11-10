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

};

//---------------------------------------------------------------------

Barberia::Barberia(){
  sala_espera_clientes = newCondVar();
  silla = newCondVar();
  barbero = newCondVar();

}


//----------------------------------------------------------------------
// función que ejecuta la hebra del barbero

void funcion_hebra_barbero()
{

}


//----------------------------------------------------------------------
// función que ejecuta la hebra del cliente
void funcion_hebra_cliente()
{

}

//----------------------------------------------------------------------

int main()
{

}
