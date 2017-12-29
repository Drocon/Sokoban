/*

Jesus Alvarez Coll
Rafael Garces Hermosilla

Grupo 23

		-- PRACTICA 3 --
	En esta tercera practica hemos realizado el juego Sokoban. Su funcionamiento es sencillo.
	Mueves las cajas () que hay repartidas por el tablero con el jugador 00 usando las flechas
	direccionales hasta los destinos de la caja *. 
	Se ha añadido una funcionalidad de poder deshacer 10 movimientos en total, con lo cual en
	cualquier momento del juego puedes volver al movimiento anterior. Ademas, el programa te 
	dice si una caja que tenga el jugador a su alrededor esta bloqueada por lo que no se puede
	mover.
	Tras acabar un nivel, los resultados de cada jugador se guardaran en un archivo .txt con 
	el nombre del jugador para tener una lista con sus partidas ganadas y su información. Si
	no existe ese archivo lo crea. Tambien actualiza la información de un nivel si lo ha 
	realizado en menos movimientos que la anterior vez.
	
*/


#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <iomanip>
using namespace std;
#include <Windows.h>

//	****	CONSTANTES Y TIPOS	****
const int DIM = 50;

const int MAXH = 10;

const int MAXE = 100;

typedef enum tCasilla{
	Libre = ' ', 
	Muro = '#', 
	DestinoL = '.', 
	DestinoC = '*', 
	DestinoJ = '+', 
	Jugador = '@', 
	Caja = '$'
};

typedef tCasilla tTablero[DIM][DIM];

typedef struct {
	tTablero tablero;
	int nFilas;		//Menor que DIM
	int nColumnas;	//Menor que DIM
	int filaJugador;
	int columnaJugador;
	int cajasTablero;
	int cajasColocadas;
}tSokoban;

typedef tSokoban tEstadoTablero[MAXH];

typedef struct {
	tEstadoTablero estado;
	int elementosOcupados;
}tHistoria;

typedef struct {
	tHistoria historia;
	tSokoban sokoban;
	int numMovimientos;
	string nFichero;
	int nivel;
}tJuego;

typedef enum tTecla{
	Arriba = 72,
	Abajo = 80,
	Derecha = 77,
	Izquierda = 75,
	Salir = 27,
	Deshacerd = 100,
	DeshacerD = 68,
	Nada
};

typedef struct {
	string fichero;
	int nivel;
	int movimientos;
}tPartida;

typedef tPartida tExitos[MAXE];

typedef struct {
	string nombreJ;
	tExitos exitos;
	int nPartidasResueltas;
}tInfo;

//	**** DECLARACION DE SUBPROGRAMAS	****

//	*VERSION 1*
//	Visualizacion del tablero

void dibujaCasilla(tCasilla casilla);
/*
Da color a la casilla que sea tCasilla.
	Entrada: Un enumerado tCasilla.
	Salida: El color del cual se va a dibujar la casilla.
*/

void dibujar(const tJuego &juego);
/*
Dibuja la totalidad del array comprendido entre los limites posibles.
	Entrada: El estado del array en ese momento hasta los limites.
	Salida: El array dibujado.
*/

void colorFondo(int color);
/*
Selecciona el color del que se va a dibujar el fondo.
	Entrada: Un entero.
	Salida: El color seleccionado.
*/

//	Carga de un nivel del juego
void inicializa(tJuego &juego);
/*
Inicializa todo el struct tJuego con variables validas y nulas.
	Entrada: El estado del struct tJuego.
	Salida: Todo el struct tJuego inicializado.
*/

bool cargarJuego(tJuego &juego);
/*
Intenta cargar el juego abriendo el fichero.
	Entrada: El estado del struct tJuego.
	Salida: Un booleano de si se ha podido cargar.
*/

bool cargarNivel(ifstream &fichero, tSokoban &sokoban, int nivel);
/*
Intenta cargar el nivel deseado en el archivo dado anteriormente.
	Entrada: El fichero, el estado del struct tSokoban y el nivel al que se desea acceder.
	Salida: Un booleano de si se ha encontrado el nivel.
*/

