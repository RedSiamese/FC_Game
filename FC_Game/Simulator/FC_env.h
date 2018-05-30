#pragma once

#include "FC_map.h"
#include "FC_car.h"

#include <vector>
#include <string>

#include <windows.h>
#include <thread>
using namespace std;

class FC_ENV {
public:
	FC_ENV(FC_MAP & map)
		:map(map),
		show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)), 
		temp_show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		trail(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		thread_refresh(refresh,this){ 

		cvCvtColor(map.get_map(), trail, CV_GRAY2BGR);
		isstart=false; time=0; 
	}

	FC_ENV(const string & str)
		:map(FC_MAP(str)),
		show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		temp_show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		trail(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		thread_refresh(refresh, this) { 

		cvCvtColor(map.get_map(), trail, CV_GRAY2BGR);
		isstart=false; time = 0; 
	}

	FC_MAP& get_map() {
		return map;
	}

	~FC_ENV() { cvReleaseImage(&show); cvReleaseImage(&temp_show);}

	const IplImage* get_show(){ return show; }

	void refresh_show() {
		static CvScalar color[] = { CV_RGB(255, 0, 0) ,
			CV_RGB(0, 255, 0) ,
			CV_RGB(0, 0, 255) ,
			CV_RGB(255, 128, 0) ,
			CV_RGB(0, 255, 255) ,
			CV_RGB(255, 255, 0) };

		cvCopy(trail, temp_show);

		for (int i = cars.size() - 1; i > -1; i--) {

			FC_POINT loc = cars[i]->get_loc();
			float dir = cars[i]->get_dir();
			float dst_angle = cars[i]->get_dst_angle();
			float steering_angle = cars[i]->get_steering_angle();

			if (loc.Y - sin(dir) * 9 >= 0 && loc.Y - sin(dir) * 9 < trail->height&& loc.X - cos(dir) * 9 >= 0 && loc.X - cos(dir) * 9 < trail->width) {
				cvSet2D(trail, loc.Y - sin(dir) * 9, loc.X - cos(dir) * 9, color[i]);
			}

			cvCircle(temp_show, cvPoint(loc.X - cos(dir) * 9, loc.Y - sin(dir) * 9), 9, color[i]);

			cvLine(temp_show, cvPoint(loc.X, loc.Y), cvPoint(loc.X + cos(dir) * 50,
				loc.Y + sin(dir) * 50), color[i]);

			cvLine(temp_show, cvPoint(loc.X, loc.Y), cvPoint(loc.X + cos(dir + dst_angle) * 50,
				loc.Y + sin(dir + dst_angle) * 50), color[i]);

			cvLine(temp_show, cvPoint(loc.X, loc.Y), cvPoint(loc.X + cos(dir + steering_angle) * 50,
				loc.Y + sin(dir + steering_angle) * 50), CV_RGB(255, 255, 0));

		}

		cvCopy(temp_show, show);
	}

	int get_time() {
		return time;
	}

	void add_car(FC_CAR*car) {
		for (int i = 0; i < cars.size(); i++) {
			if (cars[i] == NULL) {
				cars[i] = car;
				return;
			}
		}
		cars.push_back(car);
	}

	FC_CAR & get_car(const string &name) {
		for (int i = 0; i < cars.size(); i++) {
			if (cars[i]->get_name() == name) {
				return *cars[i];
			}
		}
	}

	void delete_car(const string &name) {
		for (int i = 0; i < cars.size(); i++) {
			if (cars[i]->get_name() == name) {
				delete cars[i];
				cars[i] = NULL;
			}
		}
	}

	void start() { isstart = true; }

	friend void refresh(FC_ENV* env);

private:
	FC_ENV(FC_ENV & ENV);

	vector<FC_POINT> way;
	
	bool isstart;
	int time;
	FC_MAP map;
	vector<FC_CAR*> cars;
	IplImage* show;
	IplImage* trail;
	IplImage* temp_show;
	thread thread_refresh;
};

void refresh(FC_ENV* env) {
	while (1) {
		Sleep(1);
		env->refresh_show();

		if (env->isstart) {
			env->time++;
			for (int i = env->cars.size() - 1; i > -1; i--) {
				env->cars[i]->move();
			}
		}
		
	}
}