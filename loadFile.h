#ifndef LOADFILE_H
#define LOADFILE_H

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

uint8_t* readFile(const char *filename, char binary);

#endif /* LOADFILE_H */