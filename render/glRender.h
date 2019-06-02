
#pragma once
#include "glCommon.h"
#include "GLShader.h"
#include "glCamera.h"
#include <string>
#include <vector>
#include "../data_prepare.h"
// Function prototypes


struct renderParam {
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraUp; glm::vec3 lightPos; glm::vec3 lightColor;
	int width = 1024; int height = 768;
};
struct vertex {
	glm::vec3 position;
	glm::vec3 normal;
};
class model_data {
public:
	int id;
	float2 *pos;
	glm::vec3 scale,color;
	std::vector<vertex> vertices;
	unsigned int VAO;
	unsigned int VBO;
	void init_box(int id,float2 *pos,glm::vec3 scale,glm::vec3 color) {
		this->id = id;
		this->pos = pos;
		this->scale = scale;
		this->color = color;
		vertices.clear();
		float v[] = { -0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f };
		for (int i = 0; i < 36; i++) {
			vertex v0; v0.position = glm::vec3(v[i * 3], v[i * 3 + 1], v[i * 3 + 2]); v0.normal = glm::normalize(glm::vec3(v[i * 3], v[i * 3 + 1], v[i * 3 + 2]));
			vertices.push_back(v0);
		}
		this->bindData();
	}

	glm::mat4 getModelMatrix() {
		glm::mat4 modelMatrix = glm::mat4(1);
		glm::vec3 p = glm::vec3(pos->x, pos->y, -1);
		modelMatrix = glm::translate(modelMatrix,p); // translate it down so it's at the center of the scene
		modelMatrix = glm::scale(modelMatrix, scale);	// it's a bit too big for our scene, so scale it down
		return modelMatrix;
	}
	void bindData() {
		glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(vertex),
			&this->vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
			(GLvoid*)0);
		// ���÷���ָ��
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
			(GLvoid*)offsetof(vertex, normal));
		glBindVertexArray(0);
	}
	void draw(glShader & shader) {
		glm::mat4 model = getModelMatrix();
		shader.setVec3("objectColor", color);
		shader.setMat4("model",model);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}
};
class glRender
{
public:
	glCamera camera;
	glm::vec3 lightPos, lightColor;
	int width = 1024, height = 768;
	GLFWwindow* window;
	std::vector<model_data> models;
	glShader shader ;
	glRender(renderParam param) {
		this->camera = glCamera(glm::vec3(param.cameraPos.x, param.cameraPos.y, param.cameraPos.z), glm::vec3(param.cameraUp.x, param.cameraUp.y, param.cameraUp.z));
		this->camera.Front = glm::vec3(param.cameraTarget.x, param.cameraTarget.y, param.cameraTarget.z) - this->camera.Position;
		this->width = param.width; this->height = param.height;
		this->lightColor = param.lightColor; this->lightPos = param.lightPos;
		initGL();
		models.clear();
	}
	glRender()
	{
		models.clear();
		initGL();
	}
	glm::mat4  getViewMatrix()
	{
		return  this->camera.GetViewMatrix();
	}
	glm::mat4 getProjectionMatrix() {
		return  glm::perspective(glm::radians(this->camera.Zoom), ((float)(width)) / ((float)(height)), 0.1f, 1000.0f);
	}

	inline void initGL() {
		/* Initialize the library */
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
		}
		/* Make the window's context current */
		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window,&camera );

		auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mode)
		{
			static_cast<glCamera*>(glfwGetWindowUserPointer(window))->key_callback(window,key,scancode,action,mode);
		};
		auto mouse_callback = [](GLFWwindow* window, double xpos, double ypos)
		{
			static_cast<glCamera*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos,ypos);
		};
		auto scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset)
		{
			static_cast<glCamera*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
		};
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		// Options
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glewExperimental = GL_TRUE;
		glewInit();
		glEnable(GL_DEPTH_TEST);
		shader = glShader("render\\phong.vs", "render\\phong.fs");
		shader.use();
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
		shader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
		//shader.setBool("blinn", false);
	}
	~glRender()
	{
		glfwTerminate();
	}
	void render() {
		camera.updateDt(glfwGetTime());
		glfwPollEvents();
		camera.Do_Movement();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.setMat4("projection", getProjectionMatrix());
		shader.setMat4("view", getViewMatrix());
		for (int i = 0; i < models.size(); i++) {
			models[i].draw(shader);
		}
		//FBO_2_PPM_file(frame);
		glfwSwapBuffers(window);
	}
	inline void FBO_2_PPM_file(int num)
	{
		FILE    *output_image;
		int     output_width, output_height;

		output_width = width;
		output_height = height;
		/// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
		unsigned char   *pixels = (unsigned char*)malloc(output_width*output_height * 3);
		/// READ THE CONTENT FROM THE FBO
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		//glBindTexture(GL_TEXTURE_2D, _texture);
		//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA,GL_UNSIGNED_BYTE, pixels);
		glReadPixels(0, 0, output_width, output_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		char filename[256];
		// sprintf_s(filename, "./data/ppm_ve/static%05d.ppm", num);

		output_image = fopen(filename, "wt");

		std::ofstream ofs(filename, std::ios::out | std::ios::binary);
		ofs << "P6\n" << output_width << " " << output_height << "\n255\n";
		for (unsigned i = 0; i < output_width * output_height * 3; ++i) {
			ofs << (unsigned char)(pixels[i]);
		}
		ofs.close();
		free(pixels);
	}
};
