
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

inline fc_point create_point(int16 x, int16 y);
inline uint32 fc_sqrt_short(uint16 x);
inline uint32 fc_sqrt_long(uint32 x);
inline float fc_sqrt_short_to_float(uint16 x);
inline float fc_sqrt_long_to_float(uint32 x);
inline float fc_sqrt(uint32 x);
inline float fc_sqrt_float(float x);
inline float fc_atan(float x);
inline void fc_clear_buf(void* buf, uint32 size);
inline void fc_copy_buf(void*dst, void* scr, uint32 size);
inline vector_in_long distance_vector_in_long(vector_in_long vec_l);
inline fc_vector distance_vector(fc_vector vec);
inline float fc_exp(float x);

#endif