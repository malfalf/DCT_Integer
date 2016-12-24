#include<stdio.h>
#include <conio.h>
#include<math.h>
#include "qdbmp.h"
#include "Tasks.h"


//Development glogal dependecies
BMP*    bmp;
UCHAR   r, g, b;
UINT    width, height;
UINT    h, w, s, k;
//--

//special
int debug_outputBPM(UCHAR * image_tonal_data, const char * app);

//too specialized!!
UCHAR image_tonal_data_bss[512 * 512];		//8bit...+250kByte
UCHAR image_tonal_data_bss_ini[512 * 512];
float image_tonal_data_csn[512 * 512];		//8bit...+250kByte
UCHAR image_tonal_data_blk[512 * 512];
int image_tonal_data_blk_int[512 * 512];
int image_tonal_data_blk_int_out[512 * 512];


int main(int argc, char* argv[])
{
	
	char buff[100];
	sprintf(buff, "res/lena512.bmp");		//target img

	UCHAR cmp_mask[64];							//Mask vetor (pre-generated?)

	int nblocks = 0;

	/*
	//Multiplicamos las matrices DCT e IDCT por un valor múltiplo de 8 para que las divisiones sean desplazamientos
	for (k = 0; k < 64; k++) {
		DCT[k] = (long)(T[k] * pow(2,N));
		DCTt[k] = (long)(Tt[k] * pow(2,N));		
	}
	*/
	
	// Load file and extract data /////////////////////////////////////////////////////////////////////////////////////// 

	/* Read an image file */
	bmp = BMP_ReadFile(buff);
	/* If an error has occurred, notify and exit */
	BMP_CHECK_ERROR(stderr, -1);
	/* Get image's dimensions */
	width = BMP_GetWidth(bmp);
	height = BMP_GetHeight(bmp);
	/* Iterate through all the image's pixels */
	//Después de este bucle, tenemos en image_tonal_data_bss los pixels de la imagen colocados en un VECTOR de 512*512 valores
	for (h = 0; h < height; ++h)
	{
		for (w = 0; w < width; ++w)
		{
			/* Get pixel's RGB values */
			BMP_GetPixelIndex(bmp, w, h, &image_tonal_data_bss_ini[w + width*h]);

		}
	}

	
	//  Make blocks ///////////////////////////////////////////////////////////////////////////////////////
		
	//blocker operation (ver descripción en task.cpp)
	//Tendremos en image_tonal_data_blk_ulong los valores ordenador por bloques 8x8
	nblocks = blocker_8_512_sq(image_tonal_data_bss_ini, image_tonal_data_blk);

	// Input modifications ///////////////////////////////////////////////////////////////////////////////////////

	//Generamos la máscara. En este caso nos quedamos con los 6 valores de la esquina superior
	block_mask_8_512_sq(cmp_mask, 7);


	//Vas haciendo la DCT de todos los bloques 8x8
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

	//Aplicas la máscara a los bloques anteriores con la transformación
	for (int n = 0; n < nblocks; n++)
	{
		Mask(&image_tonal_data_blk_int[n * B * B], cmp_mask);
	}


	for (int n = 0; n < nblocks; n++)
	{
		int A_int_IDCT[B * B];
		
		QnD_TtAT(&image_tonal_data_blk_int[n * B * B], A_int_IDCT);

		for (int m = 0; m < B * B; m++) {
			image_tonal_data_blk_int_out[m + n * 64] = A_int_IDCT[m];
		}
	}


	//En imager volveremos a desplzar los valores obtenidos 2*N posiciones para hacer la división de nuevo,
	//ya que volvemos a multiplicar por DCT e IDCT escaladas 2^N
	imager_8_512_sq(image_tonal_data_blk_int_out, image_tonal_data_bss);


	debug_outputBPM(image_tonal_data_bss, "maskValue7");	

	//El siguiente bucle es para ver la diferencia entre la imagen de salida y la original
	//El if es porque si con UCHAR restas un valor a otro que es menor, el resultado es 255 y falsea la prueba.
	for (int k = 0; k < 512 * 512; k++) {
		if(image_tonal_data_bss_ini[k] > image_tonal_data_bss[k]) image_tonal_data_bss_ini[k] -= image_tonal_data_bss[k];
		else image_tonal_data_bss_ini[k] = image_tonal_data_bss[k] - image_tonal_data_bss_ini[k];
	}

	debug_outputBPM(image_tonal_data_bss_ini, "diffMaskValue7");

	////////////////////////////////////////////////////////////////////////////////////////

	/* Free all memory allocated for the image */
	BMP_Free(bmp);

	return 0;
}

int debug_outputBPM(UCHAR * image_tonal_data, const char * app)
{

	for (h = 0; h < height; ++h)
	{
		for (w = 0; w < width; ++w)
		{
			/* Set pixel's RGB values */
			BMP_SetPixelIndex(bmp, w, h, image_tonal_data[w + width*h]);

		}
	}

	/* Save result */
	char buff[50];
	sprintf(buff, "res/lena512_%s.bmp", app);
	BMP_WriteFile(bmp, buff);
	BMP_CHECK_ERROR(stderr, -2);
	return 0;

}