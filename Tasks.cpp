#include "Tasks.h"
#include <conio.h>

/*Matrices DCT e IDCT multiplicadas por 2^9 = 512
long DCT[64] = { 181,  181,  181,  181,  181,  181,  181,  181,
251,  212,  142,   49,  -49, -142, -212, -251,
236,   97,  -97, -236, -236,  -97,   97,  236,
212,  -49, -251, -142,  142,  251,   49, -212,
181, -181, -181,  181,  181, -181, -181,  181,
142, -251,   49,  212, -212,  -49,  251, -142,
97, -236,  236, -97,  -97,   236, -236,   97,
49, -142,  212, -251,  251, -212,  142,  -49,
};

long DCTt[64] = { 181,  251,  236,  212,  181,  142,   97,   49,
181,  212,   97,  -49, -181, -251, -236, -142,
181,  142,  -97, -251, -181,   49,  236,  212,
181,   49, -236, -142,  181,  212,  -97, -251,
181,  -49, -236,  142,  181, -212,  -97,  251,
181, -142,  -97,  251, -181,  -49,  236, -212,
181, -212,   97,   49, -181,  251, -236,  142,
181, -251,  236, -212,  181, -142,   97,  -49,
};
*/

//Valores de la matriz DCT e IDCT multiplicados por 2^8 = 256 y redondeadas.
//De esta forma tenemos valores enteros y no en punto flotante
long DCT[64] = {	90,   90,   90,   90,   90,   90,   90,   90,
					125,  106,  71,   24,   -24,  -71,  -106, -125,
					118,  48,   -48,  -118, -118, -48,  48,   118,
					106,  -24,  -125, -71,  71,   125,  24,   -106,
					90,   -90,  -90,  90,   90,   -90,  -90,  90,
					71,   -125, 24,   106,  -106, -24,  125,  -71,
					48,   -118, 118,  -48,  -48,  118,  -118, 48,
					24,   -71,  106,  -125, 125,  -106, 71,   -24,
				};

long DCTt[64] = {	90,  125,  118,  106,   90,   71,   48,   24,
					90,  106,  48,   -24,   -90,  -125, -118, -71,
					90,  71,   -48,  -125,  -90,  24,   118,  106,
					90,  24,   -118, -71,   90,   106,  -48,  -125,
					90,  -24,  -118, 71,    90,   -106, -48,  125,
					90,  -71,  -48,  125,   -90,  -24,  118,  -106,
					90,  -106, 48,   24,    -90,  125,  -118, 71,
					90,  -125, 118,  -106,  90,   -71,  48,   -24,
				};

/*
Function : blocker_8_512_sq
--------------------------

Ordenará la imagen que le entra como argumento en bloques de 8x8. Por lo tanto, se tendrá a su 
salida un vector en el que los primeros 64 valores corresponden al primer bloque de 8x8 de la 
imagen de entrada, los siguientes 64 valores corresponderán al segundo bloque, etc.
Para ello es necesario crear el vector block_pattern para conocer que posiciones coger del vector
de entrada para conseguir el orden correcto. 
block_pattern = [0, 1, ..., 7, 512, 513, ..., 519, 1024, 1025, ..., 1031,..., 3583, 3584, ..., 3590]

cdata:	vector de 512x512 correspondiente a los valores de los píxeles de la imagen de entrada.
		Primeros 512 valores corresponden a la primera fila de la imagen
		Segundos 512 valores corresponden a la segunda final de la imagen
		Etc.
bstring:	vector de 512x512 valores en los que los píxeles estarán ordenados en bloques de 8x8.
			Primeros 64 valores corresponden al primer bloque de 8x8 píxeles de la imagen original
			Segundos 64 valores corresponden al segundo bloque de 8x8 píxeles de la imagen original
			Etc.

returns:	Número de bloques resultantes.

*/

