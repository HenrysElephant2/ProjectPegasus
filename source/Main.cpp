#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
//#include "TempRender.h"

#include "LevelLoader.h"
#include "State.h"
#include "Gameplay.h"
#include "ShaderManager.h"

// Screen dimension constants
int SCREEN_WIDTH = 1400;
int SCREEN_HEIGHT = 900;

bool init();
void handleKeys( unsigned char key );
void handleMouse( int x, int y );
void close();
double getElapsedTime();

SDL_Window* gWindow = NULL;
SDL_GLContext gContext;
Uint64 previousTime = 0;
int prevMouseX = 0, prevMouseY = 0;

State *state;
ShaderManager * sm;

bool quit = false;
bool capturedMode = true;


bool init() {
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		// Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

		gWindow = SDL_CreateWindow( "Project Pegasus", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE ); // SDL_WINDOW_FULLSCREEN_DESKTOP
		if( gWindow == NULL ) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			gContext = SDL_GL_CreateContext( gWindow );
			if( gContext == NULL ) {
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				if( SDL_GL_SetSwapInterval( 1 ) < 0 )
					printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
			}
		}
	}

	return success;
}

void handleKeys( SDL_Event e ) {
	if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q )
		quit = true;
	else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE ) {
		capturedMode = !capturedMode;
		SDL_SetRelativeMouseMode(capturedMode?SDL_TRUE:SDL_FALSE);
	}
	else if( capturedMode )
		state->keyEvent( &e );
}

void handleMouse( SDL_Event e ) {
	if( capturedMode )
		state->mouseEvent( &e );
}

void close() {
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	SDL_Quit();
	delete state;
}

double getElapsedTime() {
	Uint64 currentTime = SDL_GetPerformanceCounter();
	double deltaTime = (double)((currentTime - previousTime) * 1000 / (double)SDL_GetPerformanceFrequency());
	previousTime = currentTime;
	return deltaTime / 1000;
}

int main( int argc, char* args[] ) {
	if( !init() ) {
		printf( "Failed to initialize!\n" );
	}
	else {
		sm = ShaderManager::createShaderManager();
		ShaderManager::loadShaders(sm);

		std::string levelname;
		if(argc == 2)
			levelname = args[1];
		else levelname = "";
		state = new Gameplay(SCREEN_WIDTH, SCREEN_HEIGHT, levelname);


		SDL_SetRelativeMouseMode(capturedMode?SDL_TRUE:SDL_FALSE);
		SDL_Event e;
		
		SDL_StartTextInput();

		while( !quit ) {
			while( SDL_PollEvent( &e ) != 0 ) {
				if( e.type == SDL_QUIT )
					quit = true;
                else if( e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED ) {
                    SCREEN_WIDTH = e.window.data1;
                    SCREEN_HEIGHT = e.window.data2;
                   	state->reshape(SCREEN_WIDTH, SCREEN_HEIGHT);
                }
				else if( e.type == SDL_KEYDOWN || e.type == SDL_KEYUP ) {
					handleKeys( e );
				}
				else if( e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP ||  e.type == SDL_MOUSEMOTION ) {
					handleMouse( e );
				}
			}

			//rs->render();
			//rs->update( getElapsedTime() );
			//std::cout << "updating state" << std::endl;
			state->update();
			SDL_GL_SwapWindow( gWindow );
			if( glGetError() != GL_NO_ERROR ) 
				std::cout << "Fuck" << std::endl;
			
			
		}
		
		SDL_StopTextInput();
	}

	close();
	return 0;
}