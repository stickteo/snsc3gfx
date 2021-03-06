// main.c
// teod 21-06-26
// + fix h/v flip
// teod 21-06-24
// + implement reverse

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bmp.h"
#include "main.h"

FILE *ft, *fm, *fb;
char *tile;
char *img;
int *map;
int *pal;

void done (void) {
	fclose(ft);
	fclose(fm);
	fclose(fb);
	free(tile);
	free(img);
	free(map);
	free(pal);
	return;
}

void reverseMode (char *t_n, char *m_n, char *T_n, char *M_n) {
	// t, m are files with headers; these 2 files will be edited
	// T, M are raw data to insert into the t, m files
	FILE *t, *m, *T, *M;
	long size;

	// get header info
	// ---------------
	struct TILE ts;
	struct MAP ms;
	//int pal[256];

	t = fopen(t_n,"rb");
	readTileInfo(t,&ts);
	printTileInfo(&ts);
	//readPalette(t,&ts,pal);
	char *pal;
	pal = malloc(ts.palSize);
	fseek(t,ts.palOff,SEEK_SET);
	for (int i=0; i<ts.palSize; i++) {
		pal[i] = fgetc(t);
	}
	
	fclose(t);

	m = fopen(m_n,"rb");
	readMapInfo(m,&ms);
	printMapInfo(&ms);
	fclose(m);

	// insert new tile data
	// --------------------
	T = fopen(T_n,"rb");
	fseek(T,0,SEEK_END);
	size = ftell(T);
	//              8 bits    pixel   tile
	// size bytes * ------ * ------ * ----- = size/n/8 tiles
	//               byte    n bits   64 px
	ts.tileCount = size/ts.bpp/8;
	ts.tileSize = size;
	ts.tileOff = 0x30; // header
	ts.palOff = 0x30+size; // header+tileSize
	
	t = fopen(t_n,"wb");
	writeTileInfo(t,&ts);

	// write tile data
	rewind(T);
	int c;
	c = fgetc(T);
	while(c != EOF) {
		fputc(c,t);
		c = fgetc(T);
	}

	// write palette data
	//for (int i=0; i<ts.palCount; i++) {
	//	fputColor(pal[i],t);
	//}
	for (int i=0; i<ts.palSize; i++) {
		fputc(pal[i],t);
	}

	fclose(t);
	fclose(T);

	// insert new map data
	// -------------------
	M = fopen(M_n,"rb");
	fseek(M,0,SEEK_END);
	size = ftell(M);
	ms.size = size;
	ms.size2 = size;
	ms.off = 0x20; // header
	
	m = fopen(m_n,"wb");
	writeMapInfo(m,&ms);

	// write map data
	rewind(M);
	c = fgetc(M);
	while(c != EOF) {
		fputc(c,m);
		c = fgetc(M);
	}

	fclose(m);
	fclose(M);
}

void readTileInfo (FILE *f, struct TILE *ts) {
	ts->str[0] = fgetc(f);
	ts->str[1] = fgetc(f);
	ts->str[2] = fgetc(f);
	ts->str[3] = fgetc(f);
	ts->str[4] = 0;

	ts->unk0 = fget32(f);
	ts->unk1 = fget32(f);
	ts->unk2 = fget32(f);
	ts->unk3 = fget32(f);

	ts->tileSize  = fget32(f);
	ts->palSize   = fget32(f);
	ts->tileCount = fget32(f);
	ts->palCount  = fget32(f);
	ts->tileOff   = fget32(f);
	ts->palOff    = fget32(f);

	ts->unk4 = fget32(f);

	// n bytes   8 bits   tile    n   bits
	// ------- * ------ * ----- = - * -----
	//   tile     byte    64 px   8   pixel
	// should be either 4 or 8 bpp
	ts->bpp = ts->tileSize / ts->tileCount / 8;
}

void printTileInfo (struct TILE *ts) {
	printf("-----------------------------------------------\n");
	printf("tile info (%s)\n",ts->str);
	printf("-----------------------------------------------\n");
	
	printf("%08x %08x %08x %08x\n",
		ts->unk0, ts->unk1, ts->unk2, ts->unk3);
	printf("%d tiles (%d bytes), %d bpp\n",
		ts->tileCount,ts->tileSize,ts->bpp);
	printf("%d colors (%d bytes)\n",
		ts->palCount,ts->palSize);
	printf("%08x\n", ts->unk4);
}

void writeTileInfo (FILE *f, struct TILE *ts) {
	fputc(ts->str[0],f);
	fputc(ts->str[1],f);
	fputc(ts->str[2],f);
	fputc(ts->str[3],f);
	
	fput32(ts->unk0,f);
	fput32(ts->unk1,f);
	fput32(ts->unk2,f);
	fput32(ts->unk3,f);

	fput32(ts->tileSize,f);
	fput32(ts->palSize,f);
	fput32(ts->tileCount,f);
	fput32(ts->palCount,f);
	fput32(ts->tileOff,f);
	fput32(ts->palOff,f);

	fput32(ts->unk4,f);
}

