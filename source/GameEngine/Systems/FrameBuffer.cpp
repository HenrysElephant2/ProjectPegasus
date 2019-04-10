#include "FrameBuffer.h"
FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

}
FrameBuffer::~FrameBuffer()
{	

}
void FrameBuffer::bindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}
int FrameBuffer::addTexture(int width, int height)
{
	bindFrameBuffer();
	int index = textures.size();
	textures.push_back(0);
	glGenTextures(1, &textures[index]);
	glBindTexture(GL_TEXTURE_2D, textures[index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, textures[index], 0);
	colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + index);

	glDrawBuffers(colorAttachments.size(), &colorAttachments[0]);
	return index;
}
void FrameBuffer::bindTexture(int index, GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, textures[index]);
}	

void FrameBuffer::addDepthBuffer(int width, int height)
{
	if(depthBuffer == 0)
	{
		bindFrameBuffer();
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	}
}