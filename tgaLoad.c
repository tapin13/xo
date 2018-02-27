#include "tgaLoad.h"
#include "loadFile.h"

struct TGAHeader {
	uint8_t  idlength; // Длина идентификатора (1 байт) Указывает количество байт, содержащихся в поле Идентификатор. Максимальное количество символов — 255. Нулевое значение указывает, что файл не содержит поле Идентификатор.
	uint8_t  colormap; // Тип карты цветов (1 байт) Указывает тип карты цветов, включенной в файл. На данный момент(описание от января 1991г) определено два значения — 0(файл не содержит карты цветов) и 1(файл содержит карту цветов).
	uint8_t  datatype; // Тип изображения (1 байт) TGA файл может быть использован для сохранения изображений с различной глубиной цвета.
	uint8_t  colormapinfo[5]; // Описание карты цветов (5 байт) Это поле и его подразделы описывают карту цветов (если она используется). Если поле Color Map Type(описано выше) содержит ноль, эти пять байт также должны содержать нули. 
                                    // Индекс первого элемента 	2 байта
                                    // Длина карты цветов 	2 байта
                                    // Размер элемента карты цветов 	1 байт 
	uint16_t xorigin; // Положение изображения по оси X 
	uint16_t yorigin; // Положение изображения по оси Y 
	uint16_t width; // Ширина изображения
	uint16_t height; // Высота изображения 	2 байта 
	uint8_t  bitperpel; // Глубина цвета 	1 байт 
	uint8_t  description; // Описатель изображения 	1 байт 
                                // Биты 3-0 этого поля определяют количество бит альфа-канала. 
                                // Биты 5 и 4 определяют порядок передачи пиксельных данных из файла на экран. Бит 4 устанавливается для порядка «слева направо», бит 5 — для порядка «сверху вниз».
};


GLuint TextureFromTGA(const char *filename) {
    struct TGAHeader *header;
    GLint format, internalFormat;
    GLuint texture;
    
    uint8_t *buffer;
    
    buffer = readFile(filename, 1);
    
    if(buffer == NULL) {
        return -1;
    }
    
    header = (struct TGAHeader*)buffer;
    
    /*
    printf("file idlength %d\n", header->idlength);
    printf("file colormap %d\n", header->colormap);
    printf("file datatype %d\n", header->datatype);
    printf("file xorigin %d\n", header->xorigin);
    printf("file yorigin %d\n", header->yorigin);
    printf("file width %d\n", header->width);
    printf("file height %d\n", header->height);
    printf("file bitperpel %d\n", header->bitperpel);
    printf("file description %d\n", header->description);
    */
    
    format = header->bitperpel == 24 ? GL_RGB : GL_RGBA;
    internalFormat = format == GL_RGB ? GL_RGB8 : GL_RGBA8;
    
    glGenTextures(1, &texture);
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    //Допустим, для некого пикселя на экране были определены соответствующие UV-координаты на текстуре. 
    //Мы можем сказать OpenGL тупо использовать цвет текселя, который находится по этим координатам:
    // GL_NEAREST
    // GL_LINEAR - взвешенное среднее четырех ближайших к полученным координатам текселей
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //GL_CLAMP_TO_EDGE — при значении > 1 используется 1, а в случае < 0 используется 0;
    //// в константах вместо букв U и V используются S и T
    // Остается только указать правильный параметр GL_TEXTURE_MIN_FILTER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header->width, header->height, 0, format, GL_UNSIGNED_BYTE, (const GLvoid *)(buffer + sizeof(struct TGAHeader) + header->idlength));
    glGenerateMipmap(GL_TEXTURE_2D);
    
    free(buffer);
    
    return texture;
}

