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
MessageManager.o: MessageManager.cpp MessageManager.h
System.o: System.cpp System.h
ECSEngine.o: ECSEngine.cpp ECSEngine.h
LevelLoader.o: LevelLoader.cpp LevelLoader.h
SceneLoader.o: SceneLoader.cpp LevelLoader.h
TextureLoader.o: TextureLoader.cpp Material.h stb_image.h
ShaderManager.o: ShaderManager.cpp ShaderManager.h
Gameplay.o: Gameplay.cpp Gameplay.h State.h
RenderSystem.o: RenderSystem.cpp RenderSystem.h


.c.o:
	gcc -g -c $(CFLG) $<
.cpp.o:
	g++ -g -std=c++11 -c $(CFLG) $<


Pegasus: Main.o ComponentManager.o EntityManager.o MessageManager.o System.o  ECSEngine.o LevelLoader.o SceneLoader.o TextureLoader.o ShaderManager.o Gameplay.o RenderSystem.o
	g++ -O3 -o $@ $^ $(LIBS)

clean:
	$(CLEAN)