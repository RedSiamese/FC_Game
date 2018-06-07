#pragma once

#include <opencv2\opencv.hpp>

class FC_camera
{
public:
	FC_camera(float degree, float h, float d) {
		c = cos(degree);
		s = sin(degree);
		this->d = d;
		this->h = h;
	}
	~FC_camera() {};

	inline CvPoint dpt_real(const CvPoint& xy) {
		return CvPoint((d*c + h)*xy.x / (s*xy.y + d * c),
			((d*c + h)*xy.y - h * d*s) / (s*xy.y + d * c));
	}

	inline CvPoint pt_real(const CvPoint& xy) {
		return CvPoint(d*xy.x / (d + h * c - s * xy.y),
			d*(c*xy.y + h * s) / (d + h * c - s * xy.y));
	}

	inline CvPoint dpt_point(IplImage*perspective, const IplImage*overlook, const CvPoint& xy) {
		CvPoint xyout = ijtoxy(perspective, xy);
		xyout = dpt_real(xyout);
		xyout.x /= 3;
		xyout = xytoij(overlook, xyout);
		return xyout;
	}

	inline CvPoint pt_point(const IplImage*perspective, IplImage*overlook, const CvPoint& xy) {
		CvPoint xyout = ijtoxy(overlook, xy);
		xyout.x *= 3;
		xyout = pt_real(xyout);
		xyout = xytoij(perspective, xyout);
		return xyout;
	}



	void pt_img(IplImage*perspective, const IplImage*overlook) {
		CvPoint xyout;
		for (int i = perspective->height - 1; i >= 0; i--) {
			for (int j = 0; j < perspective->width; j++) {
				xyout = dpt_point(perspective, overlook, cvPoint(j, i));
				if (xyout.x > overlook->width - 1) { break; }
				if (xyout.x < overlook->width && xyout.y < overlook->height && xyout.y >= 0 && xyout.x >= 0) {
					CV_IMAGE_ELEM(perspective, uchar, i, j) = CV_IMAGE_ELEM(overlook, uchar, xyout.y, xyout.x);
				}
			}
			if (xyout.y < -2) { break; }
		}
	}

	void dpt_img(const IplImage*perspective, IplImage*overlook) {
		CvPoint xyout;
		for (int i = 0; i < overlook->height; i++) {
			for (int j = 0; j < overlook->width; j++) {
				xyout = pt_point(perspective, overlook, CvPoint(j, i));
				if (xyout.x < perspective->width && xyout.y < perspective->height && xyout.y >= 0 && xyout.x >= 0) {
					CV_IMAGE_ELEM(overlook, uchar, i, j) = CV_IMAGE_ELEM(perspective, uchar, xyout.y, xyout.x);
				}
			}
		}
	}

private:
	float c;
	float s;
	float d;
	float h;

	CvPoint xytoij(const IplImage*img, const CvPoint &xy) {
		return CvPoint(xy.x + img->width / 2, img->height - xy.y);
	}

	CvPoint ijtoxy(const IplImage*img, const CvPoint &ij) {
		return CvPoint(ij.x - img->width / 2, img->height - ij.y);
	}
};