/*
Código creado por Carlos Pastor Ramírez y Miguel Ángel Alfonso Alfonso
para la asignatura Lenguajes y Herramientas de Diseño Digital.

Header file de Task.cpp

Curso: 2016-2017

*/

#ifndef _TSK_H_
#define _TSK_H_

#include <math.h>
#include "qdbmp.h"

#define B 8 //tamaño de los bloques
#define N 8 //para conocer los desplazamientos necesarios en los valores para tener valores enteros y no punto flotante

//Valores originales de la matriz 8x8 DCT e IDCT
//No los vamos a usar en este código
/*
const float T[64] = { 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F,
0.4904F, 0.4157F, 0.2778F, 0.0975F,-0.0975F,-0.2778F,-0.4157F,-0.4904F,
0.4619F, 0.1913F,-0.1913F,-0.4619F,-0.4619F,-0.1913F, 0.1913F, 0.4619F,
0.4157F,-0.0975F,-0.4904F,-0.2778F, 0.2778F, 0.4904F, 0.0975F,-0.4157F,
0.3536F,-0.3536F,-0.3536F, 0.3536F, 0.3536F,-0.3536F,-0.3536F, 0.3536F,
0.2778F,-0.4904F, 0.0975F, 0.4157F,-0.4157F,-0.0975F, 0.4904F,-0.2778F,
0.1913F,-0.4619F, 0.4619F,-0.1913F,-0.1913F, 0.4619F,-0.4916F, 0.1913F,
0.0975F,-0.2778F, 0.4157F,-0.4904F, 0.4904F,-0.4157F, 0.2778F,-0.0975F };

const float Tt[64] = { 0.3536F, 0.4904F, 0.4619F, 0.4157F, 0.3536F, 0.2778F, 0.1913F, 0.0975F,
0.3536F, 0.4157F, 0.1913F,-0.0975F,-0.3536F,-0.4904F,-0.4619F,-0.2778F,
0.3536F, 0.2778F,-0.1913F,-0.4904F,-0.3536F, 0.0975F, 0.4619F, 0.4157F,
0.3536F, 0.0975F,-0.4619F,-0.2778F, 0.3536F, 0.4157F,-0.1913F,-0.4904F,
0.3536F,-0.0975F,-0.4619F, 0.2778F, 0.3536F,-0.4157F,-0.1913F, 0.4904F,
0.3536F,-0.2778F,-0.1913F, 0.4904F,-0.3536F,-0.0975F, 0.4619F,-0.4157F,
0.3536F,-0.4157F, 0.1913F, 0.0975F,-0.3536F, 0.4904F,-0.4619F, 0.2778F,
0.3536F,-0.4904F, 0.4619F,-0.4157F, 0.3536F,-0.2778F, 0.1913F,-0.0975F };
*/

//Bloques para obtener los bloques 8x8 a partir de la imagen y de la imagen a partir de bloques 8x8
int blocker_8_512_sq(UCHAR *cdata, UCHAR *bstring);
int imager_8_512_sq(int *bstring, UCHAR *cdata);

//Funciones para realizar la DCT y la IDCT
void QnD_TATt(UCHAR A[B * B], int A_int[B * B]);
void QnD_TtAT(int A[B * B], int A_int[B * B]);

//Operación necesaria para las multiplicaciones
void copy_L(long *copied, long *copy, int d);

//Función para la creación de la máscara
void block_mask_8_512_sq(UCHAR *mask, int d);

//Función para la aplicación de la máscara
void Mask(int A[B*B], UCHAR mask[B*B]);

//Operaciones con matrices
long * AxB_L(long M1[B * B], long M2[B * B]);
long * BxA_L(long M1[B * B], long M2[B * B]);
void MplusC(long M1[B * B], int cns);
void MshiftRight(long M1[B * B], int shift);
void UCHAR_to_L(UCHAR A[B * B], long A_long[B * B]);
void INT_to_L(int A[B * B], long A_long[B * B]);
void L_to_INT(long M1[B * B], int A_int[B * B]);


#endif