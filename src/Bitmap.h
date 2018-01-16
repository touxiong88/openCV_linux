#ifndef __BITMAP_H_
#define __BITMAP_H_

#include "fflog.h"


struct Bitmap{
	double angle;
	double deltS;
	uint8_t *data;
	int full;
	int width;
	int height;
};

#endif
