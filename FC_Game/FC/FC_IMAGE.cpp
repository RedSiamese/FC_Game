#include "FC_IMAGE.h"
#include <stdlib.h>

#define SEARCH_LIMIT	8
#define	JUMP_LIMIT		5
#define	LEAST_SIZE		4
#define STEP_			4  

#pragma message( INCLUDE_FILE_AND_LINE( "SEARCH_LIMIT = " CATCH(SEARCH_LIMIT)))
#pragma message( INCLUDE_FILE_AND_LINE( "JUMP_LIMIT = " CATCH(JUMP_LIMIT)))
#pragma message( INCLUDE_FILE_AND_LINE( "LEAST_SIZE = " CATCH(LEAST_SIZE)))
#pragma message( INCLUDE_FILE_AND_LINE( "STEP_" CATCH(STEP_)))

/************************************	FREESCALE_BASIC	*************************************/

fc_map create_map(uint8 width, uint8 height, const char* str) {

	fc_map map{ width , height, 0, str != NULL, NULL, NULL };
	map.buf = (str) ? (pbyte)str : (pbyte)calloc(width*height, sizeof(byte));
	map.map = (byte_map)calloc(height, sizeof(pbyte));
	for (uint8 i = 0; i < height; i++) {
		map.map[i] = map.buf + i * width;
	}
	return map;
}

void free_map(fc_map* map) {
	if (!map->external) { free(map->buf); }
	free(map->map);
	CLEAR_BUF(map, sizeof(fc_map));
}

fc_track_info create_track_info(fc_map map) {
	fc_track_info ti;
	CLEAR_BUF(&ti, sizeof(fc_track_info));
	ti.mid_curve.size = 20;
	for (int8 i = 0; i < 20; i++) {
		P_TO_L(ti.mid_curve.point[i]) = CONST_VECTOR(map.width / 2, map.height - map.height*i / 21 - 2);
	}
	return ti;
}

/***************************************	FREESCALE_DISPOSE	*************************************/

void gray_to_bool(fc_map* map) {
	int32 i = 0, j = 0, cen[2] = { 255, 0 }, s_cen[2], c_cen[2], m = 0, count = 0;
	uint8 num[256], **pmap = map->map, x = map->width, y = map->height;

	for (i = 0; i < y; i += 10) {
		for (j = 20; j < x; j += 20) {
			num[count++] = pmap[i][j];
			cen[1] = MAX(pmap[i][j], cen[1]);
			cen[0] = MIN(pmap[i][j], cen[0]);
			if (count >= 256) { goto k_means; }
		}
	}
k_means:
	for (i = 0; i < 3 && cen[0] != cen[1]; i++) {
		m = (cen[0] + cen[1]) >> 1;
		for (s_cen[0] = s_cen[1] = c_cen[0] = c_cen[1] = j = 0; j < count; j++) {
			s_cen[num[j] > m] += num[j];
			c_cen[num[j] > m]++;
		}
		cen[0] = s_cen[0] / c_cen[0];
		cen[1] = s_cen[1] / c_cen[1];
	}
	map->gray = (uint8)((cen[0] * 46 + cen[1] * 82) >> 7);
}


