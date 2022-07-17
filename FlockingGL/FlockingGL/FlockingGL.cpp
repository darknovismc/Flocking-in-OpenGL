// FlockingGL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Renderer.h"

int main(void)
{
	const int wndWidth = 1024;
	const int wndHeight = 768;
    GLFWwindow* window;
	std::srand((unsigned int)std::time(nullptr));
    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(wndWidth, wndHeight, "Flocking", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	try
	{
		Renderer rend;
		rend.InitOpenGL((float)wndWidth, (float)wndHeight);
		rend.CreateShaderCompute();
		rend.CreateShaderRaster();
		rend.CreateShaderFont();
		rend.CreatePositionBuffer();
		rend.CreateVelocityBuffer();
		rend.CreateCubeBuffers();
		rend.CreateFont();
		while (!glfwWindowShouldClose(window))
		{
			rend.Clear();
			rend.Update();
			rend.DispatchShaderCompute();
			rend.DrawCube();
			rend.DrawBoids();
			rend.DrawStats();
			glfwSwapBuffers(window);
			glfwPollEvents();
			if(glfwGetKey(window,GLFW_KEY_ESCAPE))
				break;
		}
	}
	catch(std::runtime_error& err)
	{
		std::cout <<"\nException occured!\n";
		std::cout <<err.what();
		std::cout <<"\nPress a key to exit.";
		std::getchar();
	}
    glfwTerminate();
	
    return 0;
}

