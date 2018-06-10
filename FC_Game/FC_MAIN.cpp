
#include "Simulator\FC_game.h"

#include "FC\FC_SDE.h"
#include "FC\FC_IMAGE.h"

#include <io.h>
#include <shlobj.h>

using namespace std;

FC_ENV *game_env;


int tt = 0;
//
//fc_track_info get_ti(const char* name) {
//
//	FC_camera cam(game_env->get_car(name).get_cam());
//
//	for (; !game_env->get_car(name).is_refresh_finish(););
//	IplImage* sight = cvCloneImage(game_env->get_car(name).get_sight());
//
//	IplImage* sight_t = cvCreateImage(CvSize(sight->width / 2, sight->height * 2), 8, 1);
//	cam.dpt_img(sight, sight_t);
//
//	fc_map Image = create_map(sight_t->width, sight_t->height, NULL);
//	memcpy(Image.buf, sight_t->imageData, sight_t->width*sight_t->height * sizeof(byte));
//
//	/*char i2a[16];
//	_itoa(tt, i2a, 10);
//	string str("F:\\C++ Project\\FC_Simulator\\FC_Simulator\\img\\dpt\\");
//	str += string(i2a) + ".bmp";
//	cvSaveImage(str.c_str(), sight_t);*/
//
//	gray_to_bool(&Image);
//
//	fc_track_info TI = create_track_info(Image);
//
//	fc_get_track_edge(&TI, Image);
//	fc_get_track_mid(&TI, Image);
//
//	free_map(&Image);
//
//	CvPoint p;
//	for (int i = 0; i < TI.mid_curve.size; i++) {
//		p = cam.pt_point(sight, sight_t,cvPoint(TI.mid_curve.point[i].x, TI.mid_curve.point[i].y));
//		TI.mid_curve.point[i] = fc_point(FALL_IN_(p.x,0,sight->width-1), FALL_IN_(p.y,0, sight->height));
//	}
//	for (int i = 0; i < TI.right_curve.size; i++) {
//		p = cam.pt_point(sight, sight_t, cvPoint(TI.right_curve.point[i].x, TI.right_curve.point[i].y));
//		TI.right_curve.point[i] = fc_point(FALL_IN_(p.x, 0, sight->width - 1), FALL_IN_(p.y, 0, sight->height));
//	}
//	for (int i = 0; i < TI.left_curve.size; i++) {
//		p = cam.pt_point(sight, sight_t, cvPoint(TI.left_curve.point[i].x, TI.left_curve.point[i].y));
//		TI.left_curve.point[i] = fc_point(FALL_IN_(p.x, 0, sight->width - 1), FALL_IN_(p.y, 0, sight->height));
//	}
//
//	cvReleaseImage(&sight);
//	cvReleaseImage(&sight_t);
//	return TI;
//}


int main() {

	LARGE_INTEGER litmp;
	QueryPerformanceCounter(&litmp);
	fc_xorshift_init(litmp.QuadPart % 100000);/**/

	game_env = new FC_ENV("FCMAP1.png");
	new FC_THREAD_SHOW(game_env->get_show(), "env");

	Sleep(500);

	FC_POINT p(380.0, 40.0);
	game_env->add_car(new FC_CAR(string("0"), *game_env, p, 0.0, 20.0, -17.32050808 / 360 * 2 * CV_PI, 0.45, 27.0));

	game_env->set_time_speed(0);
	
	auto w = game_env->get_car("0").get_sight()->width;
	
	for (game_env->start();;) {
		game_env->get_car("0").set_velocity(100);
		
		auto TI = FC_CAR::get_track_info(game_env->get_car("0"));
		auto d = 0.04*(TI.mid_curve.point[0].x - w / 2) + 0.04*(TI.mid_curve.point[TI.mid_curve.size / 4].x - w / 2) + 0.04*(TI.mid_curve.point[TI.mid_curve.size / 2].x - w / 2);
		game_env->get_car("0").control(d, 100);
		Sleep(1);

		//printf("%lf", game_env->get_time());
	}
	
}
			/*char i2a[16];
			_itoa(i, i2a, 10);
			string str("F:\\C++ Project\\FC_Simulator\\FC_Simulator\\img\\dpt\\");
			str += string(i2a) + ".bmp";
			cvSaveImage(str.c_str(), dpt);*//**/
			//cvShowImage("asd", dpt);
			//cv::waitKey(1);/**/


			/*for (int i = 0; i < TI.mid_curve.size; i++) {
			cvSet2D(sight_t, TI.mid_curve.point[i].y, TI.mid_curve.point[i].x, 0);
			}
			for (int i = 0; i < TI.right_curve.size; i++) {
			cvSet2D(sight_t, TI.right_curve.point[i].y, TI.right_curve.point[i].x, 0);
			}
			for (int i = 0; i < TI.left_curve.size; i++) {
			cvSet2D(sight_t, TI.left_curve.point[i].y, TI.left_curve.point[i].x, 0);
			}*/