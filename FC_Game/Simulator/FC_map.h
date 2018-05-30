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


FC_MAP::FC_MAP(const string &path)
{
	fc_map = cvLoadImage(path.c_str(),CV_LOAD_IMAGE_ANYDEPTH);
}

inline FC_MAP::FC_MAP(FC_MAP & map){
	this->fc_map = cvCloneImage(map.get_map());
}

inline FC_MAP::FC_MAP(IplImage * map)
{
	this->fc_map = cvCloneImage(map);
}

FC_MAP::~FC_MAP()
{
	cvReleaseImage(&fc_map);
}

inline IplImage * FC_MAP::get_rectangle(float w, float h, const FC_VECTOR & loc, float dir)
{

	//把图逆时针旋转dir度，取wh矩形
	double degree = dir / CV_PI * 180 + 90.0;
	IplImage *img_transformation = cvCreateImage(CvSize(w, h), 8, 1);

	CvPoint2D32f center(float(loc.X), float(loc.Y));
	float m[6];
	CvMat M = cvMat(2, 3, CV_32F, m);
	cv2DRotationMatrix(center, degree, 1, &M);
	
	//平移
	m[2] = -(cos(degree / 180 * CV_PI)*loc.X + sin(degree / 180 * CV_PI)*loc.Y) + img_transformation->width / 2;
	m[5] = -(-sin(degree / 180 * CV_PI)*loc.X + cos(degree / 180 * CV_PI)*loc.Y) + img_transformation->height;

	//变换图像，并用黑色填充其余值  
	cvWarpAffine(fc_map, img_transformation, &M, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(100));

	return img_transformation;
}

inline const IplImage * FC_MAP::get_map()
{
	return fc_map;
}

