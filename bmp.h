// bmp.h
// teod 21-06-24

#ifndef BMP_H
#define BMP_H

struct BMP {
	int width;
	int height;
	int bitCount;
	char *image;
	int *palette;
};

void bmpWrite (FILE *, struct BMP *);

#endif