void cargarSokoban(tSokoban &sokoban, ifstream &fichero);
/*
Carga dentro del array la informacion del archivo.
	Entrada: El estado del struct tSokoban y el fichero.
	Salida: El array cargado.
*/

//	Juego en accion
//		Lectura de teclas especiales

tTecla leerTecla();
/*
Lee de teclado que debe hacer el programa.
	Entrada: Un entero que es el valor de la tecla.
	Salida: El enumerado de una tecla/accion valida.
*/

void stringToElecciones();
/*
Salida de texto con las opciones durante el juego.
	Entrada: Nada.
	Salida: Varios couts.
*/

//		Movimientos del tablero
void hacerMovimiento(tJuego &juego, tTecla tecla);
/*
Se le asigna que movimiento va a hacer el programa segun la tecla pulsada.
	Entrada: El estado del struct tJuego y un enumerado tTecla.
	Salida: La seleccion de un movimiento.
*/

void condicionesArriba(tJuego &juego, tTecla tecla);
/*
Realiza las comprobaciones del movimiento hacia arriba.
	Entrada: El estado del struct tJuego y un enumerado tTecla.
	Salida: La realizacion del movimiento.
*/

void condicionesAbajo(tJuego &juego, tTecla tecla);
/*
Realiza las comprobaciones del movimiento hacia abajo.
	Entrada: El estado del struct tJuego y un enumerado tTecla.
	Salida: La realizacion del movimiento.
*/

void condicionesDerecha(tJuego &juego, tTecla tecla);
/*
Realiza las comprobaciones del movimiento hacia derecha.
	Entrada: El estado del struct tJuego y un enumerado tTecla.
	Salida: La realizacion del movimiento.
*/

void condicionesIzquierda(tJuego &juego, tTecla tecla);
/*
Realiza las comprobaciones del movimiento hacia izquierda.
	Entrada: El estado del struct tJuego y un enumerado tTecla.
	Salida: La realizacion del movimiento.
*/


void condicionesSalir(tJuego &juego, tTecla tecla);
/*
Realiza las comprobaciones del movimiento salir.
	Entrada: El estado del struct tJuego y un enumerado tTecla.
	Salida: La realizacion del movimiento.
*/

int menu();
/*
Menu del programa.
	Entrada: Nada.
	Salida: Un entero.
*/

void ejecutarOpc(int opc, tJuego &juego);
/*
Realiza la opcion seleccionada.
	Entrada: Un entero y el estado del struct tJuego.
	Salida: La realizacion de esa opcion.
*/

// *VERSION 2*
bool bloqueado(const tJuego juego);
/*
Comprueba si alguna de las cajas que tiene el usuario en cualquiera de las direcciones 
esta bloqueada o no.
	Entrada: El estado del struct tJuego.
	Salida: Un booleano de si esta bloqueado o no.
*/

bool deshacerMovimiento(tJuego &juego);
/*
Comprueba si se puede deshacer un movimiento. Si se puede deshacer, lo deshace.
	Entrada: El estado del struct tJuego.
	Salida: Un booleano de si se ha deshecho, y el tablero actualizado si se ha hecho.
*/

void guardarHistoria(tJuego &juego);
/*
Guarda los ultimos movimientos realizados por el usuario.
	Entrada: El estado del struct tJuego, en especial tHistoria.
	Salida: Actualizar el struct tHistoria. y tJuego.
*/


// *VERSION 3*
bool cargarExitos(ifstream &fichero, tInfo &info, tJuego juego);
/*
Comprueba si exite un usuario con un fichero con resultados. Si es afirmativo
lo muestra por pantalla.
	Entrada: Un fichero, el estado del struct tInfo y tJuego.
	Salida: Todos los struct actualizados y un booleano de si se ha cargado o no.
*/

void guardarPartida(ofstream &fichero, tInfo info);
/*
Guarda en un archivo con el nombre del jugador el resultado de una partida finalizada.
	Entrada: Un fichero y el estado del struct tInfo.
	Salida: El archivo guardado.
*/


//	****	PROGRAMA PRINCIPAL	****
int main() {
	int opcion = -1;
	tJuego juego;

	while (opcion != 0) {
		opcion = menu();
		ejecutarOpc(opcion, juego);
	}

	return 0;
}

