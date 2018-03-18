#ifndef TGALOAD_H
#define TGALOAD_H

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint TextureFromTGA(const char *filename);

#endif /* TGALOAD_H */