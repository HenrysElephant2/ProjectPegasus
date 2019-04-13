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
CLEAN=rm -f $(EXE) *.o *.a $(OBJDIR)*.o $(OBJDIR)*.a
endif

INC := ./headers
SRC := ./source
OBJDIR := ./Objects/

CC = g++ -g -std=c++11 -c $(CFLG) -I$(INC) -o $@ $<


$(OBJDIR)Main.o: $(SRC)/Main.cpp
	$(CC)
$(OBJDIR)ComponentManager.o: $(SRC)/GameEngine/ComponentManager.cpp $(INC)/ComponentManager.h $(INC)/Component.h
	$(CC)
$(OBJDIR)EntityManager.o: $(SRC)/GameEngine/EntityManager.cpp $(INC)/EntityManager.h
	$(CC)
$(OBJDIR)MessageManager.o: $(SRC)/GameEngine/MessageManager.cpp $(INC)/MessageManager.h
	$(CC)
$(OBJDIR)System.o: $(SRC)/GameEngine/Systems/System.cpp $(INC)/System.h
	$(CC)
$(OBJDIR)ECSEngine.o: $(SRC)/GameEngine/ECSEngine.cpp $(INC)/ECSEngine.h
	$(CC)
$(OBJDIR)LevelLoader.o: $(SRC)/LevelLoader/LevelLoader.cpp $(INC)/LevelLoader.h
	$(CC)
$(OBJDIR)SceneLoader.o: $(SRC)/LevelLoader/SceneLoader.cpp $(INC)/LevelLoader.h
	$(CC)
$(OBJDIR)TextureLoader.o: $(SRC)/LevelLoader/TextureLoader.cpp $(INC)/Material.h $(INC)/stb_image.h
	$(CC)
$(OBJDIR)ShaderManager.o: $(SRC)/ShaderManager.cpp $(INC)/ShaderManager.h
	$(CC)
$(OBJDIR)Gameplay.o: $(SRC)/Gameplay.cpp $(INC)/Gameplay.h $(INC)/State.h
	$(CC)
$(OBJDIR)RenderSystem.o: $(SRC)/GameEngine/Systems/RenderSystem.cpp $(INC)/RenderSystem.h
	$(CC)
$(OBJDIR)FrameBuffer.o: $(SRC)/GameEngine/Systems/FrameBuffer.cpp $(INC)/FrameBuffer.h
	$(CC)
$(OBJDIR)PlayerMovementSystem.o: $(SRC)/GameEngine/Systems/PlayerMovementSystem.cpp $(INC)/PlayerMovementSystem.h
	$(CC)




# .c.o:
# 	gcc -g -c $(CFLG) $<
# #$(OBJDIR)/%.o: %.cpp# .cpp.o:
#  .cpp.o:
# 	g++ -g -std=c++11 -c $(CFLG) -I$(INC) $^


Pegasus: $(OBJDIR)Main.o $(OBJDIR)ComponentManager.o $(OBJDIR)EntityManager.o $(OBJDIR)MessageManager.o $(OBJDIR)System.o  $(OBJDIR)ECSEngine.o $(OBJDIR)LevelLoader.o $(OBJDIR)SceneLoader.o $(OBJDIR)TextureLoader.o $(OBJDIR)ShaderManager.o $(OBJDIR)Gameplay.o $(OBJDIR)RenderSystem.o $(OBJDIR)FrameBuffer.o $(OBJDIR)PlayerMovementSystem.o
	g++ -O3 -o $@ $^ $(LIBS)

clean:
	$(CLEAN)
