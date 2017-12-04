#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include<sstream>
#include<fstream>
#include<string>

#define ASSERT(x) if(!(x)) __debugbreak();

#ifdef DEBUG 
	#define GLCALL(x) GlClearError();\
	x;\
	ASSERT(GlCheckError(#x, __FILE__,__LINE__));
#else
	#define GLCALL(x) x;
#endif 

typedef unsigned int uint;


static void GlClearError() 
{
	while (glGetError());
}

static bool GlCheckError(const char* f, const char* file, int line)
{
	while (GLenum error = glGetError()) {
		std::cout << "GL error :" << error <<" in "<< file<<" at "<<line<< std::endl;
		return false;
	}
	return true;
}

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
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "error" << std::endl;
		return -1;
	}
	//print open gl version
	//std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = { 
		 -0.5f, -0.5f, 
		  0.5f, -0.5f, 
		  0.5f, 0.5f,
	
		 //-0.5f,-0.5f,
		 -0.5f,0.5f,
		 //0.5f, 0.5f
		};

	uint indices[] = {
		0, 1, 2,
		0, 3, 2
	};

	uint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);


	uint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	ShaderSources* ss = ParseShader("res/sources.shader");
	std::cout << ss->vertexShader << std::endl;
	std::cout << ss->fragmentShader << std::endl;

	uint shader = CreateShader(ss->vertexShader, ss->fragmentShader);
	delete(ss);
	GLCALL(glUseProgram(shader));

	GLCALL(GLint location = glGetUniformLocation(shader,"u_Color"));
	ASSERT(location != -1);
	GLCALL(glUniform4f(location, 0.9f, 0.4f, 0.7f, 1.0f));
	float r = 0.0f;
	float increment = 0.05f;
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
		
		//GlClearError();
		//glDrawArrays(GL_TRIANGLES, 0, 6); //draw square using vertex buffer
		//glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr); //draw square using index buffer
		//ASSERT(GlCheckError());

		GLCALL(glUniform4f(location, r, 0.4f, 0.0f, 1.0f));
		//use macros to wrap the open gl function with error handling code
		GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		
		//Simple color toggle logic
		if (r > 1.0f)
			increment = -0.05f;
		else if(r < 0.0f)
			increment = 0.05f;
		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}