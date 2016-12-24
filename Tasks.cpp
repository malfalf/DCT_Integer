#include "Tasks.h"
#include <conio.h>

//long DCT[64] = { 0,0,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 };

//long DCTt[64] = { 0,0,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 };

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

//Function deff

/*
	Takes the array with the image pixel values ordered in consecutive rows and transform it into and array of matrix sub-blocks of dimension B from the original image
	Primero mete en block_pattern los valores [0, 1, ..., 7, 512, 513, ..., 519, 1024, 1025, ..., 1031,..., 3583, 3584, ..., 3590] que ayuda con los saltos que hay que 
	dar en el vector de entrada para luego coger en orden los bloques de 8x8. Hay que recordar que cdata es el vector con todos los pixels de la imagen, pero que
	se van pasando todos seguidos. Es decir, para una imagen de 512 x 512, cdata tendrán:
		- valores de 0 a 511 la primera fila de la imagen, 
		- valores de 512 a 1023 la segunda fila de la imagen,
		-...
		- valores de 261632 a 262143 la última fila de la imagen.
	Pero nosotros en bstring queremos tener los primeros 64 valores el primer bloque 8x8, por lo que tenemos que coger los valores de 0 a 7, de 512 a 519, de 1024 a 1031... 
	y para eso se crea block_pattern
	Por lo tanto, en bstring tendremos los valores ordenador por bloques de 8x8 de la imagen.
	
	Finalmente, devuelve el número de bloques que se tienen (en este caso es fijo por tener una imagen de 512x512 y bloque de 8x8.
*/
int blocker_8_512_sq(UCHAR *cdata, UCHAR *bstring)
{
	const int n = 512, m = 512;
	int block_pattern[B*B], c = 0;
	
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		//Using n here is dependent of the oder chosen for the vector, in this case is rows so correspond to n
			block_pattern[c++] = j;
	//calculates the ofsets to extract the blocks from the vector and reorder the data in a block wise way of substrings of B^2 bits
	for (int i = 0; i < (m / B); i++)
		for (int j = 0; j < (n / B); j++)
			for (int k = 0; k < B*B; k++)
				bstring[k + j*(B*B) + i*(n / B)*(B*B)] = (UCHAR)(cdata[block_pattern[k] + B*j + (n / B)*B*B*i]);

	return (m / B)*(m / B);
	//Retunrs the number of blocks to ease operation
}

/*
	Takes an array of row arrays of matrix sub-blocks of dimension B from the original image matrix and transform it into an array with the pixel values ordered in consecutive rows  
*/
int imager_8_512_sq(int *bstring, UCHAR *cdata)
{
	const int n = 512, m = 512;
	int block_pattern[B*B], c = 0;

	//Generates a vector that describes the ubication of a 8x8 section on the image having its data distributed in a unidimensional vector ordered in rows
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		//Using n here is dependent of the oder chosen for the vector, in this case is rows so correspond to n
			block_pattern[c++] = j;

	//calculates the ofsets to extract the blocks from the vector and reorder the data in a block wise way of substrings of B^2 bits
	for (int i = 0; i < (m / B); i++)
		for (int j = 0; j < (n / B); j++)
			for (int k = 0; k < B*B; k++)
				cdata[block_pattern[k] + B*j + (n / B)*B*B*i] = (UCHAR)(bstring[k + j*(B*B) + i*(n / B)*(B*B)]);

	return (m / B)*(m / B);
	//Retunrs the number of blocks to ease operation
}

/*
	copies d values of the first parameter on to the second parameter being the parameters UCHAR vector pointers with dimension abobe d
*/
void copy_F(long *copied, long *copy, int d)
{
	for (int i = 0; i < d; i++)
		copy[i] = copied[i];
}

/*
	generates a mask consisting in a matrix of dimension B with ones in the superior corner untill the diagonal d, the generated matrix is then copied into the UCHAR vector pointer passed as first parameter
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


//F variant accepts float as second matrix
//Modifies ther first parameter pushing in the result, correspond to A
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
	copy_F(TMP, M1, B * B);
	return M1; //Returns the pointer of the firts parameter variable that also contains the result to allow recursive calls
}


// F variant accepts float as second matrix
//Modifies ther first parameter pushing in the result, correspond to A
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
	copy_F(TMP, M1, B * B);
	return M1; //Returns the pointer of the firts parameter variable that also contains the result to allow recursive calls
}


void INT_to_L(int M1[B * B], long A_long[B * B]) 
{
	for (int k = 0; k < B * B; k++) {
		A_long[k] = (long)M1[k];
	}
}

void UCHAR_to_L(UCHAR M1[B * B], long A_long[B * B])
{
	for (int k = 0; k < B * B; k++) {
		A_long[k] = (long)M1[k];
	}
}

void L_to_INT(long M1[B * B], int A_int[B * B]) {
	for (int k = 0; k < B * B; k++) {
		A_int[k] = (int)M1[k];
	}
}

void MplusC(long M1[B * B], int cns)
{
	for (int i = 0; i < B*B; i++) {
		M1[i] += cns;
	}
}

void MshiftRight(long M1[B * B], int shift)
{
	for (int i = 0; i < B*B; i++) {
		M1[i] = M1[i] >> 2 * shift;
	}

}

//
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
Función que te crea la matriz máscara. Con la variable precisión dices el número de "filas" que coges:
- Con valor 0, pones a 1 el valor 0,0 y el resto a 0
- Con valor 1, pones a 1 los valores 0,0; 0,1; 1,0 y el resto a 0
- ...
*/
void createMask(int precision) {
	//Para inicializar todos los valores de mask a 0
	for (int i = 0; i < B; i++) {
		for (int j = 0; j < B; j++) {
			mask[i*B + j] = 0;
		}
	}

	for (int i = 0; i < precision; i++) {
		for (int j = 0; j < precision; j++) {
			mask[i*B + j] = 1;
		}
	}
	mask[precision] = 1;
	mask[B*precision] = 1;
}


void Mask(int A[B*B], UCHAR mask[B*B]) {
	for (int p = 0; p < B*B; p++)
		A[p] *= mask[p];
}