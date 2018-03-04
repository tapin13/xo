LDLIBS=-lGL -lGLEW -lglfw3

all:	clean
	#gcc xo.c loadFile.c loadShader.c tgaLoad.c -Wall -lglfw3 -lGL -lGLEW -lm -lX11 -lpthread -ldl -o xo
	#gcc xo.c -lGLEW -lGL -lglfw -o xo
	gcc xo.c loadFile.c loadShader.c tgaLoad.c -Wall -lglfw -lGL -lGLEW -lm -lX11 -lpthread -ldl -o xo
	
clean:
	rm -f *.o xo

	
.PHONY: clean all 