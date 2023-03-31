#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>


// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores) {

	//actualizo la variable actual
	actual.fila        = sensores.posF;
	actual.columna     = sensores.posC;
	actual.orientacion = sensores.sentido;

	// Capturo los destinos
	objetivos.clear();
	for (int i=0; i<sensores.num_destinos; i++){
		estado aux;
		aux.fila = sensores.destino[2*i];
		aux.columna = sensores.destino[2*i+1];
		objetivos.push_back(aux);
	}

	//Si no hay plan, construyo uno
	if (!hayPlan){
		hayPlan = pathFinding (sensores.nivel, actual, objetivos, plan);
	}

	Action sigAccion;
	if (hayPlan and plan.size() > 0){
		sigAccion = plan.front();
		plan.erase(plan.begin());
	}
	else{
		cout << "no se pudo encontrar un plan\n";
	}

  return sigAccion;
}


// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const list<estado> &destino, list<Action> &plan){
	switch (level){
		case 0: cout << "Demo\n";
						estado un_objetivo;
						un_objetivo = objetivos.front();
						cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
			      return pathFinding_Profundidad(origen,un_objetivo,plan);
						break;

		case 1: cout << "Optimo numero de acciones\n";
						estado un_objetivo1;
						un_objetivo1 = objetivos.front();
						cout << "fila: " << un_objetivo1.fila << " col:" << un_objetivo1.columna << endl;
						return pathFinding_Anchura(origen,un_objetivo1,plan);
						break;

		case 2: cout << "Optimo en coste 1 Objetivo\n";
						estado un_objetivo2;
						un_objetivo2 = objetivos.front();
						cout << "fila: " << un_objetivo2.fila << " col:" << un_objetivo2.columna << endl;
						return pathFinding_Dijkstra(origen,un_objetivo2,plan);
						break;
		case 3: cout << "Optimo en coste 3 Objetivos\n";
						estado un_objetivo3, un_objetivo4, un_objetivo5;
						un_objetivo3 = objetivos.front();
						objetivos.pop_front();
						un_objetivo4 = objetivos.front();
						objetivos.pop_front();
						un_objetivo5 = objetivos.front();
						cout << "fila: " << un_objetivo3.fila << " col:" << un_objetivo3.columna << endl;
						cout << "fila: " << un_objetivo4.fila << " col:" << un_objetivo4.columna << endl;
						cout << "fila: " << un_objetivo5.fila << " col:" << un_objetivo5.columna << endl;

						return pathFinding_Dijkstra3(origen,un_objetivo3,un_objetivo4,un_objetivo5,plan);
						break;
		case 4: cout << "Algoritmo de busqueda usado en el reto\n";
						// Incluir aqui la llamada al algoritmo de busqueda usado en el nivel 2
						cout << "No implementado aun\n";
						break;
	}
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M')
		return true;
	else
	  return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}

struct nodo{
	estado st;
	list<Action> secuencia;
	int coste;
	bool operator <(const nodo &n) const{
		return coste > n.coste;
	}
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;								 // Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

  while (!Abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		Abiertos.pop();
		Cerrados.insert(current.st);


		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (Cerrados.find(hijoForward.st) == Cerrados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty()){
			current = Abiertos.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}


// Implementación de la busqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;								 // Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

  while (!Abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (Cerrados.find(hijoForward.st) == Cerrados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty()){
			current = Abiertos.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}


//---------------------- Implementación de Dijkstra ---------------------------

struct ComparaEstados2{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or
				(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini > n.bikini) or
				(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini == n.bikini and a.zapatillas > n.zapatillas))
			return true;
		else
			return false;
	}
};


struct ComparaEstados3{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or
				(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini > n.bikini) or
				(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini == n.bikini and a.zapatillas > n.zapatillas) or
				(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini == n.bikini and a.zapatillas == n.zapatillas and a.objetivo1 > n.objetivo1) or
				(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini == n.bikini and a.zapatillas == n.zapatillas and a.objetivo1 == n.objetivo1 and a.objetivo2 > n.objetivo2) or
				(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini == n.bikini and a.zapatillas == n.zapatillas and a.objetivo1 == n.objetivo1 and a.objetivo2 == n.objetivo2 and a.objetivo3 > n.objetivo3))
			return true;
		else
			return false;
	}
};

bool estaenCerrados(set<estado,ComparaEstados2> Cerrados, estado st){
	bool esta = false;
	set<estado,ComparaEstados2>::iterator it = Cerrados.begin();
	while (it != Cerrados.end())
	{
		if((*it) == st){
			esta = true;
		}
	    it++;
	}
	return esta;
}


bool estaenCerrados2(set<estado,ComparaEstados3> Cerrados, estado st){
	bool esta = false;
	set<estado,ComparaEstados3>::iterator it = Cerrados.begin();
	while (it != Cerrados.end())
	{
		if((*it) == st){
			esta = true;
		}
	    it++;
	}
	return esta;
}