int blocker_8_512_sq(UCHAR *cdata, UCHAR *bstring)
{
	const int n = 512, m = 512;
	int block_pattern[B*B], c = 0;
	
	//Creación del vector block_pattern. Descripción arriba.
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		
			block_pattern[c++] = j;

	//Obtención del vector de datos ordenados en bloques de 8x8
	for (int i = 0; i < (m / B); i++)
		for (int j = 0; j < (n / B); j++)
			for (int k = 0; k < B*B; k++)
				bstring[k + j*(B*B) + i*(n / B)*(B*B)] = (UCHAR)(cdata[block_pattern[k] + B*j + (n / B)*B*B*i]);

	//Devuelve el número de bloques resultante
	return (m / B)*(m / B);
}


/*
Function : imager_8_512_sq
--------------------------

Generará la imagen a partir de un vector con los bloques de 8x8 valores.
Para ello es necesario crear el vector block_pattern para conocer que posiciones coger del vector
de entrada para conseguir el orden correcto en la imagen final.
block_pattern = [0, 1, ..., 7, 512, 513, ..., 519, 1024, 1025, ..., 1031,..., 3583, 3584, ..., 3590]

bstring:	vector de 512x512 correspondiente a los bloques de 8x8 píxeles.
			Primeros 64 valores corresponden el primer bloque de 8x8
			Segundos 64 valores corresponden al segundo bloque de 8x8
			Etc.
cdata:	vector de 512x512 valores en los que los píxeles estarán ordenados para obtener una imagen de 512x512.
		Primeros 512 valores corresponden a la primera fila de la imagen
		Segundos 512 valores corresponden a la segunda fila de la imagen
		Etc.

returns:	Número de bloques resultantes.

*/
int imager_8_512_sq(int *bstring, UCHAR *cdata)
{
	const int n = 512, m = 512;
	int block_pattern[B*B], c = 0;

	//Creación del vector block_pattern. Descripción arriba.
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		
			block_pattern[c++] = j;

	//Obtención de la imagen de 512x512 píxeles
	for (int i = 0; i < (m / B); i++)
		for (int j = 0; j < (n / B); j++)
			for (int k = 0; k < B*B; k++)
				cdata[block_pattern[k] + B*j + (n / B)*B*B*i] = (UCHAR)(bstring[k + j*(B*B) + i*(n / B)*(B*B)]);

	return (m / B)*(m / B);
}


/*
Function : copy_L
-----------------

Copia los valores de formato long del vector del primer argumneto en el vector del segundo argumento

copied:	puntero a los datos que se quieren copiar
copy:	puntero a los datos donde se quiere copiar
d:	número de datos que se quieren copiar

*/
void copy_L(long *copied, long *copy, int d)
{
	for (int i = 0; i < d; i++)
		copy[i] = copied[i];
}


/*
Function : block_mask_8_512_sq
------------------------------

Creará la máscara para quedarnos con los valores deseados tras la DCT. Esta máscara será un vector
de 8x8 valores que tendrá los 1s deseados en la esquina superior izquierda y 0s en las posiciones
restantes. Ejemplo, si d es 5:
mask =	1 1 1 1 1 0 0 0
		1 1 1 1 0 0 0 0
		1 1 1 0 0 0 0 0
		1 1 0 0 0 0 0 0
		1 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0

mask:	puntero al vector donde guardaremos los valores de la máscara
d:	número diagonales de 1s deseado. Valor máximo 8.

*/
void block_mask_8_512_sq(UCHAR *mask, int d)
{
	int c = 0;

	for (int i = 0; i < B; i++)
		for (int j = 0; j < B; j++)
			if (d - i > j)
				mask[c++] = 1;
			else
				mask[c++] = 0;
}





// TEMP /////////////////////////////////////////////////////////////////////////////////////////////////

	double vectorResult[B*B];
	double vectorMask[B*B];
	double vectorTrans[B*B];
	double mask[B*B];



