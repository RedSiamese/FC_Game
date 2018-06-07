#pragma once

#include <opencv2/opencv.hpp>
#include <thread>
#include <string>

using namespace std;

void window(void*);

class FC_THREAD_SHOW
{
public:
	FC_THREAD_SHOW(const IplImage* img,const string &str) 
		:img(img),str(str), thread_window(window, this) {
	}
	~FC_THREAD_SHOW() {}

	void show_window() {
		cvShowImage(str.c_str(), img);
		cv::waitKey(1);
	}
	friend void window(void*);
private:
	const IplImage * img;
	const string str;
	thread thread_window;
};

void window(void *p) {
	FC_THREAD_SHOW* win = (FC_THREAD_SHOW*)p;
	cvNamedWindow(win->str.c_str(), 1);
	while (1) {
		win->show_window();
		Sleep(16);
	}
}