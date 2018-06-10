#pragma once

#include "FC_map.h"
#include "FC_car.h"

#include <vector>

#include <windows.h>
#include <thread>



extern class FC_CAR;

class FC_ENV {
public:
	FC_ENV(FC_MAP & map);
	FC_ENV(const string & str);
	~FC_ENV();

	FC_MAP& get_map();
	const IplImage* get_show();
	void refresh_show();
	int get_time();
	void add_car(FC_CAR*car);
	FC_CAR & get_car(const string &name);

	void set_car_xy(const string &name, const FC_POINT& p);
	void set_car_dir(const string &name, float dir);
	void set_car_velocity(const string &name, float v);
	void delete_car(const string &name);
	void trail_clear();
	void start();
	void set_time_speed(float zoom);

	
	friend void refresh(void*);

private:
	FC_ENV(FC_ENV & ENV);

	vector<FC_POINT> len;
	
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