void fc_get_track_edge(fc_track_info* track_info, fc_map map) {

	int8 site[2] = { LEFT, RIGHT };
	fc_point *ppoint = track_info->mid_curve.point;
	track_info->left_curve = track_info->right_curve = { 0 };

	for (uint8 i = 0, msize = track_info->mid_curve.size, fp = track_info->mid_curve.fptr; i < 2; i++) {
		fc_curve edge = { 0 };
		uint8 count = FALL_IN_(fp - 2, 0, msize - 1), ty, tx, limit_x = site[i] * map.width / 2 + (map.width - map.width / 2);

		//首点下移
		for (int8 tap = 0; tap < 2 && ppoint[count].y<map.height - 3 && GET_PIX_XY(map, ppoint[count].x, ppoint[count].y + 2) > map.gray; tap++, ppoint[count].y += 2);
		for (/*(fp>5) ? count = 0 :*/; count<msize && GET_PIX_P(map, ppoint[count]) > map.gray; count++) {
			for (tx = ppoint[count].x, ty = ppoint[count].y; GET_PIX_XY(map, tx, ty) > map.gray&&tx != limit_x; tx += site[i]) {
#ifdef WIN_DEBUG
				paint(fc_point{ tx, ty }, RGB(255, 200, 200));
#endif
			}
			if (tx != limit_x && GET_PIX_XY(map, tx + 3 * site[i], ty + 1) <= map.gray) {
				P_TO_L(CURVE_PUSH_BACK(edge)) = CONST_VECTOR(tx, ty);
#ifdef WIN_DEBUG
				paint(CURVE_BACK(edge), RGB(255, 100, 100));
#endif
				ty -= 3;
				for (tx = ppoint[count].x; GET_PIX_XY(map, tx, ty) > map.gray&&tx != limit_x; tx += site[i]) {
#ifdef WIN_DEBUG
					paint(fc_point{ tx, ty }, RGB(255, 200, 200));
#endif
				}
				if (tx != limit_x && (GET_PIX_XY(map, tx - 75 * site[i], ty - 3) <= map.gray || GET_PIX_XY(map, tx + 5 * site[i], ty - 3) <= map.gray) && ABS(tx - edge.point[0].x)<10) {
					P_TO_L(CURVE_PUSH_BACK(edge)) = CONST_VECTOR(tx, ty);
#ifdef WIN_DEBUG
					paint(CURVE_BACK(edge), RGB(255, 100, 100));
#endif
					fc_search_edge(&edge, map, site[i]);
					if (edge.size > LEAST_SIZE) {
						if (edge.point[0].y < map.height * 9 / 10) {
							edge.fptr = fc_fix_back(map, &edge);
						}
						(site[i] == LEFT) ? track_info->left_curve = edge : track_info->right_curve = edge;
						break;
					}
				}
			}
			CLEAR_BUF(&edge, sizeof(fc_curve));
		}
	}
}


void fc_search_edge(fc_curve* curve, fc_map map, int8 site) {

	fc_curve edge = *curve;
	int8 fix = 0, step, pb, round = 0;
	fc_point point, temp_point, vec, ovec;

	for (;;) {
		P_TO_L(vec) = VECTOR_MINUS(CURVE_BACK(edge), CURVE_BACK_N(edge, FALL_IN_(4, 0, edge.size)));
		P_TO_L(point) = POINT_MOVE(CURVE_BACK(edge), vec, JUMP_LIMIT);
		if (!IF_FALL_IN_MAP(map, point)) { break; }

		//printf("%d\n", +(point.y << 1) / map.height);
#ifdef WIN_DEBUG
		paint(point, RGB(0, 255, 255));
#endif
		pb = GRAY_TO_BOOL(map, point);
		P_TO_L(ovec) = ORTHOGONAL(vec, pb*site);
		for (step = 1, temp_point = point; step < SEARCH_LIMIT && IF_FALL_IN_MAP(map, temp_point) && pb == GRAY_TO_BOOL(map, temp_point); step++) {		//慢
			P_TO_L(temp_point) = POINT_MOVE(point, ovec, step);
#ifdef WIN_DEBUG
			paint(temp_point, RGB(0, 255, 255));
#endif
		}
		if (!IF_FALL_IN_MAP(map, temp_point)) { break; }
		else if (step == SEARCH_LIMIT) {
			if (pb == TRUE && (edge.size > 4 || CURVE_BACK(edge).y<map.height / 2)) {//全白////////////////////////??????????
#ifdef WIN_DEBUG
				paint(fc_point(point.x + vec.x * 8 - site * 15, point.y + vec.y * 8), RGB(255, 0, 0));
				//paint(fc_point(point.x - site * 25, point.y - 40), RGB(255, 0, 0));
#endif
				if (fix != 1
					&& GET_PIX_XY(map, FALL_IN_(point.x + vec.x * 4 - site * 15, 0, map.width - 1),
						FALL_IN_(point.y + vec.y * 4, 0, map.height - 1))>map.gray
					&& fc_fix(map, &edge, site)) {
					fix = 1;
				}//增加了补线成功概率
				else {
					if (round++ < 1) {
						P_TO_L(point) = POINT_MOVE(CURVE_BACK(edge), ovec, SEARCH_LIMIT >> 1);
						CURVE_PUSH_BACK(edge) = point;
#ifdef WIN_DEBUG
						paint(CURVE_BACK(edge), RGB(255, 100, 100));
#endif
					}
					else { break; }
				}//添加补完
			}
			//直角！
			else { break; }//全黑或小于6
		}
		else {
			P_TO_L(point) = POINT_MOVE(point, ovec, step - 2);
			CURVE_PUSH_BACK(edge) = point;
#ifdef WIN_DEBUG
			paint(CURVE_BACK(edge), RGB(255, 100, 100));
#endif
		}
	}
	*curve = edge;
}


