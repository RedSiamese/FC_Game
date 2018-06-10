
#ifndef FC_IMAGE_H
#define FC_IMAGE_H

#include "FC_SDE.h"
#include "FC_MATH.h"

#define RIGHT				1
#define LEFT				-1


typedef struct fc_map
{
	uint8			width;
	uint8			height;
	uint8			gray;

	uint8			external;
	pbyte			buf;
	byte_map		map;

}fc_map;

typedef enum fc_mark
{
	null,
	destination
}fc_mark;

typedef struct fc_track_info
{

	fc_mark					track_mark;

	fc_curve				mid_curve;
	fc_curve				right_curve;
	fc_curve				left_curve;

	float					feature_degree[4];
	float					feature_length[4];
	fc_curve				feature_vector;
	fc_line					mid_least_square;

	//	fc_track_classify		track_type;

}fc_track_info;

fc_map create_map(uint8 width, uint8 height, const char* str);
void free_map(fc_map* map);
fc_track_info create_track_info(fc_map map);

void gray_to_bool(fc_map* map);
void fc_get_track_edge(fc_track_info* track_info, fc_map map);
void fc_search_edge(fc_curve* curve, fc_map map, int8 site);
bool fc_fix(fc_map map, fc_curve* curve, int8 site);
uint8 fc_fix_back(fc_map map, fc_curve* curve);
void fc_get_track_mid(fc_track_info* track_info, fc_map map);
void get_track_mark(fc_track_info* track_info, fc_map);


#endif