//	****	SUBPROGRAMAS	****
//	Visualizacion del tablero
void dibujaCasilla(tCasilla casilla) {
	if (casilla == Libre) {
		colorFondo(1);
		cout << "  ";
	}
	else if (casilla == Muro) {
		colorFondo(3);
		cout << "  ";
	}
	else if (casilla == DestinoL) {
		colorFondo(6);
		cout << ". ";
	}
	else if (casilla == DestinoC) {
		colorFondo(5);
		cout << "[]";
	}
	else if (casilla == DestinoJ) {
		colorFondo(7);
		cout << "00";
	}
	else if (casilla == Jugador) {
		colorFondo(4);
		cout << "00";
	}
	else if (casilla == Caja) {
		colorFondo(8);
		cout << "()";
	}

}

void dibujar(const tJuego &juego) {
	int i =  0, j = 0;

	cout << "Nombre: " << juego.nFichero << " Nivel: " << juego.nivel << endl;

	for (; i < juego.sokoban.nFilas; i++) {
		for (; j < juego.sokoban.nColumnas; j++) {
			dibujaCasilla(juego.sokoban.tablero[i][j]);
		}
		colorFondo(0);
		cout << endl;
		j = 0;
	}

	cout << "Numero de movimientos: " << juego.numMovimientos << endl;
}

void colorFondo(int color) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, 15 | (color << 4));
}

//	Carga de un nivel del juego
void inicializa(tJuego &juego) {
	int f = 0;
	int c = 0;

	juego.nivel = 0;				//	Pongo 0 para incializarlo a un valor.
	juego.nFichero = " XXX ";		//	Pongo a XXX el nombre del fichero para inicializarlo.
	juego.numMovimientos = 0;		//	Pongo a 0 los movimientos para inicializarlos.

	juego.sokoban.cajasColocadas = 0;
	juego.sokoban.cajasTablero = 0;
	juego.sokoban.columnaJugador = 0;
	juego.sokoban.filaJugador = 0;
	juego.sokoban.nColumnas = 0;
	juego.sokoban.nFilas = 0;

	juego.historia.elementosOcupados = 0;

	for (; f < DIM; f++) {
		for (; c < DIM; c++) {
			juego.sokoban.tablero[f][c] = Libre;
			colorFondo(0);
		}
		c = 0;
	}
}

bool cargarJuego(tJuego &juego) {
	int nivel;
	ifstream fichero;
	string nombre;
	bool cargado = false;

	cout << "Fichero: ";
	cin >> nombre;

	fichero.open(nombre);
	if (fichero.is_open()) {
		cout << "Nivel: ";
		cin >> nivel;
		
		if (cargarNivel(fichero, juego.sokoban, nivel) == true) {
			juego.nFichero = nombre;
			juego.nivel = nivel;
					
			cargado = true;
		}
		else {
			cout << "No se ha encontrado el nivel." << endl;
		}
	}
	else {
		cout << "No se ha cargado el fichero. " << endl;
	}

	fichero.close();

	return cargado;
}

bool cargarNivel(ifstream &fichero, tSokoban &sokoban, int nivel) {
	bool encontrado = false;
	int nAux;				//Auxiliar para guardar el nivel
	string aux = "Level";	//Auxiliar para buscar Level
	string level;

	
	while (!fichero.eof()) {
		fichero >> level;

		if (level == aux) {
			fichero >> nAux;

			if (nAux == nivel) {
				cargarSokoban(sokoban, fichero);
				encontrado = true;
			}
		}
	}
	
	return encontrado;
}