int ComportamientoJugador::coste(Action accion, estado &st){
	int resultado = 0;
	char contenidoCasilla = mapaResultado[st.fila][st.columna];

	if(contenidoCasilla == 'K'){
		st.bikini = true;
		st.zapatillas = false;
	}else if(contenidoCasilla == 'D'){
		st.bikini = false;
		st.zapatillas = true;
	}

	switch(accion){
		case actFORWARD:
			if(contenidoCasilla == 'A' and !st.bikini){
				resultado = 200;
			}else if(contenidoCasilla == 'A' and st.bikini){
				resultado = 10;
			}else if(contenidoCasilla == 'B' and !st.zapatillas){
				resultado = 100;
			}else if(contenidoCasilla == 'B' and st.zapatillas){
				resultado = 15;
			}else if(contenidoCasilla == 'T'){
				resultado = 2;
			}else{
				resultado = 1;
			}
	 		break;
		case actTURN_L:
			if(contenidoCasilla == 'A' and !st.bikini){
				resultado = 500;
			}else if(contenidoCasilla == 'A' and st.bikini){
				resultado = 5;
			}else if(contenidoCasilla == 'B' and !st.zapatillas){
				resultado = 3;
			}else if(contenidoCasilla == 'B' and st.zapatillas){
				resultado = 1;
			}else if(contenidoCasilla == 'T'){
				resultado = 2;
			}else{
				resultado = 1;
			}
			break;
			case actTURN_R:
				if(contenidoCasilla == 'A' and !st.bikini){
					resultado = 500;
				}else if(contenidoCasilla == 'A' and st.bikini){
					resultado = 5;
				}else if(contenidoCasilla == 'B' and !st.zapatillas){
					resultado = 3;
				}else if(contenidoCasilla == 'B' and st.zapatillas){
					resultado = 1;
				}else if(contenidoCasilla == 'T'){
					resultado = 2;
				}else{
					resultado = 1;
				}
				break;
			case actIDLE:
				resultado = 0;
				break;
			}
				return resultado;
}

// Implementación de Dijkstra.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Dijkstra(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados2> Cerrados; // Lista de Cerrados
	priority_queue<nodo> Abiertos;								 // Lista de Abiertos

  nodo current;

	current.st = origen;
	current.st.bikini = false;
	current.st.zapatillas = false;
	current.coste = 0;
	current.secuencia.empty();
	Abiertos.push(current);

  while (!Abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		Abiertos.pop();
		Cerrados.insert(current.st);
		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end()){
			hijoTurnR.coste += coste(actTURN_R,hijoTurnR.st);
			hijoTurnR.secuencia.push_back(actTURN_R);
			if(!estaenCerrados(Cerrados,hijoTurnR.st))
				Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end()){
			hijoTurnL.coste += coste(actTURN_L,hijoTurnL.st);
			hijoTurnL.secuencia.push_back(actTURN_L);
			if(!estaenCerrados(Cerrados,hijoTurnL.st))
				Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (Cerrados.find(hijoForward.st) == Cerrados.end()){
				hijoForward.coste += coste(actFORWARD,hijoForward.st);
				hijoForward.secuencia.push_back(actFORWARD);
				if(!estaenCerrados(Cerrados,hijoForward.st))
					Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty()){
			current = Abiertos.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		cout << "Coste del plan: " << current.coste << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}

void objetivos_cumplidos(estado &st,const estado &destino1,const estado &destino2,const estado &destino3){
	if(st.fila == destino1.fila and st.columna == destino1.columna)
		st.objetivo1 = true;

	if(st.fila == destino2.fila and st.columna == destino2.columna)
		st.objetivo2 = true;

	if(st.fila == destino3.fila and st.columna == destino3.columna)
		st.objetivo3 = true;
}

// Implementación de Dijkstra.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Dijkstra3(const estado &origen, const estado &destino, const estado &destino2, const estado &destino3, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados3> Cerrados; // Lista de Cerrados
	priority_queue<nodo> Abiertos;				// Lista de Abiertos

  nodo current;

	current.st = origen;
	current.st.bikini = false;
	current.st.zapatillas = false;
	current.st.objetivo1 = false;
	current.st.objetivo2 = false;
	current.st.objetivo3 = false;

	current.coste = 0;
	current.secuencia.empty();
	Abiertos.push(current);

  while (!Abiertos.empty() and (!current.st.objetivo1 or !current.st.objetivo2  or !current.st.objetivo3)){
		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end()){
			hijoTurnR.coste += coste(actTURN_R,hijoTurnR.st);
			hijoTurnR.secuencia.push_back(actTURN_R);
			objetivos_cumplidos(hijoTurnR.st,destino,destino2,destino3);
			if(!estaenCerrados2(Cerrados,hijoTurnR.st))
				Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end()){
			hijoTurnL.coste += coste(actTURN_L,hijoTurnL.st);
			hijoTurnL.secuencia.push_back(actTURN_L);
			objetivos_cumplidos(hijoTurnL.st,destino,destino2,destino3);
			if(!estaenCerrados2(Cerrados,hijoTurnL.st))
				Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (Cerrados.find(hijoForward.st) == Cerrados.end()){
				hijoForward.coste += coste(actFORWARD,hijoForward.st);
				hijoForward.secuencia.push_back(actFORWARD);
				objetivos_cumplidos(hijoForward.st,destino,destino2,destino3);
				if(!estaenCerrados2(Cerrados,hijoForward.st))
					Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty()){
			current = Abiertos.top();
			if (Cerrados.find(current.st) != Cerrados.end()){
				Abiertos.pop();
				current = Abiertos.top();
			}

		}
	}

  cout << "Terminada la busqueda\n";

	if ((current.st.fila == destino.fila and current.st.columna == destino.columna) or
			(current.st.fila == destino2.fila and current.st.columna == destino2.columna) or
			(current.st.fila == destino3.fila and current.st.columna == destino3.columna)){

		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		cout << "Coste del plan: " << current.coste << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}




// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}


// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
