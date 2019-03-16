
#ifndef LOAD_SHADER_H
#define LOAD_SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include <OpenGL/gl3.h>

#ifdef __cplusplus
extern "C" {
#endif


int CreateShader(GLenum type,char* file);
char* ReadText(char *file);
void PrintShaderLog(int obj,char* file);
void PrintProgramLog(int obj);
int CreateShaderProg(char* VertFile,char* FragFile);
void Fatal(const char* format , ...);

#ifdef __cplusplus
}
#endif
#endif