void cargarSokoban(tSokoban &sokoban, ifstream &fichero) {
	string aux;
	int i = 0;
	int num = 0;

	getline(fichero, aux);
	getline(fichero, aux);

	while (aux[0] != 'L' && !fichero.eof()) {
		for (; i < aux.size(); i++) {
			if (aux[i] == '*') {
				sokoban.cajasTablero++;
				sokoban.cajasColocadas++;
			}
			else if (aux[i] == '@') {
				sokoban.columnaJugador = sokoban.nColumnas;
				sokoban.filaJugador = sokoban.nFilas;
			}
			else if (aux[i] == '$') {
				sokoban.cajasTablero++;
			}

			sokoban.tablero[sokoban.nFilas][sokoban.nColumnas] = (tCasilla)aux[i];
			sokoban.nColumnas++;
		}

		getline(fichero, aux);
		if (!fichero.eof() && aux[0] != 'L') {
			i = 0;
			sokoban.nFilas++;
			num = sokoban.nColumnas;
			sokoban.nColumnas = 0;
		}
		else {
			//sokoban.nFilas+1;
			sokoban.nColumnas = num- 1;
		}
	}
}

//	Juego en accion
//		Lectura de teclas especiales
tTecla leerTecla() {
	tTecla tecla = Nada;
	int dir = 0;

	stringToElecciones();
	cin.sync();
	dir =_getch();
	if (dir != 68 && dir != 100) {
		if (dir == 0xe0) {
			dir = _getch();

			switch (dir) {
			case Arriba:
				tecla = Arriba;
				break;
			case Abajo:
				tecla = Abajo;
				break;
			case Izquierda:
				tecla = Izquierda;
				break;
			case Derecha:
				tecla = Derecha;
				break;
			case Nada:
				tecla = Nada;
				break;
			}
		}
		else if (dir == 27)
			tecla = Salir;
	}
	else {
		switch (dir) {
		case DeshacerD:
			tecla = DeshacerD;
			break;
		case Deshacerd:
			tecla = Deshacerd;
			break;
		}
	}

	return tecla;
}

void stringToElecciones() {
	cout << setw(2) << "* Use las teclas para mover 00 -> el jugador. "<< endl;
	cout << setw(2) << "* D/d para Deshacer movimientos." << endl;
	cout << setw(2) << "* Esc para Salir." << endl;
}

//		Movimientos del tablero
void hacerMovimiento(tJuego &juego, tTecla tecla) {

	switch (tecla) {
	case Arriba:
		condicionesArriba(juego, Arriba);
		
		break;
	case Abajo:
		condicionesAbajo(juego, Abajo);

		break;
	case Izquierda:
		condicionesIzquierda(juego, Izquierda);

		break;
	case Derecha:
		condicionesDerecha(juego, Derecha);

		break;
	case Salir:
		return;
	
	case Nada:
		break;
	}
}

void condicionesArriba(tJuego &juego, tTecla tecla) {

	if (juego.sokoban.filaJugador - 1 > 0){
		if (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja) {
			if (juego.sokoban.filaJugador - 2 > 0) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Libre) {
					if(juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador){

						juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.filaJugador--;
						juego.numMovimientos++;
					}
					else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

						juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.filaJugador--;
						juego.numMovimientos++;
					}
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoL) {
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

						juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.filaJugador--;
						juego.sokoban.cajasColocadas++;
						juego.numMovimientos++;
					}
					else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {
						juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.filaJugador--;
						juego.numMovimientos++;
					}
				}
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == DestinoL) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

				juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.filaJugador--;
				juego.numMovimientos++;

			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.filaJugador--;
				juego.numMovimientos++;
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == DestinoC) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoL) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.filaJugador--;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

					juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.filaJugador--;
					juego.numMovimientos++;
				}
			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Libre) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)Caja;
					juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.filaJugador--;
					juego.sokoban.cajasColocadas--;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

					juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] = (tCasilla)Caja;
					juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.filaJugador--;
					juego.sokoban.cajasColocadas--;
					juego.numMovimientos++;
				}
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Libre) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

				juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.filaJugador--;
				juego.numMovimientos++;
			}
			else if(juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.filaJugador--;
				juego.numMovimientos++;
			}
		}
	}
}

