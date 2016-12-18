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
unsigned long image_tonal_data_blk_ulong[512 * 512];		//8bit...+250kByte
long image_tonal_data_blk[512 * 512];
float image_tonal_data_csn[512 * 512];		//8bit...+250kByte
UCHAR image_tonal_data_blk_dummy[512 * 512];

int main(int argc, char* argv[])
{
	
	char buff[100];
	sprintf(buff, "res/lena512.bmp");		//target img

	UCHAR cmp_mask[64];							//Mask vetor (pre-generated?)

	int nblocks = 0;

	//Multiplicamos las matrices DCT e IDCT por un valor múltiplo de 8 para que las divisiones sean desplazamientos
	for (k = 0; k < 64; k++) {
		DCT[k] = (long)(T[k] * pow(2,N));
		DCTt[k] = (long)(Tt[k] * pow(2,N));		
	}
	
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
			BMP_GetPixelIndex(bmp, w, h, &image_tonal_data_bss[w + width*h]);

		}
	}

	
	//  Make blocks ///////////////////////////////////////////////////////////////////////////////////////
		
	//blocker operation (ver descripción en task.cpp)
	//Tendremos en image_tonal_data_blk_ulong los valores ordenador por bloques 8x8
	nblocks = blocker_8_512_sq(image_tonal_data_bss, image_tonal_data_blk_ulong);
	//En image_total_data_csn tienes los valores de image_tonal_data_blk transformados en float
	//copy_F_U(image_tonal_data_blk, image_tonal_data_csn, 512*512);

	k = 0;

	//Para pasar de unsigned long a long (lo he hecho porque me daba problema el signo al principio.
	for (k = 0; k < 512 * 512; k++) {
		image_tonal_data_blk[k] = (long)(image_tonal_data_blk_ulong[k]);

	}


	// Input modifications ///////////////////////////////////////////////////////////////////////////////////////

	//Generamos la máscara. En este caso nos quedamos con los 6 valores de la esquina superior
	block_mask_8_512_sq(cmp_mask, 8);

	//Vas haciendo la DCT de todos los bloques 8x8
	for (int n = 0; n < nblocks; n++)
	{
		QnD_TATt(&image_tonal_data_blk[n * B * B]);
	}

	k = 0;

	//Aquí hacemos el desplazamiento de N bits para hacer una división. Como hemos hecho una multiplicación tanto en
	//la matriz DCT como en la IDCT, hay que dividir entre 2 veces ese valor. He hecho que la multiplicación sea múltiplo
	//de 8 para que sean desplazamientos en lugar de divisiones.
	for (k = 0; k < 512 * 512; k++) {
		image_tonal_data_blk[k] = image_tonal_data_blk[k] >> 2*N;

	}


	//Aplicas la máscara a los bloques anteriores con la transformación
	for (int n = 0; n < nblocks; n++)
	{
		Mask(&image_tonal_data_blk[n * B * B], cmp_mask);
	}
	//En image_tonal_data_blk tienes los valores en formato int de nuevo
	//copy_U_F(image_tonal_data_csn, image_tonal_data_blk, 512 * 512);
	
	
	// imager_8_512_sq(image_tonal_data_blk, image_tonal_data_bss);
	// debug_outputBPM(image_tonal_data_bss, "csnt_cmp");

	for (int n = 0; n < nblocks; n++)
	{
		QnD_TtAT(&image_tonal_data_blk[n * B * B]);
	}


	//Para pasar un array de unsigned long a imager (de nuevo, he tenido problemas con los signos del long
	k = 0;

	for (k = 0; k < 512 * 512; k++) {
		image_tonal_data_blk_ulong[k] = (unsigned long)(image_tonal_data_blk[k]);

	}

	//En imager volveremos a desplzar los valores obtenidos 2*N posiciones para hacer la división de nuevo,
	//ya que volvemos a multiplicar por DCT e IDCT escaladas 2^N
	imager_8_512_sq(image_tonal_data_blk_ulong, image_tonal_data_bss);

	debug_outputBPM(image_tonal_data_bss, "_sinmascara10");	

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