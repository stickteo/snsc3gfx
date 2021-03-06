// main.h
// teod 21-06-04

#ifndef MAIN_H
#define MAIN_H

// structs
// -------
struct TILE {
	char str[16];  //    0
	int unk0;      //    4
	int unk1;      //    8
	int unk2;      //  0xC
	int unk3;      // 0x10
	int tileSize;  // 0x14
	int palSize;   // 0x18
	int tileCount; // 0x1C
	int palCount;  // 0x20
	int tileOff;   // 0x24
	int palOff;    // 0x28
	int unk4;      // 0x2C
	
	int bpp;
};

struct MAP {
	char str[16]; //    0
	int unk0;     //    4
	int unk1;     //    8
	int unk2;     //  0xC
	int unk3;     // 0x10
	int width;    // 0x14
	int height;   // 0x16
	int size;     // 0x18
	int size2;    // 0x1A
	int off;      // 0x1C
};

// file routines
// -------------
int fget16 (FILE *f);
int fget32 (FILE *f);
int fgetColor (FILE *f);

void fput16 (uint32_t n, FILE *f);
void fput32 (uint32_t n, FILE *f);
void fputColor (int n, FILE *f);

// main reading
// ------------
int processMap (FILE *f, int **map2);
int processTile (FILE *f, int **pal2, char **tile2);

// image
// -----
void drawTile (char *img, char *tile, int meta, int x, int y, int w);
void drawBmp (struct BMP *b, char **img, char *tile, int *pal);

// file header
// -----------
void readTileInfo (FILE *f, struct TILE *ts);
void readMapInfo (FILE *f, struct MAP *ms);

void printTileInfo (struct TILE *ts);
void printMapInfo (struct MAP *ms);

void writeTileInfo (FILE *f, struct TILE *ts);
void writeMapInfo (FILE *f, struct MAP *ms);

void readPalette (FILE *f, struct TILE *ts, int *pal);

#endif
