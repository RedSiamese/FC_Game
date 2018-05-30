
#ifndef FC_MATH_H
#define FC_MATH_H

#include "FC_SDE.h"

#define TRUE			1L
#define FALSE			-1L

#define PI										3.1415926535897932384626433832795L
#define FLT_MAX									3.402823466e+38F
#define DBL_MAX								    1.7976931348623158e+308L
#define UINT_MAX								0xffffffffU
#define INT_MAX									0x7fffffffL
#define random_f(min,max)						(xorshift128()*((max)-(min))+(min))
#define random_gauss(v,e)						(gaussrand()*v+e)

//输出层不能ReLU，用softmax，梯度直接概率减
#define ReLU(x)									(((x)>0)?x:0)				//Rectified Linear Units
#define dReLU(x)								(((x)>0)?1:0)

#define S_n1_p1(x)								(2/(1 + exp(-x))-1)		//Sigmoid -1~1
#define dS_n1_p1(x)								((x+1)*(1-(x+1)/2))

#define S_0_p1(x)								(1/(1 + exp(-x)))			//Sigmoid 0~1
#define dS_0_p1(x)								(x*(1-x))

#define DIV4A5I(a,b)							(((a)+((b)>>1))/(b))
#define FALL_IN_(x,L,R)							(((x)<(L))?(L):((x)>(R))?(R):(x))
#define IF_FALL_IN_(x,L,R)						(((x)<(L))?(0):((x)>(R))?(0):(1))
#define MIN(x,y)								(((x)>(y))?(y):(x))
#define MAX(x,y)								(((x)>(y))?(x):(y))
#define ABS(x)									(((x)>=0)?(x):-(x))
#define SGN(x)									(((x)>=0)?(1):(-1))
#define _IF_(x)									((x!=0)?1:0)

#define IF_FALL_IN_MAP(map,point)				(((point).x>-1&&(point).x<(map).width-1&&(point).y>-1&&(point).y<(map).height-1)?1:0)

#define P_TO_L(point)							((long&)(point))
#define CONST_VECTOR(x,y)						(((long)(y)<<(8*sizeof(short)))|((short)(x)&0xffff))
#define VECTOR_MINUS(vec1,vec2)					CONST_VECTOR((vec1).x-(vec2).x, (vec1).y-(vec2).y)
#define VECTOR_PLUS(vec1,vec2)					CONST_VECTOR((vec1).x+(vec2).x, (vec1).y+(vec2).y)
#define VECTOR_MEAN(vec1,vec2,percentage)		CONST_VECTOR(((vec1).x*(percentage)+(vec2).x*(100-(percentage)))/100, ((vec1).y*(percentage)+(vec2).y*(100-(percentage)))/100)
#define ORTHOGONAL(vec,dir)						CONST_VECTOR(-(vec).y*(dir),(vec).x*(dir))
#define VECTOR_CP(vec1,vec2)					((vec1).x*(vec2).y - (vec2).x*(vec1).y)
#define VECTOR_DP(vec1,vec2)					((vec1).x*(vec2).x + (vec2).y*(vec1).y)
#define VECTOR_MODULE_SQ(vec)					((vec).y*(vec).y+(vec).x*(vec).x+1)
#define EUCLIDEAN_DISTANCE_SQ(point1,point2)	(((point1).x-(point2).x)*((point1).x-(point2).x)+((point1).y-(point2).y)*((point1).y-(point2).y))
#define VECTOR_MODULE(vec)						fc_sqrt_short(VECTOR_MODULE_SQ(vec))
#define EUCLIDEAN_DISTANCE(point1,point2)		fc_sqrt_short(EUCLIDEAN_DISTANCE_SQ(point1,point2))
#define VECTOR_TO_SLOPE(vec)					(((vec).x)?(float)(vec).y/(vec).x:FLT_MAX)
#define VECTOR_TO_INV_SLOPE(vec)				(((vec).y)?(float)(vec).x/(vec).y:FLT_MAX)
#define DIRECTION_VECTOR(vec)					distance_vector_in_long(P_TO_L(vec))
#define VECTOR_ZOOM(vec,zoom)					CONST_VECTOR((vec).x*zoom,(vec).y*zoom)
#define VECTOR_SHIFTING(vec,num)				(((num)>0)?CONST_VECTOR((vec).x<<(num),(vec).y<<(num)):CONST_VECTOR((vec).x<<-(num),(vec).y<<-(num)))
#define VECTOR_SYM_XY(vec)						CONST_VECTOR((vec).y,(vec).x)

#define POINT_MOVE_X(point,vec,step)			CONST_VECTOR((point).x+(step)*SGN((vec).x), (point).y+(step)*(vec).y/ABS((vec).x))
#define POINT_MOVE_Y(point,vec,step)			CONST_VECTOR((point).x+(step)*(vec).x/ABS((vec).y), (point).y+(step)*SGN((vec).y))
#define POINT_MOVE(point,vec,step)				((ABS((vec).x)>ABS((vec).y))?POINT_MOVE_X((point),(vec),(step)):POINT_MOVE_Y((point),(vec),(step)))
#define POINT_MOVE_UP(point)					((point).y--)
#define POINT_MOVE_DOWN(point)					((point).y++)
#define POINT_MOVE_LEFT(point)					((point).x--)
#define POINT_MOVE_RIGHT(point)					((point).x++)

#define GET_PIX_XY(map,x,y)						(*((map).buf+(map).width*(y)+(x)))
#define GET_PIX_P(map,point)					GET_PIX_XY((map),(point).x,(point).y)

