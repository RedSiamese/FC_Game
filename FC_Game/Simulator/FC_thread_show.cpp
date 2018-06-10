#include "FC_thread_show.h"

FC_THREAD_SHOW::FC_THREAD_SHOW(const IplImage* img, const string &str)
	:img(img), str(str), thread_window(window, this) {
}
FC_THREAD_SHOW::~FC_THREAD_SHOW() {}

void FC_THREAD_SHOW::show_window() {
	cvShowImage(str.c_str(), img);
	cv::waitKey(1);
}


void window(void *p) {
	FC_THREAD_SHOW* win = (FC_THREAD_SHOW*)p;
	cvNamedWindow(win->str.c_str(), 1);
	while (1) {
		win->show_window();
		cvWaitKey(16);
	}
}