void readPalette (FILE *f, struct TILE *ts, int *pal) {
	fseek(f,ts->palOff,SEEK_SET);

	int i;
	for (i=0; i<ts->palCount; i++) {
		pal[i] = fgetColor(ft);
	}
	for (;i<256;i++) {
		pal[i] = 0;
	}
}

void readMapInfo (FILE *f, struct MAP *ms) {
	ms->str[0] = fgetc(f);
	ms->str[1] = fgetc(f);
	ms->str[2] = fgetc(f);
	ms->str[3] = fgetc(f);
	ms->str[4] = 0;

	ms->unk0 = fget32(f);
	ms->unk1 = fget32(f);
	ms->unk2 = fget32(f);
	ms->unk3 = fget32(f);

	ms->width  = fget16(f);
	ms->height = fget16(f);
	ms->size   = fget16(f);
	ms->size2  = fget16(f);
	
	ms->off = fget32(f);
}

void printMapInfo (struct MAP *ms) {
	printf("-----------------------------------------------\n");
	printf("map info (%s)\n",ms->str);
	printf("-----------------------------------------------\n");

	printf("%08x %08x %08x %08x\n",
		ms->unk0, ms->unk1, ms->unk2, ms->unk3);
	printf("%dx%d pixels (%d bytes)(%d bytes)\n",
		ms->width,ms->height,ms->size,ms->size2);
}

void writeMapInfo (FILE *f, struct MAP *ms) {
	fputc(ms->str[0],f);
	fputc(ms->str[1],f);
	fputc(ms->str[2],f);
	fputc(ms->str[3],f);

	fput32(ms->unk0,f);
	fput32(ms->unk1,f);
	fput32(ms->unk2,f);
	fput32(ms->unk3,f);

	fput16(ms->width,f);
	fput16(ms->height,f);
	fput16(ms->size,f);
	fput16(ms->size2,f);

	fput32(ms->off,f);
}


int main(int argc, char **argv) {
	ft=0; fm=0; fb=0;
	tile=0;

	// -t: tile file
	// -m: map file
	// -b: bmp file

	char *tArg, *mArg, *bArg;
	tArg=0;
	mArg=0;
	bArg=0;
	
	int revTrue, bmpTrue;
	bmpTrue = 0;
	revTrue = 0;

	char *TArg, *MArg;
	TArg=0;
	MArg=0;

	// process arguments
	for (int i=1; i<argc; i++) {
		if (argv[i][0] != '-') {
			printf("expected switch, got: %s\n", argv[i]);
			return 0;
		}
		switch(argv[i][1]) {
			case 't': tArg = argv[i+1]; i++; break;
			case 'm': mArg = argv[i+1]; i++; break;
			case 'b':
				bArg = argv[i+1];
				bmpTrue = 1;
				i++;
				break;
			case 'r': revTrue = 1; break;
			case 'T': TArg = argv[i+1]; i++; break;
			case 'M': MArg = argv[i+1]; i++; break;
			default:
				printf("invalid switch, got: %s\n", argv[i]);
				return 0;
		}
	}
	
	if (revTrue) {
		reverseMode(tArg,mArg,TArg,MArg);
		return 0;
	}

	// ---------------
	// bmp export mode
	// ---------------
	if (tArg==0 || mArg==0 || bArg==0) {
		printf("not enough files for bmp export\n");
		return 0;
	}

	ft = fopen(tArg, "rb");
	if(!ft) {
		printf("failed to open: %s\n",tArg);
		
		done();
		return 0;
	}
	fm = fopen(mArg, "rb");
	if(!fm) {
		printf("failed to open: %s\n",mArg);
		
		done();
		return 0;
	}
	fb = fopen(bArg, "wb");
	if(!fb) {
		printf("failed to open: %s\n",bArg);
		
		done();
		return 0;
	}
	
	// -------------
	// process tiles
	// -------------
	if(processTile(ft,&pal,&tile)){
		done();
		return 1;
	}

	// -----------
	// process map
	// -----------
	if(processMap(fm,&map)){
		done();
		return 1;
	}

	// -------------
	// construct bmp
	// -------------
	struct BMP b;
	drawBmp(&b,&img,tile,pal);
	bmpWrite(fb,&b);
	
	done();
	return 0;
}

