#pragma once
class Renderer
{
private:
	GLuint programCompute;
	GLuint programRasterization;
	GLuint programFont;
	GLuint positionBuffer;
	GLuint velocityBuffer;
	GLuint vertexBufferCube;
	GLuint vertexBufferFont;
	GLuint indexBufferCube;
	GLuint vertexArrayCube;
	GLuint vertexArrayFont;
	GLuint textureFont;
	int texture_width,texture_height;
	double deltaTime;
	const int maxParticles;
	char* readShaderSource(const char * fileName);
	GLuint createShader(GLuint shader1,GLuint shader2=0);
	GLuint compileShader(char* fileName,GLenum type);
	void renderText(std::string text, float x, float y, glm::vec3 color);
	void updateBoidVB();
	GLint deltaLoc;
	GLint colorLoc;
	GLint colorLocFont;
	GLint mvpLoc;
	GLint orthoLoc;
	glm::mat4 mvp;
	glm::mat4 proj;
	glm::mat4 ortho;
public:
	Renderer():programCompute(0),programRasterization(0),programFont(0),positionBuffer(0),velocityBuffer(0),textureFont(0),
		vertexBufferCube(0),indexBufferCube(0),vertexBufferFont(0),maxParticles(10000){}
	~Renderer()
	{
		if(programCompute)
			glDeleteProgram(programCompute);
		if(programRasterization)
			glDeleteProgram(programRasterization);
		if(programFont)
			glDeleteProgram(programFont);
		glDeleteBuffers(1,&positionBuffer);
		glDeleteBuffers(1,&velocityBuffer);
		glDeleteBuffers(1,&vertexBufferCube);
		glDeleteBuffers(1,&indexBufferCube);
		glDeleteBuffers(1,&vertexBufferFont);
		glDeleteTextures(1,&textureFont);
	}
	void CreateShaderCompute()
	{
		programCompute = createShader(compileShader("ComputeShader.glsl",GL_COMPUTE_SHADER));
		deltaLoc = glGetUniformLocation(programCompute, "u_deltaTime");
	}
	void CreateShaderRaster()
	{
		programRasterization = createShader(compileShader("VertexShader.glsl",GL_VERTEX_SHADER),compileShader("FragmentShader.glsl",GL_FRAGMENT_SHADER));
		colorLoc = glGetUniformLocation(programRasterization,"u_Color");
		mvpLoc = glGetUniformLocation(programRasterization, "u_mvp");
	}
	void CreateShaderFont()
	{
		programFont = createShader(compileShader("VertexShaderFont.glsl",GL_VERTEX_SHADER),compileShader("FragmentShaderFont.glsl",GL_FRAGMENT_SHADER));
		colorLocFont = glGetUniformLocation(programFont,"u_Color");
		orthoLoc = glGetUniformLocation(programFont, "u_mproj");
	}
	void CreatePositionBuffer();
	void CreateVelocityBuffer();
	void DispatchShaderCompute();
	void InitOpenGL(float width, float height);
	void Clear(){glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);}
	void CreateCubeBuffers();
	void DrawCube();
	void DrawBoids();
	void DrawStats();
	void Update();
	void CreateFont();
	static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};