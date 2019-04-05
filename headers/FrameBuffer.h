#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
#include <OpenGL/gl3.h>

class FrameBuffer {
private:
	GLuint frameBufferID;
	std::vector<GLuint> textures;
	std::vector<GLenum> colorAttachments;
	GLuint depthBuffer = 0;


public:
	FrameBuffer();
	~FrameBuffer();
	void bindFrameBuffer();
	int addTexture(int width, int height);
	void bindTexture(int index, GLenum textureUnit); // bind the texture
	void addDepthBuffer(int width, int height);

};

#endif