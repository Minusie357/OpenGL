#include "Application.h"
#include <iostream>
#include <string>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Minusi;

int main()
{
	std::unique_ptr<Application> application{ new Application("Test Window")};
	application->Initialize();
	application->Run();
	return 0;
}







Application::Application(const std::string& title)
	: _Title(title)
{

}



void Application::Initialize()
{
	if (glfwInit() == false)
	{
		glfwTerminate();
		return;
	}

	// setup glfw window properties, openGL versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// core profile = no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);



	_MainWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, _Title.c_str(), nullptr, nullptr));
	if(_MainWindow == nullptr)
	{
		glfwTerminate();
		return;
	}

	// get buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(_MainWindow.get(), &bufferWidth, &bufferHeight);
	// set context for glew to use
	glfwMakeContextCurrent(_MainWindow.get());
	
	
	
	// allow modern extension features
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(_MainWindow.get());
		glfwTerminate();
		return;
	}

	// setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);
 }



void Minusi::Application::Run()
{
	_RenderingComponent->CreateTriangle();

	bool isRight = true;
	float triangleOffset = 0.0f;
	float rotateOffset = 0.0f;
	float triangleMaxOffset = 0.7f;
	float triangleIncrement = 0.05f;
	float rotateIncrement = 0.01f;
	// loop until window closed
	while (glfwWindowShouldClose(_MainWindow.get()) == false)
	{
		// get and handle user input events
		glfwPollEvents();

		if (isRight)
		{
			triangleOffset += triangleIncrement;
		}
		else
		{
			triangleOffset -= triangleIncrement;
		}
		rotateOffset += rotateIncrement;

		if (abs(triangleOffset) >= triangleMaxOffset)
		{
			isRight = !isRight;
		}

		// clear window
		glClearColor(0.0f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_RenderingComponent->GetShader());

		glm::mat4 model(1.0f);
		model = glm::rotate(model, 45 * rotateOffset * (float)(M_PI / 180), glm::vec3(0.f, 0.f, 1.f));
		model = glm::translate(model, glm::vec3(triangleOffset, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.f));
		glUniformMatrix4fv(_RenderingComponent->GetUniformModel(), 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(_RenderingComponent->GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);
		glfwSwapBuffers(_MainWindow.get());
	}
}



void Minusi::RenderingComponent::CreateTriangle()
{
	std::string vertexShader =
		"#version 330\n"
		"\n"
		"layout (location = 0) in vec3 pos;\n"
		"\n"
		"uniform mat4 model;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = model * vec4(pos.x * 0.4f, pos.y * 0.4f, pos.z, 1.0);\n"
		"}";

	std::string fragmentShader = 
		"#version 330\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}"; 

	GLfloat vertices[] = {
		-1.f, -1.f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

	glGenBuffers(1, &_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_CompileShaders(vertexShader, fragmentShader);
}



void Minusi::RenderingComponent::_AddShader(GLuint program, const std::string& code, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);

	const GLchar* shaderCode[1];
	shaderCode[0] = code.c_str();
	
	GLint codeLength[1];
	codeLength[0] = code.length();

	glShaderSource(shader, 1, shaderCode, codeLength);
	glCompileShader(shader);



	GLint result = 0;
	GLchar log[1024] = { 0, };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shader, 1024, NULL, log);

		glGetProgramInfoLog(shader, 1024, NULL, log);
		return;
	}

	glAttachShader(program, shader);
}



void Minusi::RenderingComponent::_CompileShaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode)
{
	_Program = glCreateProgram();
	if (_Program == 0)
	{
		return;
	}

	_AddShader(_Program, vertexShaderCode, GL_VERTEX_SHADER);
	_AddShader(_Program, fragmentShaderCode, GL_FRAGMENT_SHADER);



	GLint result = 0;
	GLchar log[1024] = { 0, };

	glLinkProgram(_Program);
	glGetProgramiv(_Program, GL_LINK_STATUS, &result);
	if (result == 0)
	{
		glGetProgramInfoLog(_Program, 1024, NULL, log);
		return;
	}



	glValidateProgram(_Program);
	glGetProgramiv(_Program, GL_LINK_STATUS, &result);
	if (result == 0)
	{
		glGetProgramInfoLog(_Program, 1024, NULL, log);
		return;
	}

	_UniformModel = glGetUniformLocation(_Program, "model");
}