void condicionesAbajo(tJuego &juego, tTecla tecla) {

	if (juego.sokoban.filaJugador + 1 < juego.sokoban.nFilas) {
		if (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja) {
			if (juego.sokoban.filaJugador + 2 < juego.sokoban.nFilas) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Libre) {
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

						juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.filaJugador++;
						juego.numMovimientos++;
					}
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

						juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.filaJugador++;
						juego.numMovimientos++;
					}
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoL) {
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

						juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.filaJugador++;
						juego.sokoban.cajasColocadas++;
						juego.numMovimientos++;
					}
					else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {
						juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.filaJugador++;
						juego.numMovimientos++;
					}
				}
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == DestinoL) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

				juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.filaJugador++;
				juego.numMovimientos++;

			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.filaJugador++;
				juego.numMovimientos++;
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == DestinoC) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoL) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.filaJugador++;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ)	{

					juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.filaJugador++;
					juego.numMovimientos++;
				}
			}
			else  if(juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Libre ){
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)Caja;
					juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.filaJugador++;
					juego.sokoban.cajasColocadas--;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

					juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.filaJugador++;
					juego.sokoban.cajasColocadas--;
					juego.numMovimientos++;
				}
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Libre) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

				juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.filaJugador++;
				juego.numMovimientos++;
			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.filaJugador++;
				juego.numMovimientos++;
			}
		}
	}
}

void condicionesDerecha(tJuego &juego, tTecla tecla) {

	if (juego.sokoban.columnaJugador + 1 < juego.sokoban.nColumnas) {
		if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja) {
			if (juego.sokoban.columnaJugador + 2 < juego.sokoban.nColumnas) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Libre){ 
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.columnaJugador++;
						juego.numMovimientos++;
					}
					else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.columnaJugador++;
						juego.numMovimientos++;
					}

				}
				else if(juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2]==DestinoL){
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.columnaJugador++;
						juego.sokoban.cajasColocadas++;
						juego.numMovimientos++;
					}
					else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.columnaJugador++;
						juego.numMovimientos++;
					}
				}
			}
		}
		else if(juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == DestinoL){
			if(juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ){

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.columnaJugador++;
				juego.numMovimientos++;

			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.columnaJugador++;
				juego.numMovimientos++;
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == DestinoC) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == DestinoL) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.columnaJugador++;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ ) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.columnaJugador++;
					juego.numMovimientos++;
				}
			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Libre) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)Caja;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.cajasColocadas--;
					juego.sokoban.columnaJugador++;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] = (tCasilla)Caja;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.cajasColocadas--;
					juego.sokoban.columnaJugador++;
					juego.numMovimientos++;
				}

			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Libre) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.columnaJugador++;
				juego.numMovimientos++;
			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.columnaJugador++;
				juego.numMovimientos++;
			}
		}
	}
}

void condicionesIzquierda(tJuego &juego, tTecla tecla) {

	if (juego.sokoban.columnaJugador - 1 > 0){
		if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja) {
			if (juego.sokoban.columnaJugador - 2 > 0) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Libre) {
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.columnaJugador--;
						juego.numMovimientos++;
					}
					else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)Caja;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.columnaJugador--;
						juego.numMovimientos++;
					}
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoL) {
					if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
						juego.sokoban.columnaJugador--;
						juego.sokoban.cajasColocadas++;
						juego.numMovimientos++;
					}
					else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)DestinoC;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)Jugador;
						juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
						juego.sokoban.columnaJugador--;
						juego.sokoban.cajasColocadas++;
						juego.numMovimientos++;
					}
				}
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == DestinoL) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.columnaJugador--;
				juego.numMovimientos++;

			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)DestinoJ;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.columnaJugador--;
				juego.numMovimientos++;
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == DestinoC) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoL) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.columnaJugador--;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)DestinoC;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.columnaJugador--;
					juego.numMovimientos++;

				}
			}
			else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Libre) {
				if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)Caja;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
					juego.sokoban.columnaJugador--;
					juego.numMovimientos++;
				}
				else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] = (tCasilla)Caja;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)DestinoJ;
					juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
					juego.sokoban.columnaJugador--;
					juego.numMovimientos++;
				}
			}
		}
		else if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Libre) {
			if (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == DestinoJ) {

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)DestinoL;
				juego.sokoban.columnaJugador--;
				juego.numMovimientos++;
			}
			else if(juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] == Jugador) {

				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] = (tCasilla)Jugador;
				juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador] = (tCasilla)Libre;
				juego.sokoban.columnaJugador--;
				juego.numMovimientos++;
			}
		}
	}
}

