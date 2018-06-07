#pragma once

#define ABS(x) (((x)>0)?(x):-(x))
#define SGN(x) (((x)>0)?1:-1)
#define ROUND(x,L,R) (((x)<(L))?(L):((x)>(R))?(R):(x))
#define IF(x) ((x)?1:0)
#define MIN(x,y) (((x)>(y))?(y):(x))
#define MAX(x,y) (((x)>(y))?(x):(y))

#define fc_random_f(min,max)						(fc_xorshift128()*((max)-(min))+(min))

typedef struct FC_POINT
{
	float X;	//cm
	float Y;	//cm

	FC_POINT(){}
	FC_POINT(float x, float y) {
		X = x;
		Y = y;
	}
}FC_POINT, FC_VECTOR;

static unsigned long fcx = 5128238, fcy = 1124132, fcz = 152341, fcw = 2596459;
void fc_xorshift_init(unsigned int num) {
	fcx += ((float)num / 2293 - num / 2293) * 1000000;
	fcy += ((float)num / 2393 - num / 2393) * 1000000;
	fcz += ((float)num / 1913 - num / 1913) * 1000000;
	fcw += ((float)num / 2111 - num / 2111) * 1000000;
}

float fc_xorshift128() {
	unsigned long max = 0;
	unsigned long t = fcx ^ (fcx << 11);
	fcx = fcy; fcy = fcz; fcz = fcw;
	fcw = fcw ^ (fcw >> 19) ^ t ^ (t >> 8);
	return (float)fcw / (max - 1);
}