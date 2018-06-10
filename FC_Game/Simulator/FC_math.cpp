#include "FC_math.h"

FC_POINT::FC_POINT() {}
FC_POINT::FC_POINT(float x, float y) {
	X = x;
	Y = y;
}

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