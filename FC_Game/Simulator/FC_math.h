#pragma once

typedef struct FC_POINT
{
	float X;	//cm
	float Y;	//cm

	FC_POINT();
	FC_POINT(float, float);
	void operator=(const FC_POINT&);
}FC_POINT, FC_VECTOR;

static unsigned long fcx = 5128238, fcy = 1124132, fcz = 152341, fcw = 2596459;
void fc_xorshift_init(unsigned int num);
float fc_xorshift128();

#define ABS(x) (((x)>0)?(x):-(x))
#define SGN(x) (((x)>0)?1:-1)
#define ROUND(x,L,R) (((x)<(L))?(L):((x)>(R))?(R):(x))
#define IF(x) ((x)?1:0)
#define MIN(x,y) (((x)>(y))?(y):(x))
#define MAX(x,y) (((x)>(y))?(x):(y))

#define fc_random_f(min,max)						(fc_xorshift128()*((max)-(min))+(min))