/*
Function : AxB_L
----------------

Multiplicación matricial de valores long M1xM2

M1:	vector de 8x8 valores que será el primer operando de la multiplicación matricial
M2: vector de 8x8 valores que será el segundo operando de la multiplicación matricial

returns:	puntero para permitir llamadas recursivas.

*/
long * AxB_L(long M1[B * B], long M2[B * B])
{
	long TMP[B * B];
	for (int k = 0; k < B; k++) {
		for (int i = 0; i < B; i++) {
			TMP[i + k*B] = 0;
			for (int j = 0; j < B; j++) {
				TMP[k*B + i] += (M1[j + k*B]) * M2[i + j*B];
			}
		}
	}
	//Copiamos los valores obtenidos en TMP
	copy_L(TMP, M1, B * B);
	//Devuelve el puntero para permitir llamadas recursivas
	return M1; 
}


/*
Function : BxA_L
----------------

Multiplicación matricial de valores long M2xM1

M1:	vector de 8x8 valores que será el primer operando de la multiplicación matricial
M2: vector de 8x8 valores que será el segundo operando de la multiplicación matricial

returns:	puntero para permitir llamadas recursivas.

*/
long * BxA_L(long M1[B * B], long M2[B * B])
{
	long TMP[B * B];
	for (int k = 0; k < B; k++) {
		for (int i = 0; i < B; i++) {
			TMP[i + k*B] = 0;
			for (int j = 0; j < B; j++) {
				TMP[k*B + i] += (M2[j + k*B]) * M1[i + j*B];
			}
		}
	}
	//Copiamos los valores obtenidos en TMP
	copy_L(TMP, M1, B * B);
	//Devuelve el puntero para permitir llamadas recursivas
	return M1; 
}


/*
Function : INT_to_L
-------------------

Transforma los valores de una matriz de formato int a formato long

M1:	vector de 8x8 valores con formato int
A_long:	vector de 8x8 valores con formato long

*/
void INT_to_L(int M1[B * B], long A_long[B * B]) 
{
	for (int k = 0; k < B * B; k++) {
		A_long[k] = (long)M1[k];
	}
}


/*
Function : UCHAR_to_L
---------------------

Transforma los valores de una matriz de formato unsigned char a formato long

M1:	vector de 8x8 valores con formato unsigned char
A_long:	vector de 8x8 valores con formato long

*/
void UCHAR_to_L(UCHAR M1[B * B], long A_long[B * B])
{
	for (int k = 0; k < B * B; k++) {
		A_long[k] = (long)M1[k];
	}
}


/*
Function : L_to_INT
-------------------

Transforma los valores de una matriz de formato long a formato int

M1:	vector de 8x8 valores con formato long
A_int:	vector de 8x8 valores con formato int

*/
void L_to_INT(long M1[B * B], int A_int[B * B]) {
	for (int k = 0; k < B * B; k++) {
		A_int[k] = (int)M1[k];
	}
}


/*
Function : MplusC
-----------------

Sumará a todos los valores de una matriz un valor constante

M1:	vector de 8x8 valores con formato long
cns:	valor constante que sumar a cada valor del vector

*/
void MplusC(long M1[B * B], int cns)
{
	for (int i = 0; i < B*B; i++) {
		M1[i] += cns;
	}
}


/*
Function : MshiftRight
----------------------

Función que realizará una desplazamiento hacia la derecha 2*shift posiciones de todos
los valores de un vector. Será como realizar una división entre 2^shift.

M1:	vector de 8x8 valores con formato long
shift:	definirá el número de posiciones que se desplaza cada valor del vector

*/
void MshiftRight(long M1[B * B], int shift)
{
	for (int i = 0; i < B*B; i++) {
		M1[i] = M1[i] >> 2 * shift;
	}

}


