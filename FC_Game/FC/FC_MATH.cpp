#include "FC_MATH.h"

inline fc_point create_point(int16 x, int16 y) {
	return fc_point{ x,y };
}
inline uint32 fc_sqrt_short(uint16 x) {
	return fc_sqrt_short_8192(x) >> 13;
}
inline uint32 fc_sqrt_long(uint32 x) {
	return fc_sqrt_long_536870912(x) >> 29;
}
inline float fc_sqrt_short_to_float(uint16 x) {
	return 0.000122070313F * fc_sqrt_short_8192(x);
}
inline float fc_sqrt_long_to_float(uint32 x) {
	return 0.00000000186264514923095703125F * fc_sqrt_long_536870912(x);
}
inline float fc_sqrt(uint32 x) {
	return ((x) < 65536) ? fc_sqrt_short_to_float(x) : fc_sqrt_long_to_float(x);
}
inline float fc_sqrt_float(float x) {
	return InvSqrt(x);
}
inline float fc_atan(float x) {
	return (float)fc_atan_vector(256, x * 256) / 256;
}
inline void fc_clear_buf(void* buf, uint32 size) {
	for (pbyte p = (pbyte)buf, p_end = (pbyte)buf + size; p < p_end; *p++ = 0) {}
}
inline void fc_copy_buf(void*dst, void* scr, uint32 size) {
	for (pbyte p = (pbyte)(scr), p_end = p + (size), pd = (pbyte)(dst); p < p_end; *pd++ = *p++){}
}
inline vector_in_long distance_vector_in_long(vector_in_long vec_l) {
	fc_vector vec;
	P_TO_L(vec) = vec_l;
	uint32 module = VECTOR_MODULE_SQ(vec);
	int32 zoom = ((module < 65536) ? 8 : 0), len = FC_SQRT_LONG(module << zoom << zoom);
	return ((len) ? CONST_VECTOR(((int32)vec.x << 4 << zoom) / len, ((int32)vec.y << 4 << zoom) / len) : CONST_VECTOR(0, 0));
}
inline fc_vector distance_vector(fc_vector vec) {
	uint32 module = VECTOR_MODULE_SQ(vec);
	int32 zoom = ((module < 65536) ? 8 : 0), len = FC_SQRT_LONG(module << zoom << zoom);
	return ((len) ? fc_vector{ (int16)(((int32)vec.x << 4 << zoom) / len), (int16)(((int32)vec.y << 4 << zoom) / len) } : fc_vector{ 0, 0 });
}
inline float fc_exp(float x) {
	float base = 1.0F + x*0.00390625F;
	return base *= base *= base *= base *= base *= base *= base *= base *= base;
}

uint32 fc_sqrt_short_8192(uint16 x) {
	if (x) {
		uint32 din = 0x00FFFFFFUL / x;
		float f = x;
		uint16 e = ((int32&)f >> 23 & 0xff) - 127, ans = x - (x - ((uint32)1 << e) >> 1) >> (e >> 1);
		if (e & 1) { ans -= (ans >> 2); }
		ans = (((3 << 12) - (din * ans * ans >> 12)) * ans) >> 13;
		return ((3 << 12) - (din * ans * ans >> 12)) * ans;
	}
	else { return 0; }
}

uint64 fc_sqrt_long_536870912(uint32 x) {
	if (x) {
		uint64 din = 0x00FFFFFFFFFFFFFFULL / x;
		double f = x;
		uint32 e = ((int64&)f >> 52 & 0xff) - 2047, ans = (x - ((x - ((uint32)1 << e)) >> 1)) >> (e >> 1);
		if (e & 1) { ans -= (ans >> 2); }
		ans = (((3 << 28) - (din * ans * ans >> 28)) * ans) >> 29;
		ans = (((3 << 28) - (din * ans * ans >> 28)) * ans) >> 29;
		return ((3 << 28) - (din * ans * ans >> 28)) * ans;
	}
	else { return 0; }
}

float InvSqrt(float x) {
	float xhalf = 0.5f*x;
	int32 i = (int32&)x;
	i = 0x5f375a86 - (i >> 1);
	x = (float&)i;
	return 1 / (x*(1.5f - xhalf * x*x));
}

fc_curve convolution(fc_curve cur, pint32 kernel, int32 kernel_size) {
	fc_curve out = cur;
	CLEAR_BUF(out.point, sizeof(out.point));
	for (int8 i = 0; i < out.size; i++) {
		for (int8 j = -kernel_size / 2; j < (kernel_size + 1) / 2; j++) {
			out.point[i].x += cur.point[FALL_IN_(i - j, 0, cur.size - 1)].x*kernel[j + kernel_size / 2];
			out.point[i].y += cur.point[FALL_IN_(i - j, 0, cur.size - 1)].y*kernel[j + kernel_size / 2];
		}
	}
	return out;
}

