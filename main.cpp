#pragma once
#include "data_prepare.h"
#include "render\glRender.h"
#include<windows.h>

int main() {
	DWORD t1, t2;
	int beltNum =10; int allNum = 10000;
	for (int allNum = 50000;  allNum <= 550000;  allNum+=50000) {
		int objectPerBelt = allNum / beltNum;
		cout << "条带数：" << beltNum << "物体数目" << objectPerBelt*beltNum << endl;
		object_data * objects;
		belt * belts;
		t1 = GetTickCount();
		objects = new object_data[beltNum*objectPerBelt];
		belts = new belt[beltNum];
		std::vector<intersect> intersects;
		generate_belt_object(beltNum, objectPerBelt, objects, belts, intersects);
		t2 = GetTickCount();
		cout << "生成数据时间：" << (t2 - t1) << endl;


		//t1 = GetTickCount();
		//renderParam param;
		//param.cameraPos = glm::vec3(500 / 2, 500 / 2, -500);
		//param.cameraUp = glm::vec3(0, 1, 0);
		//param.cameraTarget = glm::vec3(500 / 2, 500 / 2, -1);
		//param.lightColor = glm::vec3(1, 1, 1);
		//param.lightPos = param.cameraPos;
		//glRender render(param);
		//for (int i = 0; i < beltNum*objectPerBelt; i++) {
		//	model_data data; data.init_box(i, &objects[i].pos, glm::vec3(5, 5, 5), glm::vec3(0, 1, 0));
		//	render.models.push_back(data);
		//}
		//for (int i = 0; i < intersects.size(); i++) {
		//	model_data data; data.init_box(i + beltNum*objectPerBelt, &intersects[i].pos, glm::vec3(5, 5, 5), glm::vec3(1, 0, 0));
		//	render.models.push_back(data);
		//}
		//t2 = GetTickCount();
		//cout << "生成渲染数据时间：" << (t2 - t1) << endl;

		unsigned int collisionTime = 0, updateTime = 0, renderTime = 0;
		//while (!glfwWindowShouldClose(render.window))
		//{
		int maxNum = 100;

		for (int i = 0; i < maxNum; i++) {
			t1 = GetTickCount();
			check_obj_intersect(objects, beltNum*objectPerBelt, intersects);
			t2 = GetTickCount(); collisionTime += (t2 - t1);
			t1 = GetTickCount();
			update_object_state(objects, objectPerBelt, beltNum);
			t2 = GetTickCount(); updateTime += (t2 - t1);
			//t1 = GetTickCount();
			//render.render();
			//t2 = GetTickCount(); renderTime += (t2 - t1);
		}
			
		//}

		cout << "平均碰撞处理时间：" << ((float)collisionTime / maxNum) << endl;
		cout << "平均更新数据时间：" << ((float)updateTime) / maxNum << endl;
		cout << "平均渲染数据时间：" << ((float)renderTime / maxNum) << endl;
		cout << "--------------------------------" << endl;
	}
	return 0;
}