int menu() {
	int op = -1;
	int tiempo = 1000;


	while ((op < 0) || (op > 2)) {
		cout << " --- Practica 3 FP: *SOKOBAN* --- " << endl;
		cout << "  1 - Jugar partida." << endl;
		cout << "  2 - Ver resultados de usuarios." << endl;
		cout << "  0 - Salir." << endl;
		cout << "  Opcion: ";
		cin >> op;

		if ((op < 0) || (op > 2)) {
			cout << "¡Opcion no valida!" << endl;
			Sleep(tiempo);
		}
		system("cls");
	}

	return op;
}

void ejecutarOpc(int opc, tJuego &juego) {
	int tiempo = 1500;
	int Tiempo = 500;
	int movimientos = 0;
	bool bloq = false;
	bool deshacer = false;
	bool cargar;
	bool guardar;
	bool existos;
	string aux;
	ifstream fich;
	ofstream fichero;
	tInfo info;
	tTecla tecla = Nada;
	
	switch (opc) {
	case 1:
		inicializa(juego);
		cargar = cargarJuego(juego);
		system("cls");

		if (cargar == true) {
			while (tecla != Salir) {
				dibujar(juego);
				cout << "Numero de movimientos desechos: " << movimientos << endl;

				if (bloq == true) {
					cout << endl;
					cout << " Caja bloqueada. " << endl;
					bloq = false;
				}

				cout << endl;
				tecla = leerTecla();

				if (tecla == Derecha || tecla == Izquierda || tecla == Arriba || tecla == Abajo) {
					guardarHistoria(juego);
					hacerMovimiento(juego, tecla);
					bloq = bloqueado(juego);

					if (juego.sokoban.cajasColocadas == juego.sokoban.cajasTablero) {
						system("cls");
						dibujar(juego);
						cout << endl;
						cout << " NIVEL SUPERADO. FELICIDADES. " << endl;
						cout << " Nº movimientos: " << juego.numMovimientos << endl;
						Sleep(tiempo);

						//Version 3
						cout << endl;
						cout << " Nombre del jugador: ";
						cin >> info.nombreJ;
						aux = info.nombreJ + ".txt";
						info.nPartidasResueltas = 0;

						fich.open(aux);
						existos = cargarExitos(fich, info, juego);

						if (existos == true) {
							fichero.open(aux);
							guardarPartida(fichero, info);
							fichero.close();

							tecla = Salir;
						}
						else {
							fichero.open(aux);
							guardarPartida(fichero, info);
							fichero.close();

							tecla = Salir;
						}
						system("pause");
					}
				}
				else if (tecla == DeshacerD || tecla == Deshacerd) {
					if (movimientos < MAXH) {
						deshacer = deshacerMovimiento(juego);

						if (deshacer == true) {
							cout << endl;
							cout << "Movimiento deshecho." << endl;
							Sleep(Tiempo);
							movimientos++;
							juego.numMovimientos--;
						}

					}
					else {
						cout << endl;
						cout << "Se ha llegado al limite maximo de deshacer movimientos." << endl;
						Sleep(Tiempo);
					}
				}

				system("cls");
			}
		}
		system("cls");
	break;
	case 2:
		string usuario, aux, n;
		int lvl, ni;
		ifstream fIn;

		system("cls");
		cout << "------------------------------------------" << endl;
		cout << "* Nombre de usuario: ";
		cin >> usuario;
		cout << "------------------------------------------" << endl;;
		cout << endl;
		aux = usuario;
		usuario = usuario + ".txt";

		cout << "------------------------------------------" << endl;;
		fIn.open(usuario);
		if (fIn.is_open()) {
			while (!fIn.eof()) {
				fIn >> n;
				fIn >> lvl;
				fIn >> ni;
				cout << "* " << n << "     "<< lvl << "     " << ni << endl;
			}
			fIn.close();
		}
		else {
			cout << endl;
			cout << "El usuario " << aux << " no ha registrado ninguna partida terminada." << endl;

		}
		cout << "------------------------------------------" << endl;;

		cout << endl;
		system("pause");
		system("cls");
	break;
	}
}

