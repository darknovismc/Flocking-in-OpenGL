Small application that set ups OpenGL rendering API for using compute shaders to simulate forces between many particels(>10000). 
Please include glm library to build the project.
ComputeShader.glsl -main program where particle postion update is computed by the GPU.
FragmantShader.glsl - draw particles Fragment shader.
stb_image.cpp - helper function for loading image files. We load the font image file with it.
Renderer.cpp - main rendering class object encapsulation
