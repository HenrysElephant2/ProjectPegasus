EXE=Pegasus

all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=-lglu32 -lopengl32 -lSDL2
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations

LIBS=-framework OpenGL -F /Library/Frameworks -framework SDL2 -lassimp
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm -lSDL2
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif


Main.o: Main.cpp
ComponentManager.o: ComponentManager.cpp ComponentManager.h Component.h
EntityManager.o: EntityManager.cpp EntityManager.h
LoadShader.o: LoadShader.c LoadShader.h
MessageManager.o: MessageManager.cpp MessageManager.h
System.o: System.cpp System.h
SceneLoader.o: SceneLoader.cpp LevelLoader.h
TempRender.o: TempRender.cpp TempRender.h


.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -std=c++11 -c $(CFLG) $<


Pegasus: Main.o ComponentManager.o EntityManager.o LoadShader.o MessageManager.o System.o TempRender.o SceneLoader.o
	g++ -O3 -o $@ $^ $(LIBS)

clean:
	$(CLEAN)