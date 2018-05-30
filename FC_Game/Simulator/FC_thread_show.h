#pragma once

#include "FC_math.h"
#include "FC_map.h"
#include "FC_car.h"
#include <thread>
#include <string>
#include <windows.h>

using namespace std;

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
	friend void window(FC_THREAD_SHOW *win);
private:
	const IplImage * img;
	const string str;
	thread thread_window;
};

void window(FC_THREAD_SHOW *win) {
	cvNamedWindow(win->str.c_str(), 1);
	while (1) {
		win->show_window();
		Sleep(16);
	}
}