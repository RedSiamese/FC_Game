#pragma once

#include "FC_car.h"

#include <vector>

#include <windows.h>
#include <thread>

void refresh(void*);

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
		time_speed = 1;
	}

	FC_ENV(const string & str)
		:map(str),
		show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		temp_show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		trail(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
		thread_refresh(refresh, this) { 

		cvCvtColor(map.get_map(), trail, CV_GRAY2BGR);
		isstart=false; time = 0; 
		time_speed = 1;
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
		return time/ time_speed;
	}

	void add_car(FC_CAR*car) {
		if (cars.size() >= 6) { return; }
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

	void set_car_xy(const string &name, const FC_POINT& p) {
		for (int i = 0; i < cars.size(); i++) {
			if (cars[i]->get_name() == name) {
				cars[i]->set_loc(p);
			}
		}
	}

	void set_car_dir(const string &name, float dir) {
		for (int i = 0; i < cars.size(); i++) {
			if (cars[i]->get_name() == name) {
				cars[i]->set_dir(dir);
			}
		}
	}

	void set_car_velocity(const string &name, float v) {
		for (int i = 0; i < cars.size(); i++) {
			if (cars[i]->get_name() == name) {
				cars[i]->set_velocity(v);
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

	void trail_clear(){ cvCvtColor(map.get_map(), trail, CV_GRAY2BGR); }

	void start() { isstart = true; }

	void set_time_speed(float zoom) {
		time_speed = (zoom > 1) ? zoom : 1;
	}

	friend void refresh(void*);

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

	float time_speed;
};

void refresh(void* p) {
	FC_ENV* env = (FC_ENV*)p;
	while (1) {
		Sleep(1);
		env->refresh_show();

		if (env->isstart) {
			env->time++;
			for (int i = env->cars.size() - 1; i > -1; i--) {
				env->cars[i]->refresh_state(env->time_speed);
			}
		}
		
	}
}