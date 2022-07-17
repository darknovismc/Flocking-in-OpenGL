#include "stdafx.h"
#include "stb_image.h"
#include "Renderer.h"

void GLAPIENTRY Renderer::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) 
{
	char* _source;
    char* _type;
    char* _severity;

    switch (source) 
	{
        case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;
        case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;
        case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;
        default:
        _source = "UNKNOWN";
        break;
    }
    switch (type) 
	{
        case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;
        case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;
        case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;
        case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;
        case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;
        default:
        _type = "UNKNOWN";
        break;
    }
    switch (severity) 
	{
        case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;
        case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;
        default:
        _severity = "UNKNOWN";
        break;
    }
    std::cout <<"[OpenGL Error]("<<"Source:"<<_source<<" Type:"<<_type<<" Severity:"<<_severity<<")"<<std::endl<<message<<std::endl;
}

char* Renderer::readShaderSource(const char * fileName)
{
	std::ifstream file(fileName,std::ios::binary);

	if(!file.is_open())
		throw std::runtime_error("Shader file not opened!");
	file.seekg(0,std::ios::end);
	std::streamoff size = file.tellg();       
    file.seekg(0,std::ios::beg);

	char* buf = new char[(unsigned int)size+1];
	file.read(buf,size);
    buf[size] = NULL;
	file.close();
	return buf;
}

void Renderer::InitOpenGL(float width, float height)
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::string msg = "Initialization problem:";
		msg += (char*)glewGetErrorString(err);
		throw std::runtime_error(msg);
	}
	else if (GLEW_VERSION_4_6)
		std::cout <<"OpneGL version 4.6 supported!\n";
	else if(GLEW_VERSION_4_5)
		std::cout <<"OpneGL version 4.5 supported!\n";
	else if(GLEW_VERSION_4_4)
		std::cout <<"OpneGL version 4.4 supported!\n";
	else 
		throw std::runtime_error("OpenGL version not supported!\n");
	std::cout << glGetString(GL_VERSION)<<std::endl;
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageControl(GL_DEBUG_SOURCE_API,GL_DONT_CARE,GL_DEBUG_SEVERITY_NOTIFICATION,0,nullptr,GL_FALSE);
    glDebugMessageCallback(MessageCallback, 0);
	glEnable(GL_DEPTH_TEST);
	proj = glm::perspective(90.0f, width / height, 0.01f, 100.0f);
	ortho = glm::ortho(0.0f, width, 0.0f, height);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

GLuint Renderer::compileShader(char* fileName,GLenum type)
{
	GLuint shader = glCreateShader(type);
	char* shader_source =readShaderSource(fileName);
	glShaderSource(shader , 1 ,&shader_source , 0);
	delete[] shader_source;
	glCompileShader(shader);
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::unique_ptr<GLchar[]> errorLog(new GLchar[maxLength]);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		std::string msg = fileName+std::string(" Shader compilation failed:\n");
		msg+=&errorLog[0];
		throw std::runtime_error(msg);
	}
	return shader;
}

GLuint Renderer::createShader(GLuint shader1,GLuint shader2)
{
	GLuint program = glCreateProgram();
	glAttachShader(program , shader1);
	if(shader2)
		glAttachShader(program , shader2);
	glLinkProgram(program);
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::unique_ptr<GLchar[]> errorLog(new GLchar[maxLength]);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

		std::string msg = "Shader linking failed:";
		msg+=&errorLog[0];
		throw std::runtime_error(msg);
	}
	glValidateProgram(program);
	GLint isValid = 0;
	glGetProgramiv(program,GL_VALIDATE_STATUS,&isValid);
	if (isValid == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::unique_ptr<GLchar[]> errorLog(new GLchar[maxLength]);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

		std::string msg = "Shader validation failed:";
		msg+=&errorLog[0];
		throw std::runtime_error(msg);
	}
	glDeleteShader(shader1);
	if(shader2)
		glDeleteShader(shader2);
	return program;
}

