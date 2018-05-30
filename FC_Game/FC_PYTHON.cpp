#pragma once

#include <python.h>

#include "Simulator\FC_map.h"
#include "Simulator\FC_env.h"
#include "Simulator\FC_car.h"
#include "Simulator\FC_math.h"
#include "Simulator\FC_thread_show.h"


#include "FC\FC_SDE.h"
#include "FC\FC_IMAGE.h"


#include <iostream>

FC_ENV *game_env;

FC_THREAD_SHOW *win_env;



void game_init(string path) {

	LARGE_INTEGER litmp;
	QueryPerformanceCounter(&litmp);
	fc_xorshift_init(litmp.QuadPart % 100000);

	game_env = new FC_ENV(path);
//	game_env->add_car(new FC_CAR("0",game_env->get_map(), FC_POINT{ 380.0, 50.0 }, 0, 20, -17.32050808 / 360 * 2 * CV_PI, 0.45, 27.0));

	new FC_THREAD_SHOW(game_env->get_show(), "env");
	Sleep(500);
}

void add_car(const char*name, FC_POINT start_point) {
	game_env->add_car(new FC_CAR(name, game_env->get_map(), start_point, 0, 20, -17.32050808 / 360 * 2 * CV_PI, 0.45, 27.0));
}

void delete_car(const char*name) {
	game_env->delete_car(name);
}

void ctrl(const char* name,float degree,float velocity) {
	game_env->get_car(name).control(degree, velocity);
}

float get_velocity(const char* name) {
	return game_env->get_car(name).get_velocity().linear_velocity;
}

const IplImage* get_car_sight(const char* name) {
	return game_env->get_car(name).get_sight();
}

void show_car_sight(const char* name) {
	new FC_THREAD_SHOW(game_env->get_car(name).get_sight(), name);
}

