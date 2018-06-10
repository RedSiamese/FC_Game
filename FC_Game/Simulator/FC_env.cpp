
#include "FC_env.h"

void refresh(void*);

FC_ENV::FC_ENV(FC_MAP & map)
	:map(map),
	show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
	temp_show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
	trail(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
	thread_refresh(refresh, this) {

	cvCvtColor(map.get_map(), trail, CV_GRAY2BGR);
	isstart = false; time = 0;
	time_speed = 1;
}

FC_ENV::FC_ENV(const string & str)
	:map(str),
	show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
	temp_show(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
	trail(cvCreateImage(cvSize(map.get_map()->width, map.get_map()->height), 8, 3)),
	thread_refresh(refresh, this) {

	cvCvtColor(map.get_map(), trail, CV_GRAY2BGR);
	isstart = false; time = 0;
	time_speed = 1;
}

FC_MAP& FC_ENV::get_map() {
	return map;
}

FC_ENV::~FC_ENV() { cvReleaseImage(&show); cvReleaseImage(&temp_show); }

const IplImage* FC_ENV::get_show() { return show; }

void FC_ENV::refresh_show() {
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

int FC_ENV::get_time() {
	if(time_speed)
		return time / time_speed;
	else return time;
}

void FC_ENV::add_car(FC_CAR*car) {
	if (cars.size() >= 6) { return; }
	for (int i = 0; i < cars.size(); i++) {
		if (cars[i] == NULL) {
			cars[i] = car;
			return;
		}
	}
	cars.push_back(car);
}

FC_CAR & FC_ENV::get_car(const string &name) {
	for (int i = 0; i < cars.size(); i++) {
		if (cars[i]->get_name() == name) {
			return *cars[i];
		}
	}
}

void FC_ENV::set_car_xy(const string &name, const FC_POINT& p) {
	for (int i = 0; i < cars.size(); i++) {
		if (cars[i]->get_name() == name) {
			cars[i]->set_loc(p);
		}
	}
}

void FC_ENV::set_car_dir(const string &name, float dir) {
	for (int i = 0; i < cars.size(); i++) {
		if (cars[i]->get_name() == name) {
			cars[i]->set_dir(dir);
		}
	}
}

void FC_ENV::set_car_velocity(const string &name, float v) {
	for (int i = 0; i < cars.size(); i++) {
		if (cars[i]->get_name() == name) {
			cars[i]->set_velocity(v);
		}
	}
}

void FC_ENV::delete_car(const string &name) {
	for (int i = 0; i < cars.size(); i++) {
		if (cars[i]->get_name() == name) {
			delete cars[i];
			cars[i] = NULL;
		}
	}
}

void FC_ENV::trail_clear() { cvCvtColor(map.get_map(), trail, CV_GRAY2BGR); }

void FC_ENV::start() { isstart = true; }

void FC_ENV::set_time_speed(float zoom) {
	time_speed = (zoom > 1) ? zoom : (zoom == 0) ? 0 : 1;
}

float FC_ENV::get_time_speed() {
	return time_speed;
}

void refresh(void* p) {
	FC_ENV* env = (FC_ENV*)p;
	while (1) {
		if(env->time_speed)Sleep(1);
		env->refresh_show();

		if (env->isstart) {
			env->time++;
			for (int i = env->cars.size() - 1; i > -1; i--) {
				env->cars[i]->refresh_state(env->time_speed);
			}
		}

	}
}