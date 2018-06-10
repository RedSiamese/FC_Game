
#include "FC_car.h"

void camera_refresh(FC_CAR* car);

FC_CAR::FC_CAR(const string &name, FC_ENV &env_, FC_POINT &start, float dir, float wb, float cam_degree, float cam_h, float cam_d)
	:env(env_), thread_refresh(camera_refresh, this), cam(cam_degree, cam_h, cam_d)
{
	this->name = name;
	velocity = FC_CAR_VELOCITY{ 0.0F,A_V };
	location = start;
	direction = dir;

	wheel_base = wb;
	sight_long = 64;
	sight_width = 192;

	steering_angle = 0.0;
	dst_angle = 0.0;
	dst_velocity = 0.0;

	f_system = 1.0;
	f_wheel = 8.0;
	weight = 1.0;


	sight = cvCreateImage(CvSize(sight_width, sight_long), 8, 1);
	memset(sight->imageData, 0, sight->widthStep*sight->height);

	refresh_finish = false;
}

FC_CAR::FC_CAR(FC_CAR & car, const string &name) :
	env(car.env), thread_refresh(camera_refresh, this), cam(car.cam)
{
	this->name = name;
	velocity = car.velocity;
	location = car.location;
	direction = car.direction;

	wheel_base = car.wheel_base;
	sight_long = car.sight_long;
	sight_width = car.sight_width;

	steering_angle = car.steering_angle;
	dst_angle = car.dst_angle;
	dst_velocity = car.dst_velocity;

	f_system = car.f_system;
	f_wheel = car.f_wheel;
	weight = car.weight;

	sight = cvCloneImage(car.sight);

	refresh_finish = car.refresh_finish;
}

FC_CAR::~FC_CAR()
{
	cvReleaseImage(&sight);
}

 const string & FC_CAR::get_name()
{
	return name;
}

 const FC_POINT& FC_CAR::get_loc()
{
	return location;
}

 void FC_CAR::set_loc(const FC_POINT & p)
{
	location = p;
}

 float FC_CAR::get_dir()
{
	return direction;
}

 void FC_CAR::set_dir(float dir)
{
	direction = dir;
}

 FC_CAR_VELOCITY FC_CAR::get_velocity()
{
	return velocity;
}

 void FC_CAR::set_velocity(float v)
{
	velocity.linear_velocity = v;
}

