#include "loadFile.h"

uint8_t* readFile(const char *filename, char binary) {
    uint8_t *buffer = NULL;
    
    uint32_t size;
    
    FILE *fp;
    
    fp = fopen(filename, "rb");
    if(fp == NULL) {
        printf("Failed to open %s\n", filename);
        return buffer;
    }
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    rewind(fp);
    if(binary == 1) {
        buffer = malloc(size * sizeof(uint8_t)); // uint8_t eq unsigned char
    } else {
        buffer = malloc((size + 1) * sizeof(uint8_t)); // uint8_t eq unsigned char
    }
    
    fread(buffer, 1, size, fp);
    fclose(fp);
    
    if(binary == 0) {
        buffer[size] = 0;
    }
    
    return buffer;
}

