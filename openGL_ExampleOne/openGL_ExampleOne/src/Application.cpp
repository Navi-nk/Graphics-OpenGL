#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include<sstream>
#include<fstream>
#include<string>

typedef unsigned int uint;

struct ShaderSources
{
	std::string vertexShader;
	std::string fragmentShader;
};

enum class ShaderType
{
	NONE = -1, VERTEX = 0, FRAGMENT = 1
};


static ShaderSources* ParseShader(const std::string& filepath) 
{
	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss[2];

	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
			ss[(int)type] << line << "\n";
	}

	ShaderSources* sSource = new ShaderSources();
	sSource->vertexShader = ss[0].str();
	sSource->fragmentShader =ss[1].str();

	return sSource;
}

static uint	ComplieShader(uint type, const std::string& src)
{
	unsigned int id = glCreateShader(type);
	const char* source = src.c_str();
	glShaderSource(id, 1, &source, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message= (char *)alloca(length * sizeof(int));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static uint CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	uint program = glCreateProgram();
	uint vs = ComplieShader(GL_VERTEX_SHADER, vertexShader);
	uint fs = ComplieShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "error" << std::endl;
		return -1;
	}
	//print open gl version
	//std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[6] = { 
		-0.5f, 0.0f, 
		 0.0f, 0.5f, 
		 0.5f, 0.0f };

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	ShaderSources* ss = ParseShader("res/sources.shader");
	std::cout << ss->vertexShader << std::endl;
	std::cout << ss->fragmentShader << std::endl;

	uint shader = CreateShader(ss->vertexShader, ss->fragmentShader);
	delete(ss);
	glUseProgram(shader);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//legacy opengl
	/*	glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f,-0.5f);
		glVertex2f(0.0f,0.0f);
		glVertex2f(0.5f, -0.5f);
		glEnd();*/

		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements()
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}