static uint32 xxx = 51288, yyy = 11232, zzz = 1541, www = 25959;
void xorshift_init(unsigned int num) {
	xxx += ((float)num / 2293 - num / 2293) * 1000000;
	yyy += ((float)num / 2393 - num / 2393) * 1000000;
	zzz += ((float)num / 1913 - num / 1913) * 1000000;
	www += ((float)num / 2111 - num / 2111) * 1000000;
}

float xorshift128() {
	uint32 max = 0;
	uint32 t = xxx ^ (xxx << 11);
	xxx = yyy; yyy = zzz; zzz = www;
	www = www ^ (www >> 19) ^ t ^ (t >> 8);
	return (float)www / (max - 1);
}

int16 fc_atan_vector(int32 x, int32 y)
{
	static int16 angle[] = { 11520, 6801, 3593, 1824, 916, 458, 229, 115, 57, 29, 14, 7, 4, 2, 1 };

	static int32 x_new, y_new;
	static int32 angleSum, i;

	x <<= 16;
	y <<= 16;

	for (i = 0, angleSum = 0; i < 15; i++)
	{
		if (y > 0)
		{
			x_new = x + (y >> i);
			y = y - (x >> i);
			x = x_new;
			angleSum += angle[i];
		}
		else
		{
			x_new = x - (y >> i);
			y = y + (x >> i);
			x = x_new;
			angleSum -= angle[i];
		}
	}
	return angleSum;
}

void softmax(float* mdv, uint32 size) {
	float sum = 0;
	for (float* now = mdv, *end = mdv + size; now < end; sum += *now++) {}
	for (float* now = mdv, *end = mdv + size; now < end; *now++ /= sum) {}
}

fc_line fc_least_square_swap_xy(const fc_point* point, uint16 count) {
	int32 sxx = 0, sx = 0, sxy = 0, sy = 0, denominator = 0;
	for (uint16 i = 0; i < count; i++) {
		sxx += point[i].y * point[i].y;
		sx += point[i].y;
		sxy += point[i].y * point[i].x;
		sy += point[i].x;
		if ((sxy & 0x80000000) || (sxx & 0x80000000)) { return fc_line{ 0,0,2,0,0 }; }
	}
	return (denominator = sxx * count - sx * sx) ? fc_line{ (float)(sxy*count - sx * sy) / denominator ,(float)(sxx*sy - sx * sxy) / denominator ,0,0,0 } : fc_line{ 0,0,1,0,0 };
}

void fc_get_line_residual(fc_line* line, const fc_point* point, uint16 count) {
	int32 error = 0, residual = 0, variance = 0, k = line->k * 256, b = line->b * 256;
	for (uint16 i = 0; i < count; i++) {
		error = (point[i].x << 8) - (point[i].y*k + b);
		residual += error;
		variance += error * error >> 8;
		if (variance & 0x80000000)return;
	}
	line->residual = (float)residual / 256;
	line->variance = (float)variance / 256;
}

#include <math.h>
double gaussrand()
{
	static double V1, V2, S;
	static int32 phase = 0;
	double X;

	if (phase == 0) {
		do {
			double U1 = (double)random_f(0, 1);
			double U2 = (double)random_f(0, 1);

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;

		} while (S >= 1 || S == 0);
		X = V1 * sqrt(-2 * log(S) / S);
	}
	else
		X = V2 * sqrt(-2 * log(S) / S);
	phase = 1 - phase;
	return X;
}


float fc_ln(float x) {
	const float ln2 = 0.6931471805599F;
	int16 e = ((int32&)x >> 23 & 0xff) - 126;
	(long&)x &= 0x3FFFFFFF;
	(long&)x |= 0x3f800000;
	x *= 0.5F;
	float a = (x - 1) / (x + 1), b = a * a, c = b, d = 1;
	d += c * 0.3333333F;
	c *= b;
	d += c * 0.2F;
	c *= b;
	d += c * 0.142857142857F;
	return 2 * a*d + e * ln2;
}


float fc_ln_base(float x) {
	float a = (x - 1) / (x + 1), b = a * a, c = b, d = 1;
	d += c * 0.3333333F;
	c *= b;
	d += c * 0.2F;
	c *= b;
	d += c * 0.142857142857F;
	c *= b;
	d += c * 0.111111111111F;
	return 2 * a*d;
}