// *** Version 2 ***
bool bloqueado(const tJuego juego) {
	bool bloq = false;

	//Derecha
	if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja) 
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Muro) 
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Muro)) {
		bloq = true;
	}

	else if((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador + 2] == DestinoC)) {
		bloq = true;
	}


	//Izquierda
	if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador][juego.sokoban.columnaJugador - 2] == DestinoC)) {
		bloq = true;
	}

	//Arriba
	if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador - 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}


	//Abajo
	if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Muro)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Muro)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == DestinoC)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == Caja)) {
		bloq = true;
	}

	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador - 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}
	else if ((juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 1][juego.sokoban.columnaJugador + 1] == Caja)
		&& (juego.sokoban.tablero[juego.sokoban.filaJugador + 2][juego.sokoban.columnaJugador] == DestinoC)) {
		bloq = true;
	}

	return bloq;
}

bool deshacerMovimiento(tJuego &juego) {

	bool deshacer = false;

	if (juego.historia.elementosOcupados > 0) {
		int aux = 0;
		int i = 0, j = 0;

		juego.sokoban.cajasColocadas = juego.historia.estado[juego.historia.elementosOcupados - 1].cajasColocadas;
		juego.sokoban.cajasTablero = juego.historia.estado[juego.historia.elementosOcupados - 1].cajasTablero;
		juego.sokoban.columnaJugador = juego.historia.estado[juego.historia.elementosOcupados - 1].columnaJugador;
		juego.sokoban.filaJugador = juego.historia.estado[juego.historia.elementosOcupados - 1].filaJugador;
		juego.sokoban.nColumnas = juego.historia.estado[juego.historia.elementosOcupados - 1].nColumnas;
		juego.sokoban.nFilas = juego.historia.estado[juego.historia.elementosOcupados - 1].nFilas;

		for (i = 0; i < juego.sokoban.nFilas; i++) {
			for (j = 0; j < juego.sokoban.nColumnas; j++) {
				juego.sokoban.tablero[i][j] = juego.historia.estado[juego.historia.elementosOcupados - 1].tablero[i][j];
			}
		}

		juego.historia.elementosOcupados--;
		deshacer = true;
	}

	return deshacer;
}

void guardarHistoria(tJuego &juego) {
	int i = 0;
	int j = 0;

	if (juego.historia.elementosOcupados == 0) {
		juego.historia.estado[juego.historia.elementosOcupados].cajasColocadas = juego.sokoban.cajasColocadas;
		juego.historia.estado[juego.historia.elementosOcupados].cajasTablero = juego.sokoban.cajasTablero;
		juego.historia.estado[juego.historia.elementosOcupados].columnaJugador = juego.sokoban.columnaJugador;
		juego.historia.estado[juego.historia.elementosOcupados].filaJugador = juego.sokoban.filaJugador;
		juego.historia.estado[juego.historia.elementosOcupados].nColumnas = juego.sokoban.nColumnas;
		juego.historia.estado[juego.historia.elementosOcupados].nFilas = juego.sokoban.nFilas;

		for (; i < juego.sokoban.nFilas; i++) {
			for (; j < juego.sokoban.nColumnas; j++) {
				juego.historia.estado[juego.historia.elementosOcupados].tablero[i][j] = juego.sokoban.tablero[i][j];
			}
			j = 0;
		}

		juego.historia.elementosOcupados++;
	}
	else if (juego.historia.elementosOcupados < MAXH -1){
		juego.historia.estado[juego.historia.elementosOcupados].cajasColocadas	= juego.sokoban.cajasColocadas;
		juego.historia.estado[juego.historia.elementosOcupados].cajasTablero		= juego.sokoban.cajasTablero;
		juego.historia.estado[juego.historia.elementosOcupados].columnaJugador	= juego.sokoban.columnaJugador;
		juego.historia.estado[juego.historia.elementosOcupados].filaJugador		= juego.sokoban.filaJugador;
		juego.historia.estado[juego.historia.elementosOcupados].nColumnas			= juego.sokoban.nColumnas;
		juego.historia.estado[juego.historia.elementosOcupados].nFilas			= juego.sokoban.nFilas;


		for (i = 0; i < juego.sokoban.nFilas; i++) {
			for (j = 0; j < juego.sokoban.nColumnas; j++) {
				juego.historia.estado[juego.historia.elementosOcupados].tablero[i][j] = juego.sokoban.tablero[i][j];
			}
		}

		juego.historia.elementosOcupados++;
	}
	else {
		for (i = 0; i < juego.historia.elementosOcupados - 1; i++) {
			juego.historia.estado[i] = juego.historia.estado[i + 1];
		}

		juego.historia.estado[juego.historia.elementosOcupados - 1].cajasColocadas = juego.sokoban.cajasColocadas;
		juego.historia.estado[juego.historia.elementosOcupados - 1].cajasTablero = juego.sokoban.cajasTablero;
		juego.historia.estado[juego.historia.elementosOcupados - 1].columnaJugador = juego.sokoban.columnaJugador;
		juego.historia.estado[juego.historia.elementosOcupados - 1].filaJugador = juego.sokoban.filaJugador;
		juego.historia.estado[juego.historia.elementosOcupados - 1].nColumnas = juego.sokoban.nColumnas;
		juego.historia.estado[juego.historia.elementosOcupados - 1].nFilas = juego.sokoban.nFilas;


		for (i = 0; i < juego.sokoban.nFilas; i++) {
			for (j = 0; j < juego.sokoban.nColumnas; j++) {
				juego.historia.estado[juego.historia.elementosOcupados - 1].tablero[i][j] = juego.sokoban.tablero[i][j];
			}
		}
	}
}

