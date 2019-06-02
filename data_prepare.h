
#pragma once
#include <stdio.h>
#include <iostream>
#include <random>
#include <vector>
using namespace std;
#define windowWidth 1024
#define windowHeight 768
struct float2 {
	float x; float y;

	template <typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & x;
    ar & y;
  }
};
struct belt {
	float2 start;
	float2 end;
	float speed;
	float length;

	template <typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & start;
    ar & end;
    ar & speed;
    ar & length;
  }
};
struct object_data {
	float2 pos;
	float2 direction;
	bool stopped;

	template <typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & pos;
    ar & direction;
    ar & stopped;
  }
};
inline float randf(float min, float max)
{
	return min + (max - min)*(rand() / double(RAND_MAX));
}
inline bool get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
	float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
	float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
	s10_x = p1_x - p0_x;
	s10_y = p1_y - p0_y;
	s32_x = p3_x - p2_x;
	s32_y = p3_y - p2_y;

	denom = s10_x * s32_y - s32_x * s10_y;
	if (denom == 0)//ƽ�л���
		return 0; // Collinear
	bool denomPositive = denom > 0;

	s02_x = p0_x - p2_x;
	s02_y = p0_y - p2_y;
	s_numer = s10_x * s02_y - s10_y * s02_x;
	if ((s_numer < 0) == denomPositive)//�����Ǵ��ڵ���0��С�ڵ���1�ģ����ӷ�ĸ����ͬ���ҷ���С�ڵ��ڷ�ĸ
		return false; // No collision

	t_numer = s32_x * s02_y - s32_y * s02_x;
	if ((t_numer < 0) == denomPositive)
		return false; // No collision

	if (fabs(s_numer) > fabs(denom) || fabs(t_numer) > fabs(denom))
		return false; // No collision
					  // Collision detected
	t = t_numer / denom;
	if (i_x != NULL)
		*i_x = p0_x + (t * s10_x);
	if (i_y != NULL)
		*i_y = p0_y + (t * s10_y);

	return true;
}
inline bool belt_intersect(belt belt1, belt belt2, float2 & intersect) {

	return get_line_intersection(belt1.start.x, belt1.start.y, belt1.end.x, belt1.end.y, belt2.start.x, belt2.start.y, belt2.end.x, belt2.end.y, &intersect.x, &intersect.y);
}
inline void generate_belt_object(unsigned int beltNum, unsigned int objectPerBelt, object_data * objects, belt * belts, std::vector<float2>& intersects) {
	for (int i = 0; i < beltNum; i++) {
		belts[i].speed = 1;
		if (i % 2 == 0) {//horizontal belt
			belts[i].start.x = 0; belts[i].start.y = randf(0, windowHeight);
			belts[i].end.x = windowWidth; belts[i].end.y = randf(0, windowHeight);
		}
		else {// vertical belt
			belts[i].start.y = 0; belts[i].start.x = randf(0, windowWidth);
			belts[i].end.y = windowHeight; belts[i].end.x = randf(0, windowWidth);
		}
		belts[i].length = sqrtf((belts[i].end.x - belts[i].start.x)*(belts[i].end.x - belts[i].start.x) + (belts[i].end.y - belts[i].start.y)*(belts[i].end.y - belts[i].start.y));

		//determine min interval and max interval
		float maxInterval = belts[i].length / objectPerBelt;
		float minInterval = maxInterval*0.2;
		//cal belt move dir
		float2 dir; dir.x = (belts[i].end.x - belts[i].start.x) / belts[i].length; dir.y = (belts[i].end.y - belts[i].start.y) / belts[i].length;
		//set first object
		for (int j = 0; j <objectPerBelt; j++) {
			if (j == 0) {
				objects[objectPerBelt*i].pos.x = belts[i].start.x; objects[objectPerBelt*i].pos.y = belts[i].start.y;
			}
			else {
				float interval = randf(minInterval, maxInterval);
				objects[objectPerBelt*i + j].pos.x = objects[objectPerBelt*i + j - 1].pos.x + dir.x*interval;
				objects[objectPerBelt*i + j].pos.y = objects[objectPerBelt*i + j - 1].pos.y + dir.y*interval;
			}
			objects[objectPerBelt*i + j].direction.x = dir.x;
			objects[objectPerBelt*i + j].direction.y = dir.y;
		}
	}

	float2 inter;
	//get intersect points
	for (int i = 0; i < beltNum; i++) {
		for (int j = 0; j < beltNum; j++) {
			if (belt_intersect(belts[i], belts[j], inter)) {
				intersects.push_back(inter);
			}
		}
	}
}
