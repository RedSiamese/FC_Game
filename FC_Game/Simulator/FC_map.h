#pragma once

#include "FC_math.h"
#include <string>
#include <opencv2\opencv.hpp>

using namespace std;

class FC_MAP
{
public:
	FC_MAP(const string &path);
	FC_MAP(FC_MAP &map);
	FC_MAP(IplImage* map);
	~FC_MAP();

	IplImage* get_rectangle(float w,float h, const FC_VECTOR & loc, float dir);
	const IplImage*get_map();
private:

	IplImage* fc_map;

};