// *** Version 3 ***
bool cargarExitos(ifstream &fichero, tInfo &info, tJuego juego) {
	int nivel;
	int i;
	int j;
	int pos;
	bool cargado = false;

	if (fichero.is_open()) {
		while (!fichero.eof()) {
			fichero >> info.exitos[info.nPartidasResueltas].fichero;
			fichero >> info.exitos[info.nPartidasResueltas].nivel;
			fichero >> info.exitos[info.nPartidasResueltas].movimientos;
			info.nPartidasResueltas++;
		}
		
		if (info.nPartidasResueltas < MAXE) {
			i = 0;

			while ((i < info.nPartidasResueltas) && ((info.exitos[i].fichero != juego.nFichero) || (info.exitos[i].nivel < juego.nivel))) {
				i++;
			}

			if (info.exitos[i].nivel != juego.nivel) {
				//Parte que va bien
				for (j = info.nPartidasResueltas; j > i; j--) {
					info.exitos[j] = info.exitos[j - 1];
				}

				info.exitos[i].fichero = juego.nFichero;
				info.exitos[i].nivel = juego.nivel;
				info.exitos[i].movimientos = juego.numMovimientos;
				info.nPartidasResueltas++;
				//
			}
			else {
				if (info.exitos[i].movimientos >= juego.numMovimientos) {
					info.exitos[i].fichero = juego.nFichero;
					info.exitos[i].nivel = juego.nivel;
					info.exitos[i].movimientos = juego.numMovimientos;
				}
				else {
					cout << endl;
					cout << " Hiciste una puntuacion menor anteriormente: " << info.exitos[i].movimientos << endl;
					cout << " Intentalo de nuevo y mejora tu record." << endl;
					cout << endl;
				}
			}
		}

		cargado = true;
		fichero.close();
	}
	else {
		info.exitos[info.nPartidasResueltas].fichero = juego.nFichero;
		info.exitos[info.nPartidasResueltas].nivel = juego.nivel;
		info.exitos[info.nPartidasResueltas].movimientos = juego.numMovimientos;
		info.nPartidasResueltas++;
	}
		
	return cargado;
}

void guardarPartida(ofstream &fichero, tInfo info) {
	int i = 0;

	for (i = 0; i < info.nPartidasResueltas; i++) {
		fichero << info.exitos[i].fichero << " " << info.exitos[i].nivel
			<< " " << info.exitos[i].movimientos;

		if (i != info.nPartidasResueltas - 1) {
			fichero << endl;
		}
	}
	
	cout << endl;
	cout << " Se ha guardado correctamente. " << endl;
	cout << endl;
}
