#include "external/glad/glad.h"
#include "external/GLFW/glfw3.h"

#include "vector/vector.h"
#include "btree/btree.h"
#include "str/str.h"

#include "log.h"

#include <stdlib.h>
#include <stdio.h>

void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height)
{
	glViewport(0, 0, width, height);
}

constexpr u32 windowWidth = 800;
constexpr u32 windowHeight = 600;

int main(int argc, char* argv[])
{
	f32 vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};
	const char *vertexShaderSource =
		"#version 100                               \n"
		"attribute vec2 vPosition;                  \n"
		"attribute vec3 vColor;                     \n"
		"varying   vec3 fColor;                     \n"
		"void main()                                \n"
		"{                                          \n"
		"   gl_Position = vec4(vPosition, 0, 1.0);  \n"
		"   fColor = vColor;                        \n"
		"}                                          \n";
	const char *fragmentShaderSource =
		"#version 100                        \n"
		"precision mediump float;            \n"
		"varying   vec3 fColor;              \n"
		"void main()                         \n"
		"{                                   \n"
		"   gl_FragColor = vec4(fColor, 1.0);\n"
		"}                                   \n";

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Holang-Test", nullptr, nullptr);
	if(!window){
		const char *errorDescription;
		i32 errorCode = glfwGetError(&errorDescription);
		fprintf(stderr, "GLFW failed with error code %d: %s\n", errorCode, errorDescription);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if(!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)){
		fprintf(stderr, "Failed to load GLAD\n");
		return EXIT_FAILURE;
	}

	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	DEBUG_DLOPEN("str/", str);
	DEBUG_DLSYM(str, string_init);
	String *infoLog = string_init();
	i32 compileStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
	DEBUG_DLSYM(str, string_reserve);
	if(compileStatus == GL_FALSE){
		i32 reserveSize;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &reserveSize);
		string_reserve(infoLog, reserveSize);
		glGetShaderInfoLog(vertexShader, string_capacity(infoLog), nullptr, string_get_mut_data(infoLog));
		fprintf(stderr, "Vertex shader compilation failed: %s\n", string_get_data(infoLog));
		return EXIT_FAILURE;
	}

	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus == GL_FALSE){
		i32 reserveSize;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &reserveSize);
		string_reserve(infoLog, reserveSize);
		glGetShaderInfoLog(fragmentShader, string_capacity(infoLog), nullptr, string_get_mut_data(infoLog));
		fprintf(stderr, "Fragment shader compilation failed: %s\n", string_get_data(infoLog));
		return EXIT_FAILURE;
	}

	u32 shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	i32 vPosition = 0, vColor = 1;
	glBindAttribLocation(shaderProgram, vPosition, "vPosition");
	glBindAttribLocation(shaderProgram, vColor, "vColor");

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &compileStatus);
	if(compileStatus == GL_FALSE){
		i32 reserveSize;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &reserveSize);
		string_reserve(infoLog, reserveSize);
		glGetProgramInfoLog(shaderProgram, string_capacity(infoLog), nullptr, string_get_mut_data(infoLog));
		fprintf(stderr, "Shader progam linking failed: %s\n", string_get_data(infoLog));
		return EXIT_FAILURE;
	}
	DEBUG_DLSYM(str, string_uninit);
	string_uninit(infoLog);
	DEBUG_DLCLOSE(str);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	u32 VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(*vertices), (void*)0);
	glEnableVertexAttribArray(vPosition);

	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(*vertices), (void*)(3 * sizeof *vertices));
	glEnableVertexAttribArray(vColor);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram);
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();

		if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glClearColor(0.2f, 0.5f, 0.8f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();

	//LOG_ERROR("test");
	//LOG_INFO("test");
	//LOG_TRACE("test");
	//LOG_WARN("test");

	//LOGF_ERROR("Nice! %u", rand());
	//LOGF_INFO("Nice! %u", rand());
	//LOGF_TRACE("Nice! %u", rand());
	//LOGF_WARN("Nice! %u", rand());

	DEBUG_DLOPEN("vector/", vector);
	DEBUG_DLSYM(vector, vector_init);
	DEBUG_DLSYM(vector, vector_push_back);
	DEBUG_DLSYM(vector, vector_at);
	DEBUG_DLSYM(vector, vector_uninit);

	constexpr usize testSize = 767;
	u32 sum = 0;
	u32 intTest;
	
	Vector *vecTest = vector_init(sizeof intTest);
	for(usize j = 0; j < testSize; ++j){
		intTest = rand();
		vector_push_back(vecTest, &intTest);
		u32 *getBack = vector_at(vecTest, j);
		sum += *getBack;
	}
	
	vector_uninit(vecTest);

	DEBUG_DLCLOSE(vector);
	LOGF_INFO("Vector sum: %u", sum);

	DEBUG_DLOPEN("btree/", btree);
	DEBUG_DLSYM(btree, btree_init);
	DEBUG_DLSYM(btree, btree_insert);
	DEBUG_DLSYM(btree, btree_uninit);
	DEBUG_DLSYM(btree, btree_erase);
	DEBUG_DLSYM(btree, btree_search);
	
	DEBUG_DLCLOSE(btree);
}