#define GRAY_TO_BOOL(map,point)					((GET_PIX_P((map),(point)) > (map).gray)?1:-1)

#define CURVE_PUSH_BACK(curve)					(*((curve).point+(curve).size++))
#define CURVE_BACK_N(curve,n)					(*((curve).point+(curve).size-n))
#define CURVE_BACK(curve)						CURVE_BACK_N(curve,1)
#define CURVE_LONGER(cur1,cur2)					(((cur1).size>(cur2).size)?(cur1):(cur2))
#define CURVE_SHORTER(cur1,cur2)				(((cur1).size<(cur2).size)?(cur1):(cur2))	

#define FC_TAN(x)								((float)fc_atan_vector(1024, (x) * 1024) / 256)
#define FC_SQRT_FLOAT(x)						InvSqrt(x)
#define FC_SQRT(x)								(((x)<65536)?fc_sqrt_short_to_float(x):fc_sqrt_long_to_float(x))
#define FC_SQRT_LONG(x)							(((x)<65536)?fc_sqrt_short(x):fc_sqrt_long(x))


#define CLEAR_BUF(pbuf,size)					for (char* p = (char*)(pbuf), *p_end = p + (size); p < p_end;*p++ = 0)	
#define COPY_BUF(dst,scr,size)					for (char* p = (char*)(scr), *p_end = p + (size),*pd=(char*)(dst); p < p_end;*pd++ = *p++)
#define FC_CLEAR(buf,size)						CLEAR_BUF(buf,size)
#define FC_COPY(dst,scr,size)					COPY_BUF(dst,scr,size)

typedef long vector_in_long;

typedef struct fc_point
{
	int16 x, y;

#ifdef __cplusplus
	fc_point() {
		x = y = 0;
	}
	fc_point(int32 l) {
		x = *((int16*)&l+1);
		y = (int16&)l;
	}
	fc_point(short ix, short iy) {
		x = ix;
		y = iy;
	}
#endif

}fc_point, fc_vector;


typedef struct fc_curve
{
	uint8		size;
	uint8		fptr;
	uint8		pptr[8];
	fc_point	point[256];
}fc_curve;


typedef struct fc_line
{
	float	k;
	float	b;
	byte	warning;

	float	residual;
	float	variance;
}fc_line;

void		xorshift_init(unsigned int num);
float		xorshift128();
int16		fc_atan_vector(int32 x, int32 y);
uint32		fc_sqrt_short_8192(uint16 x);
uint64		fc_sqrt_long_536870912(uint32 x);
float		InvSqrt(float x);

void		softmax(float* mdv, uint32 size);
fc_curve	convolution(fc_curve cur, pint32 kernel, int32 kernel_size);

fc_line		fc_least_square_swap_xy(const fc_point* point, uint16 count);
void		fc_get_line_residual(fc_line* line, const fc_point* point, uint16 count);
double		gaussrand();

float		fc_ln(float x);

___inline fc_point create_point(int16 x, int16 y) {
	return fc_point{ x,y };
}
___inline uint32 fc_sqrt_short(uint16 x) {
	return fc_sqrt_short_8192(x) >> 13;
}
___inline uint32 fc_sqrt_long(uint32 x) {
	return fc_sqrt_long_536870912(x) >> 29;
}
___inline float fc_sqrt_short_to_float(uint16 x) {
	return 0.000122070313F * fc_sqrt_short_8192(x);
}
___inline float fc_sqrt_long_to_float(uint32 x) {
	return 0.00000000186264514923095703125F * fc_sqrt_long_536870912(x);
}
___inline float fc_sqrt(uint32 x) {
	return ((x) < 65536) ? fc_sqrt_short_to_float(x) : fc_sqrt_long_to_float(x);
}
___inline float fc_sqrt_float(float x) {
	return InvSqrt(x);
}
___inline float fc_atan(float x) {
	return (float)fc_atan_vector(256, x * 256) / 256;
}
___inline void fc_clear_buf(void* buf, uint32 size) {
	for (pbyte p = (pbyte)buf, p_end = (pbyte)buf + size; p < p_end; *p++ = 0) {}
}
___inline void fc_copy_buf(void*dst, void* scr, uint32 size) {
	for (pbyte p = (pbyte)(scr), p_end = p + (size), pd = (pbyte)(dst); p < p_end; *pd++ = *p++){}
}
___inline vector_in_long distance_vector_in_long(vector_in_long vec_l) {
	fc_vector vec;
	P_TO_L(vec) = vec_l;
	uint32 module = VECTOR_MODULE_SQ(vec);
	int32 zoom = ((module < 65536) ? 8 : 0), len = FC_SQRT_LONG(module << zoom << zoom);
	return ((len) ? CONST_VECTOR(((int32)vec.x << 4 << zoom) / len, ((int32)vec.y << 4 << zoom) / len) : CONST_VECTOR(0, 0));
}
___inline fc_vector distance_vector(fc_vector vec) {
	uint32 module = VECTOR_MODULE_SQ(vec);
	int32 zoom = ((module < 65536) ? 8 : 0), len = FC_SQRT_LONG(module << zoom << zoom);
	return ((len) ? fc_vector{ (int16)(((int32)vec.x << 4 << zoom) / len), (int16)(((int32)vec.y << 4 << zoom) / len) } : fc_vector{ 0, 0 });
}
___inline float fc_exp(float x) {
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
#endif