bool fc_fix(fc_map map, fc_curve* curve, int8 site) {

	int8 step, count;
	fc_point point_start = CURVE_BACK(*curve), temp_point, point, vec, ovec;

	P_TO_L(vec) = VECTOR_MINUS(CURVE_BACK(*curve), curve->point[FALL_IN_(curve->size - 5, 0, 255)]);
	P_TO_L(ovec) = ORTHOGONAL(vec, site);
	P_TO_L(vec) = VECTOR_ZOOM(vec, 2);
	vec.x -= site;
	for (uint8 times = 2;; times++) {
		P_TO_L(point) = POINT_MOVE(point_start, vec, JUMP_LIMIT*times);
		if (!IF_FALL_IN_MAP(map, point)) { return false; }
#ifdef WIN_DEBUG
		paint(point, RGB(255, 200, 0));
#endif
		for (step = 1, temp_point = point; step < SEARCH_LIMIT && 1 == GRAY_TO_BOOL(map, temp_point); step++) {		//慢
			P_TO_L(temp_point) = POINT_MOVE(point, ovec, step);
			if (!IF_FALL_IN_MAP(map, temp_point)) { return false; }//不知道有没有用
#ifdef WIN_DEBUG
			paint(temp_point, RGB(0, 255, 255));
#endif
		}
		if (!IF_FALL_IN_MAP(map, temp_point)) { return false; }
		else if (step != SEARCH_LIMIT) {
			//增加判断
			POINT_MOVE_UP(temp_point);
			for (count = 0; count < 10 && -1 == GRAY_TO_BOOL(map, temp_point); temp_point.x -= site * 2, count++) {
#ifdef WIN_DEBUG
				paint(temp_point, RGB(0, 255, 255));
#endif
			}
			if (count < 10 && GET_PIX_XY(map, temp_point.x - site, temp_point.y++) >= map.gray) {
				for (uint8 i = times - 2; i > 1; i--) {//补点
					P_TO_L(CURVE_PUSH_BACK(*curve)) = VECTOR_MEAN(point_start, temp_point, i * 100 / times);
				}
				CURVE_PUSH_BACK(*curve) = temp_point;
#ifdef WIN_DEBUG
				paint(temp_point, RGB(255, 100, 100));
#endif
				return true;
			}
			else { return false; }
		}
	}
}


uint8 fc_fix_back(fc_map map, fc_curve* curve) {

	fc_curve fix_edge = { 0 };
	fc_point point = curve->point[0], vec, *pp = &CURVE_BACK(*curve), *spp = curve->point;

	P_TO_L(vec) = VECTOR_MINUS(curve->point[0], curve->point[3]);
	for (POINT_MOVE(point, vec, 3); IF_FALL_IN_MAP(map, point); P_TO_L(point) = POINT_MOVE(point, vec, 3)) {
		CURVE_PUSH_BACK(fix_edge) = point;
#ifdef WIN_DEBUG
		paint(point, RGB(255, 200, 0));
#endif
	}
	for (; pp >= spp; pp--) {
		*(pp + fix_edge.size) = *pp;
	}
	pp += fix_edge.size;
	for (uint8 i = 0; pp >= spp; pp--, i++) {
		*pp = fix_edge.point[i];
	}
	curve->size += fix_edge.size;
	return fix_edge.size;
}


