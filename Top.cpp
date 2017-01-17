/*
Código creado por Carlos Pastor Ramírez y Miguel Ángel Alfonso Alfonso
para la asignatura Lenguajes y Herramientas de Diseño Digital.

Este código se utilizará en la obtención de la DCT y la IDCT de 8x8 valores
de una imagen de entrada conocida de 512x512 pixeles (lena.bmp). Para ello,
se utilizarán 4 grandes bloques:
- blocker -> ordena los píxeles de la imagen en bloques de 8x8.
- QnD_TATt -> realizará la DCT de los bloques 8x8 que se le vayan pasando como argumento
- QnD_TtAT -> realizará la DCT inversa de los bloques 8x8 que se le vayan pasando como argumento
- imager -> función inversa a blocker. Colocará los píxeles para una correcta visualización de la imagen.

En este código se utilizarán únicamente valores con formato entero.

Asimismo, cabe destacar que se hará uso de la librería qdbmp, no creada por nosotros.

Curso: 2016-2017

*/

#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "qdbmp.h"
#include "Tasks.h"


//Variables globales
BMP*    bmp;
UCHAR   r, g, b;
UINT    width, height;
UINT    h, w, s, k;
//--

//Función que nos permitirá representar las imágenes obtenidas en diferentes puntos del código
int debug_outputBPM(UCHAR * image_tonal_data, const char * app);

//Vectores donde guardaremos los valores de los píxeles en diferentes puntos del código
//Estos vectores están pensados para una imagen de 512x512 píxeles
UCHAR image_tonal_data_bss[512 * 512];		
UCHAR image_tonal_data_bss_ini[512 * 512];
float image_tonal_data_csn[512 * 512];		
UCHAR image_tonal_data_blk[512 * 512];
int image_tonal_data_blk_int[512 * 512];
int image_tonal_data_blk_int_out[512 * 512];

