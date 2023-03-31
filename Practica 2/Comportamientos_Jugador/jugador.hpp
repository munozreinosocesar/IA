#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
  bool zapatillas;
  bool bikini;
  bool objetivo1;
  bool objetivo2;
  bool objetivo3;

  bool operator==(const estado &a) const{
    if (a.fila == fila and
       a.columna == columna and
       a.orientacion == orientacion and
       a.zapatillas == zapatillas and
       a.bikini == bikini and
       a.objetivo1 == objetivo1 and
       a.objetivo2 == objetivo2 and
       a.objetivo3 == objetivo3)
      return true;
    else
      return false;
  }

  bool operator<(const estado &a) const{
    if (a.fila < fila)
      return true;
    else
      return false;
  }
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual;
    list<estado> objetivos;
    list<Action> plan;
    bool hayPlan;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Dijkstra(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Dijkstra3(const estado &origen, const estado &destino, const estado &destino2, const estado &destino3, list<Action> &plan);
    int coste(Action accion, estado &st);

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);

};

#endif