void fc_get_track_mid(fc_track_info* track_info, fc_map map) {

	fc_track_info last_track_info = *track_info;
	fc_curve R, L, M = { 0 };

	if (track_info->left_curve.point[track_info->left_curve.size / 4].x - track_info->right_curve.point[track_info->right_curve.size / 4].x > 10) {
		track_info->left_curve.size = 0;
		track_info->right_curve.size = 0;
	}

	for (int8 s = 0, j = 0; s < 2; s++, j = 0) {
		fc_curve cur = (s == 0) ? track_info->left_curve : track_info->right_curve;
		fc_vector vec_mean = { 0 };

		if (!cur.size) {
			P_TO_L(CURVE_PUSH_BACK(cur)) = CONST_VECTOR((map.width - 1)*s, map.height - 2);
			P_TO_L(CURVE_PUSH_BACK(cur)) = CONST_VECTOR((map.width - 1)*s, map.height - 3);
		}

		for (uint8 i = STEP_, startptr = 0, vec_count = 0, percent = 0; i < cur.size - STEP_ - 1; i += 2, vec_count++, percent = 100 * vec_count / (1 + vec_count)) {
			fc_vector vec1, vec2;
			P_TO_L(vec1) = VECTOR_MINUS(cur.point[i], cur.point[startptr]);
			P_TO_L(vec_mean) = VECTOR_MEAN(vec_mean, vec1, percent);
			P_TO_L(vec2) = VECTOR_MINUS(cur.point[i + STEP_], cur.point[i]);

			int32 dp = VECTOR_DP(vec_mean, vec2), cos = dp * dp * 10000 / VECTOR_MODULE_SQ(vec_mean) / VECTOR_MODULE_SQ(vec2);
			if (cos < 6400 && j<7) {
				if (cos < -5000) { break; }
				cur.pptr[j++] = startptr = (i += 5) - 5;
#ifdef WIN_DEBUG
				paint(cur.point[cur.pptr[j - 1]], RGB(255, 192, 0));
#endif
				vec_count = -1;
			}
		}
		cur.pptr[j] = (cur.pptr[j + 1] = cur.size) - 1;
		((s == 0) ? L : R) = cur;
	}

	for (int8 count = 0, startptl_r = 0, startptl_l = 0;; startptl_r = R.pptr[count], startptl_l = L.pptr[count++]) {
		for (int8 count_l = L.pptr[count] - startptl_l, count_r = R.pptr[count] - startptl_r, j = 0, count = (count_l + count_r) / 2; j < count; j++) {
			P_TO_L(CURVE_PUSH_BACK(M)) = VECTOR_MEAN(R.point[count_r*j / count + startptl_r], L.point[count_l*j / count + startptl_l], 50);
			if (P_TO_L(CURVE_BACK(M)) == P_TO_L(CURVE_BACK_N(M, 2))) { M.size--; }
		}
		if (L.pptr[count + 1] == 0 || R.pptr[count + 1] == 0) { break; }
	}

	for (int8 i = 1; i < M.size - 1; i++) {
		P_TO_L(M.point[i]) = VECTOR_MEAN(M.point[i + 1], M.point[i - 1], 50);
	}
	M.fptr = MAX(R.fptr, L.fptr);
	track_info->mid_curve = M;
}


void get_track_mark(fc_track_info * track_info, fc_map map) {

	fc_point point, temp_point, vec;
	uint8 mark = 0;
	P_TO_L(vec) = VECTOR_MINUS(track_info->mid_curve.point[2], track_info->mid_curve.point[0]);

	for (int8 i = 2; i; i >>= 1, mark >>= 1) {
		P_TO_L(point) = (i == 2) ? VECTOR_MEAN(track_info->left_curve.point[0], track_info->mid_curve.point[0], 50)
			: VECTOR_MEAN(track_info->right_curve.point[0], track_info->mid_curve.point[0], 50);
		for (uint8 j = 5; j; j--, P_TO_L(temp_point) = POINT_MOVE(point, vec, j)) {
#ifdef WIN_DEBUG
			paint(temp_point, RGB(255, 100, 100));
#endif
			mark |= GRAY_TO_BOOL(map, temp_point) & 0x80;
		}
	}
	(mark == 0x60) ? track_info->track_mark = fc_mark::destination : track_info->track_mark = fc_mark::null;
}