//Función main
int main(int argc, char* argv[])
{
	
	char buff[100];
	sprintf(buff, "res/lena512.bmp");		//Trabajaremos con la imagen lena512.bmp

	UCHAR cmp_mask[64];						//Vector de 64 valores para la máscara (8x8)

	int nblocks = 0;

	
	//Multiplicamos las matrices DCT e IDCT por un valor múltiplo de 8 para que las divisiones sean desplazamientos
	//for (k = 0; k < 64; k++) {
	//	DCT[k] = (long)(T[k] * pow(2,N));
	//	DCTt[k] = (long)(Tt[k] * pow(2,N));		
	//}
	
	
	// Obtenemos la imagen y valores particulares/ 

	//Almacenamos la imagen en bmp
	bmp = BMP_ReadFile(buff);
	//Se notifica si se produce un error
	BMP_CHECK_ERROR(stderr, -1);
	//Obtenemos las dimensiones de la imagen
	width = BMP_GetWidth(bmp);
	height = BMP_GetHeight(bmp);
	
	//Después de este bucle, tenemos en image_tonal_data_bss los pixels de la imagen colocados en un VECTOR de 512*512 valores
	for (h = 0; h < height; ++h)
	{
		for (w = 0; w < width; ++w)
		{
			//Obtenemos los valores de los píxeles
			BMP_GetPixelIndex(bmp, w, h, &image_tonal_data_bss_ini[w + width*h]);

		}
	}


	// Obtenemos los bloques de 8x8 píxeles
		
	//Para entender el funcionamiento de la función blocker_8_512_sq ver descripción en task.cpp
	//Tendremos en image_tonal_data_blk los valores ordenador por bloques 8x8
	//nblocks indicará el número de bloques 8x8 que tiene la imagen
	nblocks = blocker_8_512_sq(image_tonal_data_bss_ini, image_tonal_data_blk);


	//Para entender el funcionamiento de la función block_mask_8_512_sq ver descripción en task.cpp
	//Generamos la máscara. En este caso nos quedamos con los 6 valores de la esquina superior izquierda
	block_mask_8_512_sq(cmp_mask, 3);


	//Aplicamos la DCT
	//Para entender el funcionamiento de la función QnD_TATt ver descripción en task.cpp
	//En este bucle, se van realizando las DCT de los todos los bloques de 8x8 píxeles
	for (int n = 0; n < nblocks; n++)
	{
		int A_int_DCT[B * B];
		QnD_TATt(&image_tonal_data_blk[n * B * B], A_int_DCT);

		//Esta parte es únicamente para tener toda la DCT metida en una matriz de 256*256.
		//En Vivado, esta parte no es necesaria, ya que podemos ir pasando al siguiente bloque bloques de 8x8
		for (int m = 0; m < B * B; m++) {
			image_tonal_data_blk_int[m + n * 64] = A_int_DCT[m];
		}	
	}

	
	//Aplicación de la máscara a los bloques anteriores con la transformación
	for (int n = 0; n < nblocks; n++)
	{
		Mask(&image_tonal_data_blk_int[n * B * B], cmp_mask);
	}


	//Aplicamos la IDCT
	//Para entender el funcionamiento de la función QnD_TtAT, ver descripción en task.cpp
	//En este bucle, se van realizando las IDCT de los todos los bloques de 8x8 píxeles
	for (int n = 0; n < nblocks; n++)
	{
		int A_int_IDCT[B * B];
		
		QnD_TtAT(&image_tonal_data_blk_int[n * B * B], A_int_IDCT);

		//Esto es solo para poner toda la imagen en una matriz para luego poder visualizarla
		for (int m = 0; m < B * B; m++) {
			image_tonal_data_blk_int_out[m + n * 64] = A_int_IDCT[m];
		}
	}


	//Recomposición de la imagen
	//Para entender el funcionamiento de la función imager_8_512_sq ver descripción en task.cpp
	imager_8_512_sq(image_tonal_data_blk_int_out, image_tonal_data_bss);

	//Generamos la imagen obtenida tras todo el proceso
	debug_outputBPM(image_tonal_data_bss, "test");

#if 0
	//Esta parte es solo para mostrar diferentes resultados que se han ido obteniendo durante la simulación
	//El siguiente bucle es para ver la diferencia entre la imagen de salida y la original
	//El if es porque si con UCHAR restas un valor a otro que es menor, el resultado es 255 y falsea la prueba.
	//Función para obtener una imagen de los resultados
	for (int k = 0; k < 512 * 512; k++) {
		if(image_tonal_data_bss_ini[k] > image_tonal_data_bss[k]) image_tonal_data_bss_ini[k] -= image_tonal_data_bss[k];
		else image_tonal_data_bss_ini[k] = image_tonal_data_bss[k] - image_tonal_data_bss_ini[k];
	}

	debug_outputBPM(image_tonal_data_bss_ini, "diffmask8");
#endif

	//Liberamos la memoria reservada para la imagen
	BMP_Free(bmp);

	//Si todo ha ido bien devolvemos un 0
	return 0;
}


/*
Function : debug_outputBPM
--------------------------

Creará una imagen a partir del vector de entrada que se le pase.

image_tonal_data: vector de 256x256 que queremos transformar en imagen.
app:apéndice que se le pondrá al nombre de la imagen para distinguirla de otras.

returns: 0 si todo ha ido bien o -2 en caso de error.

*/
int debug_outputBPM(UCHAR * image_tonal_data, const char * app)
{

	for (h = 0; h < height; ++h)
	{
		for (w = 0; w < width; ++w)
		{
			// Activas los valores de los píxeles 
			BMP_SetPixelIndex(bmp, w, h, image_tonal_data[w + width*h]);
		}
	}

	//Creas la imagen y compruebas que todo OK
	char buff[50];
	sprintf(buff, "res/N8%s.bmp", app);
	BMP_WriteFile(bmp, buff);
	BMP_CHECK_ERROR(stderr, -2);
	return 0;
}