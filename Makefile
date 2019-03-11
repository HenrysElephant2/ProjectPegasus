EXE=Pegasus

all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=-lglut32cu -lglu32 -lopengl32 -lSDL2
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL -F /Library/Frameworks -framework SDL2
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm -lSDL2
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

ComponentManager.o: ComponentManager.cpp ComponentManager.h Component.h
EntityManager.o: EntityManager.cpp EntityManager.h
Main.o: Main.cpp
MessageManager.o: MessageManager.cpp MessageManager.h
System.o: System.cpp System.h



.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -std=c++11 -c $(CFLG) $<


Pegasus:Main.o ComponentManager.o EntityManager.o MessageManager.o System.o
	g++ -O3 -o $@ $^ $(LIBS)

clean:
	$(CLEAN)