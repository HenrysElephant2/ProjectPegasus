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

INC := ./headers
SRC := ./source
OBJDIR := ./Objects

CC = g++ -g -std=c++11 -c $(CFLG) -I$(INC) $<


Main.o: $(SRC)/Main.cpp
	$(CC)
ComponentManager.o: $(SRC)/GameEngine/ComponentManager.cpp $(INC)/ComponentManager.h $(INC)/Component.h
	$(CC)
EntityManager.o: $(SRC)/GameEngine/EntityManager.cpp $(INC)/EntityManager.h
	$(CC)
MessageManager.o: $(SRC)/GameEngine/MessageManager.cpp $(INC)/MessageManager.h
	$(CC)
System.o: $(SRC)/GameEngine/Systems/System.cpp $(INC)/System.h
	$(CC)
ECSEngine.o: $(SRC)/GameEngine/ECSEngine.cpp $(INC)/ECSEngine.h
	$(CC)
LevelLoader.o: $(SRC)/LevelLoader/LevelLoader.cpp $(INC)/LevelLoader.h
	$(CC)
SceneLoader.o: $(SRC)/LevelLoader/SceneLoader.cpp $(INC)/LevelLoader.h
	$(CC)
TextureLoader.o: $(SRC)/LevelLoader/TextureLoader.cpp $(INC)/Material.h $(INC)/stb_image.h
	$(CC)
ShaderManager.o: $(SRC)/ShaderManager.cpp $(INC)/ShaderManager.h
	$(CC)
Gameplay.o: $(SRC)/Gameplay.cpp $(INC)/Gameplay.h $(INC)/State.h
	$(CC)
RenderSystem.o: $(SRC)/GameEngine/Systems/RenderSystem.cpp $(INC)/RenderSystem.h
	$(CC)




# .c.o:
# 	gcc -g -c $(CFLG) $<
# #$(OBJDIR)/%.o: %.cpp# .cpp.o:
#  .cpp.o:
# 	g++ -g -std=c++11 -c $(CFLG) -I$(INC) $^


Pegasus: Main.o ComponentManager.o EntityManager.o MessageManager.o System.o  ECSEngine.o LevelLoader.o SceneLoader.o TextureLoader.o ShaderManager.o Gameplay.o RenderSystem.o
	g++ -O3 -o $@ $^ $(LIBS)

clean:
	$(CLEAN)
