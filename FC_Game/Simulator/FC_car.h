#pragma once

#define A_V (float)CV_PI*3.0F

#include "FC_math.h"
#include "FC_env.h"
#include "FC_camera.h"


#include "..\FC\FC_SDE.h"
#include "..\FC\FC_IMAGE.h"

#include <thread>
#include <opencv2\opencv.hpp>
#include <Windows.h>
#include <string>

using namespace std;

extern class FC_ENV;

struct FC_CAR_VELOCITY
{
	float linear_velocity;
	float angular_velocity;
};

class FC_CAR
{
public:
	FC_CAR(const string &name, FC_ENV &env_, FC_POINT &start, float dir, float wb, float cam_degree, float cam_h, float cam_d);
	FC_CAR(FC_CAR & car, const string &name);
	~FC_CAR();

	const string&get_name();

	const FC_POINT& get_loc();
	void set_loc(const FC_POINT&);
	float get_dir();
	void set_dir(float dir);
	float get_wheel_base();
	float get_steering_angle();
	float get_dst_angle();
	FC_CAR_VELOCITY get_velocity();

	void set_velocity(float velocity);

	void refresh_sight();
	const IplImage* get_sight();

	void set_dst_velocity(float v);
	void set_dst_angle(float ang);
	void set_steering_angle(float ang);
	void set_f_env(float);

	void refresh_state(float zoom);

	void control(float degree, float velocity);
	friend void camera_refresh(FC_CAR* car);

	const FC_camera &get_cam();

	bool is_refresh_finish();

	/*static*/
public:

	fc_track_info get_track_info();

private:

	bool refresh_finish;

	FC_CAR(FC_CAR & car);

	string name;

	FC_camera cam;

	FC_CAR_VELOCITY velocity;	//速度
	FC_POINT location;
	float dst_velocity;

	float f_env;
	float f_system;
	float f_wheel;

	float weight;

	float direction;

	float steering_angle;	//当前打角
	float dst_angle;		//目标打角

	float wheel_base;

	float sight_long;
	float sight_width;


	IplImage* sight;//摄像头视野
	FC_ENV& env;	//地图

	thread thread_refresh;

};