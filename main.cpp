#pragma once
#include "data_prepare.h"
#include "render\glRender.h"



int main() {
	int beltNum = 3;
	int objectPerBelt = 5;
	object_data * objects;
	belt * belts;
	objects = new object_data[beltNum*objectPerBelt];
	belts = new belt[beltNum];
	std::vector<float2> intersect;
	generate_belt_object(beltNum, objectPerBelt, objects, belts, intersect);

	


	renderParam param;
	param.cameraPos = glm::vec3(1024 / 2, 768 / 2, -800);
	param.cameraUp = glm::vec3(0, 1, 0);
	param.cameraTarget = glm::vec3(1024 / 2, 768 / 2, -1);
	param.lightColor = glm::vec3(1, 1, 1);
	param.lightPos = param.cameraPos;
	glRender render(param);
	for (int i = 0; i < beltNum*objectPerBelt; i++) {
		model_data data; data.init_box(i, &objects[i].pos, glm::vec3(5, 5, 5), glm::vec3(0, 1, 0));
		render.models.push_back(data);
	}
	for (int i = 0; i < intersect.size(); i++) {
		model_data data; data.init_box(i + beltNum*objectPerBelt, &intersect[i], glm::vec3(5, 5, 5), glm::vec3(1, 0, 0));
		render.models.push_back(data);
	}
	while (!glfwWindowShouldClose(render.window))
	{
		render.render();
	}
	return 0;
}