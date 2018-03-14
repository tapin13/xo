LDLIBS=-lglfw3 -lGL -lGLEW -lm -lX11 -lpthread -ldl
CFLAGS=-Wall
SOURCES=xo.c loadFile.c loadShader.c tgaLoad.c

all: $(SOURCES) 
	gcc $(SOURCES) $(CFLAGS) $(LDLIBS) -o xo
	
clean:
	rm -f *.o xo

install:
	#install xo 
uninstall:
	#rm xo
	
.PHONY: all clean