int processTile (FILE *f, int **pal2, char **tile2) {
	struct TILE ts;

	readTileInfo(f,&ts);
	printTileInfo(&ts);

	// todo: other header types ?
	if (strcmp(ts.str,"CG4 ")) {
		printf("bad tile header!\n");

		done();
		return 1;
	}

	int t_count, t_start, t_size, p_start, p_count, bpp;
	t_count = ts.tileCount;
	t_start = ts.tileOff;
	t_size = ts.tileSize;
	p_start = ts.palOff;
	p_count = ts.palCount;
	bpp = ts.bpp;

	// grab tile data
	int t, c;
	
	char *tiles;
	tiles = malloc(t_count*64);
	*tile2 = tiles;
	fseek(ft,t_start,SEEK_SET);

	t=0;
	if (bpp == 4) {
		while (t<t_size) {
			c = fgetc(ft);
			tiles[t*2] = c&15;
			tiles[t*2+1] = c>>4;
			t++;
		}
	} else if (bpp == 8) {
		while (t<t_size) {
			tiles[t] = fgetc(ft);
			t++;
		}
	} else {
		printf("wrong bpp!\n");

		done();
		return 1;
	}

	// grab color data
	int *pal;
	int i;
	pal = malloc(4*256);
	*pal2 = pal;
	fseek(ft,p_start,SEEK_SET);

	for (i=0; i<p_count; i++) {
		pal[i] = fgetColor(ft);
	}
	for (;i<256;i++) {
		pal[i] = 0;
	}

	return 0;
}

int processMap (FILE *f, int **map) {
	FILE *fm;
	fm = f;

	struct MAP ms;
	readMapInfo(f,&ms);
	printMapInfo(&ms);

	// todo: other header types ?
	if (strcmp(ms.str,"SC4 ")) {
		printf("bad tile header!\n");

		done(); return 1;
	}

	// grab map data
	*map = malloc(4*32*32); // for some reason 32x32 tiles

	int *map2;
	map2 = *map;
	fseek(fm,ms.off,SEEK_SET);
	for (int i=0; i<32*32; i++) {
		map2[i] = fget16(fm);
	}

	return 0;
}

void drawTile (char *img, char *tile, int meta, int x, int y, int w) {
	int m,t,hv,p;
	m = meta;
	t = m&1023;
	//v = (m>>11)&1;
	//h = (m>>10)&1;
	hv = (m>>10)&3;
	p = (m>>12)*16; // palette offset

	int i,j;
	if (hv==0) { // no flip
		for (i=0; i<8; i++) {
			for (j=0; j<8; j++) {
				img[(y*8+i)*w+(x*8+j)] = tile[t*64+i*8+j] + p;
			}
		}
	} else if (hv==1) { // h
		for (i=0; i<8; i++) {
			for (j=0; j<8; j++) {
				img[(y*8+i)*w+(x*8+j)] = tile[t*64+i*8+(7-j)] + p;
			}
		}
	} else if (hv==2) { // v
		for (i=0; i<8; i++) {
			for (j=0; j<8; j++) {
				img[(y*8+i)*w+(x*8+j)] = tile[t*64+(7-i)*8+j] + p;
			}
		}
	} else { // both
		for (i=0; i<8; i++) {
			for (j=0; j<8; j++) {
				img[(y*8+i)*w+(x*8+j)] = tile[t*64+(7-i)*8+(7-j)] + p;
			}
		}
	}
}

void drawBmp (struct BMP *b, char **img2, char *tile, int *pal) {
	char *img;
	img = malloc(256*256);
	*img2 = img;

	b->width = 256;
	b->height = 256;
	b->bitCount = 8;
	b->image = img;
	b->palette = pal;

	int i,j;

	// for each tile
	for (i=0; i<32; i++) {
		for (j=0; j<32; j++) {
			drawTile(img,tile,map[i*32+j],j,i,256);
		}
	}

	return;
}

// --------------
// file functions
// --------------

int fget32 (FILE *f) {
	int n;
	n = fgetc(f);
	n = (fgetc(f)<<8) | n;
	n = (fgetc(f)<<16) | n;
	n = (fgetc(f)<<24) | n;
	return n;
}

int fget16 (FILE *f) {
	int n;
	n = fgetc(f);
	n = (fgetc(f)<<8) | n;
	return n;
}

int fgetColor (FILE *f) {
	// each color are 2 bytes
	int n, r, g, b;
	n = fget16(f);
	r = ( n     &31) * 255 / 31;
	g = ((n>> 5)&31) * 255 / 31;
	b = ((n>>10)&31) * 255 / 31;
	return (r<<16) | (g<<8) | b;
}

void fput16 (uint32_t n, FILE *f) {
	fputc(n    ,f);
	fputc(n>> 8,f);
}

void fput32 (uint32_t n, FILE *f) {
	fputc(n    ,f);
	fputc(n>> 8,f);
	fputc(n>>16,f);
	fputc(n>>24,f);
}

void fputColor (int n, FILE *f) {
	int r, g, b;
	r = ((n>>16)&255) * 31 / 255;
	g = ((n>> 8)&255) * 31 / 255;
	b = ( n     &255) * 31 / 255;
	fput16(r|(g<<5)|(b<<10),f);
}

