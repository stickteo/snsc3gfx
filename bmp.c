// bmp.c
// teod 21-06-24

#include <stdio.h>
#include <stdint.h>

#include "bmp.h"

static void fput16 (uint32_t n, FILE *f) {
	fputc(n    ,f);
	fputc(n>> 8,f);
}

static void fput32 (uint32_t n, FILE *f) {
	fputc(n    ,f);
	fputc(n>> 8,f);
	fputc(n>>16,f);
	fputc(n>>24,f);
}

void bmpWriteImage4 (FILE *f, struct BMP *b) {
	int i, j;
	char *image;

	for(i=0; i<b->height; i++){
		image = b->image + i*b->width;
		j=0;
		while(j<=b->width-8){
			fputc((image[j]<<4)|(image[j+1]&15),f);
			fputc((image[j+2]<<4)|(image[j+3]&15),f);
			fputc((image[j+4]<<4)|(image[j+5]&15),f);
			fputc((image[j+6]<<4)|(image[j+7]&15),f);
			j+=8;
		}
		switch(b->width - j){
			case 0: break;
			case 1:
				fputc(image[j]<<4,f);
				fputc(0,f);
				fputc(0,f);
				fputc(0,f);
				break;
			case 2:
				fputc((image[j]<<4)|(image[j+1]&15),f);
				fputc(0,f);
				fputc(0,f);
				fputc(0,f);
				break;
			case 3:
				fputc((image[j]<<4)|(image[j+1]&15),f);
				fputc(image[j+2]<<4,f);
				fputc(0,f);
				fputc(0,f);
				break;
			case 4:
				fputc((image[j]<<4)|(image[j+1]&15),f);
				fputc((image[j+2]<<4)|(image[j+3]&15),f);
				fputc(0,f);
				fputc(0,f);
				break;
			case 5:
				fputc((image[j]<<4)|(image[j+1]&15),f);
				fputc((image[j+2]<<4)|(image[j+3]&15),f);
				fputc(image[j+4]<<4,f);
				fputc(0,f);
				break;
			case 6:
				fputc((image[j]<<4)|(image[j+1]&15),f);
				fputc((image[j+2]<<4)|(image[j+3]&15),f);
				fputc((image[j+4]<<4)|(image[j+5]&15),f);
				fputc(0,f);
				break;
			case 7:
				fputc((image[j]<<4)|(image[j+1]&15),f);
				fputc((image[j+2]<<4)|(image[j+3]&15),f);
				fputc((image[j+4]<<4)|(image[j+5]&15),f);
				fputc(image[j+6]<<4,f);
				break;
		}
	}
}

void bmpWriteImage8 (FILE *f, struct BMP *b) {
	int i, j;
	char *image;
	
	for(i=0; i<b->height; i++){
		image = b->image + i*b->width;
		j=0;
		while(j<=b->width-4){
			fputc(image[j],f);
			fputc(image[j+1],f);
			fputc(image[j+2],f);
			fputc(image[j+3],f);
			j+=4;
		}
		switch(b->width - j){
			case 0: break;
			case 1:
				fputc(image[j],f);
				fputc(0,f);
				fputc(0,f);
				fputc(0,f);
				break;
			case 2:
				fputc(image[j],f);
				fputc(image[j+1],f);
				fputc(0,f);
				fputc(0,f);
				break;
			case 3:
				fputc(image[j],f);
				fputc(image[j+1],f);
				fputc(image[j+2],f);
				fputc(0,f);
				break;
		}
	}
}

void bmpWrite (FILE *f, struct BMP *b) {
	// file header (14 bytes)
	fputs("BM",f);
	int size, offset, pSize;
	if(b->bitCount == 4) {
		pSize = 16;
	} else {
		pSize = 256;
	}

	offset = 14+40+pSize*4;
	size = offset+(b->width*b->height*b->bitCount/8);
	fput32(size,f);
	fput32(0,f);
	fput32(offset,f);
	
	// image header (40 bytes)
	fput32(40,f); // header size
	fput32(b->width,f);
	fput32(0-b->height,f); // negative means data goes
	                      // from top to bottom
	fput16(1,f); // 1 image plane
	fput16(b->bitCount,f);
	fput32(0,f); // no compression
	fput32(0,f); // compressed size
	fput32(0,f); // x pixels/meter
	fput32(0,f); // y pixels/meter
	fput32(0,f); // colors used in palette
	fput32(0,f); // important colors

	// palette
	for (int i=0; i<pSize; i++) {
		fput32(b->palette[i],f); // 0x00RRGGBB
	}

	// image
	if (b->bitCount == 4)
		bmpWriteImage4(f,b);
	else
		bmpWriteImage8(f,b);

}

