#pragma once

#include <opencv2\opencv.hpp>

class FC_camera
{
public:
	FC_camera(float degree, float h, float d);
	~FC_camera();

	inline CvPoint dpt_real(const CvPoint& xy);

	inline CvPoint pt_real(const CvPoint& xy);

	inline CvPoint dpt_point(IplImage*perspective, const IplImage*overlook, const CvPoint& xy);

	inline CvPoint pt_point(const IplImage*perspective, IplImage*overlook, const CvPoint& xy);



	void pt_img(IplImage*perspective, const IplImage*overlook);

	void dpt_img(const IplImage*perspective, IplImage*overlook);

private:
	float c;
	float s;
	float d;
	float h;

	CvPoint xytoij(const IplImage*img, const CvPoint &xy);

	CvPoint ijtoxy(const IplImage*img, const CvPoint &ij);
};