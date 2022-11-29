#pragma once
#include <string>
#include <memory>
#include <glew.h>
#include <glfw3.h>



namespace Minusi
{
	class RenderingComponent;


	struct GLFWwindowDeleter
	{
	public:
		void operator() (GLFWwindow* window)
		{
			if (window == nullptr)
			{
				return;
			}

			glfwDestroyWindow(window);
		}
	};



	class RenderingComponent
	{
	public:
		void CreateTriangle();

	public:
		GLuint& GetVAO() { return _VAO; }
		GLuint& GetVBO() { return _VBO; }
		GLuint& GetShader() { return _Program; }
		GLuint& GetUniformModel() { return _UniformModel; }

	private:
		void _AddShader(GLuint program, const std::string& code, GLenum shaderType);
		void _CompileShaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);

	private:
		GLuint														_VAO{};
		GLuint														_VBO{};
		GLuint														_Program{};

		GLuint														_UniformModel{};
	};



	class Application
	{
	private:
		static constexpr GLint WINDOW_WIDTH = 800;
		static constexpr GLint WINDOW_HEIGHT = 600;

	public:
		Application(const std::string& title);

		void Initialize();
		void Run();


	private:
		std::unique_ptr<GLFWwindow, GLFWwindowDeleter>				_MainWindow{};
		std::unique_ptr<RenderingComponent>							_RenderingComponent{ new RenderingComponent() };

		std::string													_Title{};
	};
}