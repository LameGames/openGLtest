#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	// Open file for reading
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	// Read from file and add to the string streams
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if(line.find("vertex") != std::string::npos )
				type = ShaderType::VERTEX;
			else
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n' ;
		}
	}

	return { ss[0].str(), ss[1].str() };
}

// Creates a new shader and checks that it has compiled successfully
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	// Create a new shader and input the source code
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Check that the shader successfully compiled
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " <<  (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") <<"shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

// Returns a program with two shaders attached
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	// Create the new shaders and a program
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Attach the shaders to the program and check it is running correctly
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	// Shaders are no longer needed as they've already been linked in
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	GLFWwindow* window;
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1280, 720, "OpenGLtest", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "ERROR: GLEW NOT INITIALISED" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	/* Added comments */
	float positions[] = {
		-0.5f, -0.5f,	   // 0
		 0.5f, -0.5f,	   // 1
		 0.5f,  0.5f,	   // 2
		-0.5f,  0.5f,	   // 3
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int buff;
	glGenBuffers(1, &buff);
	glBindBuffer(GL_ARRAY_BUFFER, buff);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE ,2 * sizeof(float), 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// Grab the different shader codes from a file
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	std::string vertexShader = source.VertexSource;
	std::string fragmentShader = source.FragmentSource;
	// Create two new shaders and then put them into a program. Select it as program to use
	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	int rotation = 0;
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}