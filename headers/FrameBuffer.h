#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <SDL2/SDL_opengl.h>

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
	GLuint getTextureIdentifier(int index);
	int addIntegerTexture(int width, int height);
	void bindTexture(int index, GLenum textureUnit); // bind the texture
	void addDepthBuffer(int width, int height);
	void setDepthOnlyTexture( GLuint depthTex );
	void setCubeTexture( GLuint texi, int face );
};

#endif