/*
Function : QnD_TATt
-------------------

Bloque encargado de realizar la DCT sobre los valores de entrada.
Tendrá como entrada bloques de 8x8 valores de 1byte y como salida bloques de 
8x8 valores en formato int.
Realizará los siguientes pasos:
	1. Conversión de unsigned char a long
	2. Resta de los valores por 127 para centraros en torno a 0 (pasamos de 0-255 a -127-128)
	3. DCT
	4. Desplazamiento de 2*N de los valores resultado de la DCT
	5. Transformación de los valores de long a int

A:	vector de 8x8 valores en formato unsigned char sobre los que vamos a realizar la DCT
A_int:	vector de 8x8 valores en formato int que será la salida del bloque

*/
void QnD_TATt(UCHAR A[B * B], int A_int[B * B])
{
	//Creamos esta matriz para hacer el cambio de UCHAR A long
	long A_long[B*B];

	//Pasamos los valores de UCHAR a long
	UCHAR_to_L(&A[0], A_long);

	//Le restamos 127 a los valores en long
	MplusC(&A_long[0], -127);

	//Hacemos la DCT: DCT*A_long*DCTt en formato long para no perder valores
	BxA_L(AxB_L(A_long, DCTt), DCT);

	//Dividimos entre 2*2^N. Esto lo hacemos desplazando 2*N a la derecha. 
	//Se divide entre dos veces porque estamos multiplicando por DCT e IDCT y cada una de ellas está multiplicada por 2^N
	MshiftRight(&A_long[0], N);

	//Guardamos la salida en valores tipo int (16bits). No utilizamos 8bits porque se saturan los mayores valores 
	//y justo son esos los que queremos guardar. Estos valores serán los que salgan hacia el siguiente bloque.
	L_to_INT(&A_long[0], &A_int[0]);

}


/*
Function : QnD_TtAT
-------------------

Bloque encargado de realizar la IDCT sobre los valores de entrada.
Tendrá como entrada bloques de 8x8 valores int y como salida bloques de
8x8 valores en formato int.
Realizará los siguientes pasos:
1. Conversión de int a long
2. IDCT 
3. Desplazamiento de 2*N de los valores resultado de la IDCT
4. Suma de los valores de la IDCT por 127 para volver a tener lo valores en el rango 0-255
5. Transformación de los valores de long a int

A:	vector de 8x8 valores en formato int sobre los que vamos a realizar la IDCT
A_int:	vector de 8x8 valores en formato int que será la salida del bloque

*/
void QnD_TtAT(int A[B * B], int A_int[B * B])
{
	//Creamos esta matriz para hacer el cambio de int A long
	long A_long[B*B];

	//Pasamos los valores de int a long
	INT_to_L(&A[0], A_long);

	//Hacemos la DCT inversa
	BxA_L(AxB_L(A_long, DCT), DCTt);

	//Dividimos entre 2*2^N. Esto lo hacemos desplazando 2*N a la derecha. 
	//Se divide entre dos veces porque estamos multiplicando por DCT e IDCT y cada una de ellas está multiplicada por 2^N
	MshiftRight(&A_long[0], N);

	//Le sumamos 127 a los valores en long porque durante la DCT le restamos 127
	MplusC(&A_long[0], 127);

	//Guardamos la salida en valores tipo int (16bits). No utilizamos 8bits porque se saturan los mayores valores 
	//y justo son esos los que queremos guardar. Estos valores serán los que salgan hacia el siguiente bloque.
	L_to_INT(&A_long[0], &A_int[0]);

}


/*
Function : Mask
-------------------

Aplicará la máscara creada al bloque de 8x8 de entrada. De esta forma, nos quedaremos con únicamente los valores
deseados de la DCT.

A:	vector de 8x8 valores en formato int sobre los que vamos a aplicar la máscara
mask:	vector de 8x8 valores que corresponderá con la máscara

*/
void Mask(int A[B*B], UCHAR mask[B*B]) {
	for (int p = 0; p < B*B; p++)
		A[p] *= mask[p];
}
