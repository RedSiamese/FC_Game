#pragma once

#include <opencv2/opencv.hpp>
#include <thread>
#include <string>

using namespace std;

void window(void*);

class FC_THREAD_SHOW
{
public:
	FC_THREAD_SHOW(const IplImage* img, const string &str);
	~FC_THREAD_SHOW();

	void show_window();
	friend void window(void*);
private:
	const IplImage * img;
	const string str;
	thread thread_window;
};
