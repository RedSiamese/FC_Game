#pragma once

#include "FC_map.h"
#include "FC_car.h"

#include <vector>

#include <windows.h>
#include <thread>



extern class FC_CAR;

class FC_ENV {
public:
	FC_ENV(FC_MAP&);
	FC_ENV(const string&);
	~FC_ENV();

	FC_MAP& get_map();
	const IplImage* get_show();
	void refresh_show();
	int get_time();

	void add_car(FC_CAR*);
	void delete_car(const string&);

	FC_CAR & get_car(const string&);
	
	void set_car_xy(FC_CAR&, const FC_POINT&);
	void set_car_dir(FC_CAR&, float);
	void set_car_velocity(FC_CAR&, float);
	bool isinside(FC_CAR&);

	void trail_clear();
	void start();
	void set_time_speed(float);
	float get_time_speed();
	
	friend void refresh(void*);

private:
	FC_ENV(FC_ENV & ENV);

	vector<FC_POINT> len;
	
	bool isstart;
	double time;
	FC_MAP map;
	vector<FC_CAR*> cars;
	IplImage* show;			//用于显示，由于多线程，不将处理和显示划于一张图中
	IplImage* trail;		//用于保存路径
	IplImage* temp_show;	//用于处理
	thread thread_refresh;

	float time_speed;
};