void FC_CAR::refresh_state(float zoom)
{
	float time_pass = 0.001 / zoom;
	//P=TN/9550其中N由v代替
	//速度变化
	//功率
	float P = f_system * dst_velocity;
	//转矩
	float F = P / (velocity.linear_velocity + 0.00001);
	float f = SGN(velocity.linear_velocity)*f_system;

	float f_wheel_temp = f_wheel + fc_random_f(-velocity.linear_velocity / 500, 0.2);
	f += fc_random_f(-0.1, 0.1);

	//轮转角
	steering_angle += SGN(dst_angle - steering_angle)*ROUND(velocity.angular_velocity*time_pass, 0, ABS(dst_angle - steering_angle));
	ROUND(steering_angle, -CV_PI / 3, CV_PI / 3);

	//计算转向半径
	float R = wheel_base / sin(steering_angle / 2);
	//算出最小转向半径
	float min_R = weight * pow(velocity.linear_velocity / 100, 2) / f_wheel_temp * 100;
	//算出打滑时的阻力
	f += ROUND(ABS(weight * pow(velocity.linear_velocity / 100, 2) / R * 100) - f_wheel_temp, 0, 10);

	f = -f * SGN(velocity.linear_velocity)*IF(velocity.linear_velocity);
	F = ROUND(F + f, -f_wheel_temp, f_wheel_temp);

	//速度变化
	float a = F / weight * 100;
	velocity.linear_velocity += a * time_pass;

	//移动
	//无转角
	if (ABS(steering_angle) < FLT_EPSILON) {
		float dir_degree = direction;

		location.X += cos(dir_degree) * velocity.linear_velocity*time_pass;
		location.Y += sin(dir_degree) * velocity.linear_velocity*time_pass;
	}
	//转弯
	else {

		R = MAX(ABS(R), ABS(min_R))*SGN(R);

		//转角
		float degree = velocity.linear_velocity*time_pass / R;

		float l = 2 * R*sin(degree / 2);

		location.X += l * cos(direction + degree / 2);
		location.Y += l * sin(direction + degree / 2);

		direction += degree;

	}

}

 float FC_CAR::get_wheel_base()
{
	return wheel_base;
}

 float FC_CAR::get_steering_angle()
{
	return steering_angle;
}

 float FC_CAR::get_dst_angle()
{
	return dst_angle;
}

 const IplImage * FC_CAR::get_sight()
{
	return sight;
}

 void FC_CAR::set_dst_velocity(float v)
{
	dst_velocity = v;
}

 void FC_CAR::set_dst_angle(float ang)
{
	dst_angle = (ABS(ang) < CV_PI / 3) ? (ang) : (SGN(ang)*CV_PI / 3);
}

 void FC_CAR::set_steering_angle(float ang)
{
	steering_angle = ang;
}

 void FC_CAR::refresh_sight()
{
	IplImage * temp_sight;
	FC_MAP& map = env.get_map();
	temp_sight = map.get_rectangle(map.get_map()->width / 2, map.get_map()->height / 2, location, direction);
	cam.pt_img(sight, temp_sight);

	cvReleaseImage(&temp_sight);
}

 void FC_CAR::control(float degree, float velocity)
{
	set_dst_velocity(velocity);
	set_dst_angle(degree);
}

 const FC_camera & FC_CAR::get_cam()
{
	return cam;
}

 bool FC_CAR::is_refresh_finish()
{
	return refresh_finish;
}

/*static*/
fc_track_info FC_CAR::get_track_info(FC_CAR& car)
{

	FC_camera cam(car.get_cam());

	for (; !car.is_refresh_finish(););
	IplImage* sight = cvCloneImage(car.get_sight());

	IplImage* sight_t = cvCreateImage(CvSize(sight->width / 2, sight->height * 2), 8, 1);
	cam.dpt_img(sight, sight_t);

	fc_map Image = create_map(sight_t->width, sight_t->height, NULL);
	memcpy(Image.buf, sight_t->imageData, sight_t->width*sight_t->height * sizeof(byte));

	gray_to_bool(&Image);

	fc_track_info TI = create_track_info(Image);

	fc_get_track_edge(&TI, Image);
	fc_get_track_mid(&TI, Image);

	free_map(&Image);

	CvPoint p;
	for (int i = 0; i < TI.mid_curve.size; i++) {
		p = cvPoint(TI.mid_curve.point[i].x, TI.mid_curve.point[i].y);
		p = cam.pt_point(sight, sight_t, p);
		TI.mid_curve.point[i] = fc_point(FALL_IN_(p.x, 0, sight->width - 1), FALL_IN_(p.y, 0, sight->height));
	}
	for (int i = 0; i < TI.right_curve.size; i++) {
		p = cvPoint(TI.right_curve.point[i].x, TI.right_curve.point[i].y);
		p = cam.pt_point(sight, sight_t, p);
		TI.right_curve.point[i] = fc_point(FALL_IN_(p.x, 0, sight->width - 1), FALL_IN_(p.y, 0, sight->height));
	}
	for (int i = 0; i < TI.left_curve.size; i++) {
		cvPoint(TI.left_curve.point[i].x, TI.left_curve.point[i].y);
		p = cam.pt_point(sight, sight_t, p);
		TI.left_curve.point[i] = fc_point(FALL_IN_(p.x, 0, sight->width - 1), FALL_IN_(p.y, 0, sight->height));
	}

	cvReleaseImage(&sight);
	cvReleaseImage(&sight_t);
	return TI;
}


void camera_refresh(FC_CAR* car) {
	while (1) {
		Sleep(4);

		car->refresh_finish = false;
		car->refresh_sight();
		car->refresh_finish = true;

	}
}