fc_track_info get_ti(const char* name) {

	FC_camera cam(game_env->get_car(name).get_cam());

	for (; !game_env->get_car(name).is_refresh_finish(););
	IplImage* sight = cvCloneImage(game_env->get_car(name).get_sight());

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


int get_time() {
	return game_env->get_time();
}


void start() {
	game_env->start();
}
void sleep(int i) {
	cvWaitKey(i);
}



static PyObject *Extest_game_init(PyObject *self, PyObject *args) {
	char* s;
	if (!(PyArg_ParseTuple(args, "s", &s))) {
		return NULL;
	}
	game_init(s);

	return (PyObject *)Py_BuildValue("");
}

static PyObject *Extest_add_car(PyObject *self, PyObject *args) {
	
	char* s;
	double x,y;

	if (!(PyArg_ParseTuple(args, "s(dd)", &s, &x, &y))) {
		return NULL;
	}
	add_car(s, FC_POINT{(float)x,(float)y});

	return (PyObject *)Py_BuildValue("");
}

static PyObject *Extest_delete_car(PyObject *self, PyObject *args) {
	char* s;
	if (!(PyArg_ParseTuple(args, "s", &s))) {
		return NULL;
	}
	delete_car(s);
	return (PyObject *)Py_BuildValue("");
}


static PyObject *Extest_ctrl(PyObject *self, PyObject *args) {
	double degree, velocity; char*s;
	if (!(PyArg_ParseTuple(args, "sdd", &s, &degree, &velocity))) {
		return NULL;
	}
	ctrl(s, degree, velocity);
	//需要把c中计算的结果转成python对象，s代柄笾符串对象类型。
	return (PyObject *)Py_BuildValue("");
}


static PyObject *Extest_get_velocity(PyObject *self, PyObject *args) {
	char*s;
	if (!(PyArg_ParseTuple(args, "s", &s))) {
		return NULL;
	}
	return (PyObject *)Py_BuildValue("d", (double)get_velocity(s));
}

static PyObject *Extest_show_car_sight(PyObject *self, PyObject *args) {
	char*s;
	if (!(PyArg_ParseTuple(args, "s", &s))) {
		return NULL;
	}
	show_car_sight(s);
	return (PyObject *)Py_BuildValue("");
}

static PyObject *Extest_get_car_sight(PyObject *self, PyObject *args) {
	char*s;

	if (!(PyArg_ParseTuple(args, "s", &s))) {
		return NULL;
	}

	for (; !game_env->get_car(s).is_refresh_finish(););
	IplImage* img = cvCloneImage(get_car_sight(s));

	PyObject* img_list = PyList_New(img->height);
	for (int i = 0; i < img->height; i++) {
		PyObject* img_list_width = PyList_New(img->width);
		for (int j = 0; j < img->width; j++) {
			
			CvScalar s = cvGet2D(img, i, j);

			PyList_SET_ITEM(img_list_width, j, Py_BuildValue("d",
				(double)s.val[0]/255.0));
		}
		PyList_SET_ITEM(img_list, i, img_list_width);
		//Py_DECREF(img_list_width);
	}

	cvReleaseImage(&img);
	return img_list;
}

static PyObject *Extest_get_ti(PyObject *self, PyObject *args) {
	char*s;

	if (!(PyArg_ParseTuple(args, "s", &s))) {
		return NULL;
	}
	fc_track_info ti = get_ti(s);
	
	PyObject* mid_list = PyList_New(ti.mid_curve.size);
	for (int i = 0; i < ti.mid_curve.size; i++) {
		PyObject* mid_point = PyList_New(2);

		PyList_SET_ITEM(mid_point, 0, Py_BuildValue("i",
			ti.mid_curve.point[i].x));
		PyList_SET_ITEM(mid_point, 1, Py_BuildValue("i",
			ti.mid_curve.point[i].y));

		PyList_SET_ITEM(mid_list, i, mid_point);
	}
	PyObject* left_list = PyList_New(ti.left_curve.size);
	for (int i = 0; i < ti.left_curve.size; i++) {
		PyObject* left_point = PyList_New(2);

		PyList_SET_ITEM(left_point, 0, Py_BuildValue("i",
			ti.left_curve.point[i].x));
		PyList_SET_ITEM(left_point, 1, Py_BuildValue("i",
			ti.left_curve.point[i].y));

		PyList_SET_ITEM(left_list, i, left_point);
	}
	PyObject* right_list = PyList_New(ti.right_curve.size);
	for (int i = 0; i < ti.right_curve.size; i++) {
		PyObject* right_point = PyList_New(2);

		PyList_SET_ITEM(right_point, 0, Py_BuildValue("i",
			ti.right_curve.point[i].x));
		PyList_SET_ITEM(right_point, 1, Py_BuildValue("i",
			ti.right_curve.point[i].y));

		PyList_SET_ITEM(right_list, i, right_point);
	}
	PyObject *dic_ti = PyDict_New();
	PyDict_SetItemString(dic_ti, "mid", mid_list);
	PyDict_SetItemString(dic_ti, "right", right_list);
	PyDict_SetItemString(dic_ti, "left", left_list);
	Py_DECREF(mid_list);
	Py_DECREF(right_list);
	Py_DECREF(left_list);
	return dic_ti;
}

static PyObject *Extest_get_time(PyObject *self, PyObject *args) {
	return Py_BuildValue("i",get_time());
}



static PyObject *Extest_start(PyObject *self, PyObject *args) {
	start();
	return Py_BuildValue("");
}

static PyObject *Extest_sleep(PyObject *self, PyObject *args) {
	int i;
	if (!(PyArg_ParseTuple(args, "i", &i))) {
		return NULL;
	}
	sleep(i);
	return (PyObject *)Py_BuildValue("");
}

static PyMethodDef
ExtestMethods[] =
{
	{ "game_init", Extest_game_init, METH_VARARGS,"game_init(map_path) \nparam(string)" },
{ "add_car", Extest_add_car, METH_VARARGS,"add_car(car_name, (x, y)) \nparam(string, (int, int)) \nadd a car on map (x, y)." },
{ "delete_car", Extest_delete_car, METH_VARARGS,"delete_car(car_name) \nparam(string)" },
{ "ctrl", Extest_ctrl, METH_VARARGS,"ctrl(car_name, degree, velocity) \nparam(string, double, double)" },
{ "get_velocity", Extest_get_velocity, METH_VARARGS,"get_velocity(car_name) \nparam(string) \nreturn velocity(double)." },
{ "show_car_sight", Extest_show_car_sight, METH_VARARGS,"show_car_sight(car_name) \nparam(string) \ncreate a window thread to show sight. window will refresh in auto. don't use in a loop." },
{ "get_car_sight", Extest_get_car_sight, METH_VARARGS,"get_car_sight(car_name) \nparam(string) \nreturn car sight(list of list)." },
{ "get_ti", Extest_get_ti, METH_VARARGS,"get_ti(car_name) \nparam(string) \nreturn 3 edge in a dictionary(dict) \nkey use \"mid\"\"right\"\"left\". each edge is a list of list like [[x0,y0],[x1,y1]...], from near to far." },
{ "get_time", Extest_get_time, METH_VARARGS,"get_time() \nreturn time(int) from game start." },
{ "start", Extest_start, METH_VARARGS,"start() \ngame start." },
{ "sleep", Extest_sleep, METH_VARARGS,"sleep(n) \nparam(int) \nwait for about n ms." },
{ NULL, NULL },
};

static struct PyModuleDef cModPyDem =
{
	PyModuleDef_HEAD_INIT,
	"fcgame",		/* name of module */
	"",          /* module documentation, may be NULL */
	-1,          /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	ExtestMethods
};

PyMODINIT_FUNC PyInit_fcgame(void)
{
	return PyModule_Create(&cModPyDem);
}