void Renderer::CreatePositionBuffer()
{
	glGenBuffers(1 ,&positionBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER , positionBuffer);
	glBufferData( GL_SHADER_STORAGE_BUFFER , maxParticles* sizeof(glm::vec4) ,NULL , GL_STATIC_DRAW);
	glm::vec4* positions = (glm::vec4 *)glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, maxParticles* sizeof(glm::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (GLint i = 0; i < maxParticles; i++)
		positions[i] = glm::vec4((float)std::rand()/RAND_MAX*2-1.0f,(float)std::rand()/RAND_MAX*2-1.0f,(float)std::rand()/RAND_MAX*2-1.0f,1);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Renderer::CreateVelocityBuffer()
{
	glGenBuffers(1 ,&velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER , velocityBuffer);
	glBufferData( GL_SHADER_STORAGE_BUFFER , maxParticles* sizeof(glm::vec4) ,NULL , GL_STATIC_DRAW);
	glm::vec4* velocities = (glm::vec4 *)glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, maxParticles* sizeof(glm::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (GLint i = 0; i < maxParticles; i++)
		velocities[i] = glm::vec4((float)std::rand()/RAND_MAX*2-1.0f,(float)std::rand()/RAND_MAX*2-1.0f,(float)std::rand()/RAND_MAX*2-1.0f,1);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Renderer::CreateCubeBuffers()
{
	GLfloat vertices[] = {
		-1.0f,1.0f,1.0f,//0
		-1.0f,-1.0f,1.0f,//1
		1.0f,1.0f,1.0f,//2
		1.0f,-1.0f,1.0f,//3
		-1.0f,1.0f,-1.0f,//4
		-1.0f,-1.0f,-1.0f,//5
		1.0f,1.0f,-1.0f,//6
		1.0f,-1.0f,-1.0f//7
	};
	unsigned int indices[] = {
		0, 2, 2, 3, 3, 1, 1, 0,
		2, 6, 6, 7, 7, 3, 3, 2,
		6, 4, 4, 0, 0, 2, 2, 6,
		0, 4, 4, 5, 5, 1, 1, 0,
		0, 4, 4, 6, 6, 2, 2, 0,
		1, 5, 5, 7, 7, 3, 3, 1,
	};
	glGenVertexArrays(1, &vertexArrayCube);
	glBindVertexArray(vertexArrayCube);

	glGenBuffers(1, &indexBufferCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferCube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &vertexBufferCube);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::DispatchShaderCompute()
{
	glUseProgram(programCompute);
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER , 0 , positionBuffer);
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER , 1 , velocityBuffer);
	glUniform1f(deltaLoc,(float)deltaTime);
	glDispatchComputeGroupSizeARB(maxParticles/100,1,1,100,1,1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Renderer::Update()
{
	static double old_t = 0;
	double t;
	t = glfwGetTime();
	deltaTime = (t - old_t);
	old_t = t;
	glm::mat4 view = glm::lookAt(glm::vec3(2.5f*glm::cos((float)t*0.1f), 2.5f*glm::sin((float)t*0.1f), 0.5f),glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f));
	mvp = proj * view;
}

void Renderer::DrawCube()
{
	glDisable(GL_BLEND);
	glUseProgram(programRasterization);
	glBindVertexArray(vertexArrayCube);
	glUniform4f(colorLoc,1,0,0,1);
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferCube);
	glDrawElements(GL_LINES,8*6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Renderer::DrawBoids()
{
	glUseProgram(programRasterization);
	glUniform4f(colorLoc,0.6f,0.6f,1,1);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glVertexPointer(4,GL_FLOAT, sizeof(glm::vec4), (GLvoid*)0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, maxParticles);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Renderer::DrawStats()
{
	glEnable(GL_BLEND);
	glBindVertexArray(vertexArrayFont);
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(programFont);
	glUniformMatrix4fv(orthoLoc, 1, GL_FALSE, glm::value_ptr(ortho));
	renderText(std::string("NUM:")+std::to_string((long long)(maxParticles)),0,16,glm::vec3(1.0f,1.0f,1.0f));
	renderText(std::string("FPS:")+std::to_string((long long)(1/deltaTime)),0,0,glm::vec3(1.0f,1.0f,1.0f));
}

void Renderer::CreateFont()
{
	int bpp;
	unsigned char* data = stbi_load("Font.bmp",&texture_width, &texture_height,&bpp,4);
	if(!data)
		throw std::runtime_error("Font file not found!");

	glGenTextures(1 ,&textureFont);
	glBindTexture(GL_TEXTURE_2D, textureFont);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindImageTexture(0 ,textureFont, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	if(data)
		stbi_image_free(data);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glGenVertexArrays(1, &vertexArrayFont);
	glGenBuffers(1, &vertexBufferFont);
	glBindVertexArray(vertexArrayFont);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferFont);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 
}

void Renderer::renderText(std::string text, float x, float y, glm::vec3 color)
{
	glUniform4f(colorLocFont,color.r,color.g,color.b,1);
	int w = texture_width/8;
	int h = texture_height/8;
	for (unsigned int i=0;i<text.length();i++)
    {
		float xpos = x+i*w;
		char chOffset = text[i]-(char)(' ');
		float u = (float)(chOffset%8)/8.0f;
		float v =  (float)(chOffset/8)/8.0f;
		float vertices[6][4] = 
		{
			{xpos,     y + h,		 u,        v},            
			{xpos,     y,			 u, v+0.125f},
			{xpos + w, y,     u+0.125f, v+0.125f},
			{xpos,     y + h,        u,        v},
			{xpos + w, y,     u+0.125f, v+0.125f},
			{xpos + w, y + h, u+0.125f,        v}           
		};
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferFont);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}