//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <iostream>

#include "LevelLoader.h"

// Much of this file is based on LazyFoo's SDL with OpenGL tutorial: http://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/

// Screen dimension variables
static int SCREEN_WIDTH = 1400;
static int SCREEN_HEIGHT = 900;

bool quit = false;
SDL_Window* gWindow = NULL;
SDL_GLContext gContext;


bool initGL() {
	bool success = true;
	GLenum error = GL_NO_ERROR;
	
	error = glGetError();
	if( error != GL_NO_ERROR )
		success = false;

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	error = glGetError();
	if( error != GL_NO_ERROR )
		success = false;
	
	glClearColor( 0, 0, 0, 1 );
	
	error = glGetError();
	if( error != GL_NO_ERROR )
		success = false;
	
	return success;
}

bool init() {
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );


		gWindow = SDL_CreateWindow( "Project Pegasus", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP );
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

				if( !initGL() ) {
					printf( "Unable to initialize OpenGL!\n" );
					success = false;
				}
			}
		}
	}

	return success;
}


void close() {
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	SDL_Quit();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glOrtho(0,SCREEN_WIDTH/SCREEN_HEIGHT,0,1,-1,1);

	glBegin(GL_TRIANGLES);
	glColor3d(1,1,0);
	glVertex3d(0,0,0);
	glVertex3d(0,1,0);
	glVertex3d(1,.5,0);
	glEnd();
}

int main( int argc, char* args[] ) {
	if( !init() ) {
		printf( "Failed to initialize!\n" );
	}
	else {
		SDL_GetWindowSize(gWindow, &SCREEN_WIDTH, &SCREEN_HEIGHT);
		// GameState *currentState = new Menu(SCREEN_WIDTH, SCREEN_HEIGHT);
		// currentState->reshape(SCREEN_WIDTH, SCREEN_HEIGHT);

		SDL_Event e;
		SDL_StartTextInput();
		//Alex's test code

		Scene scene = Scene();
		std::string s("testwarehouse.dae");
		scene.openFile(s);
		scene.print();
		//end test code

		while( !quit ) {
			while( SDL_PollEvent( &e ) != 0 ) {
				if( e.type == SDL_QUIT ) {
					quit = true;
				}
				else if( e.type == SDL_KEYDOWN ) {
					SDL_Keycode key = e.key.keysym.sym;
					// currentState->keyDown( key );
				}
				else if( e.type == SDL_KEYUP ) {
					SDL_Keycode key = e.key.keysym.sym;
					// currentState->keyUp( key );
				}
				else if( e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED ) {
					SCREEN_WIDTH = e.window.data1;
					SCREEN_HEIGHT = e.window.data2;
					// currentState->reshape(SCREEN_WIDTH, SCREEN_HEIGHT);
				}
				else if( e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT ) {
					int x = 0, y = 0;
					SDL_GetMouseState( &x, &y );
					// currentState->mouseDown( x, y );
				}
				else if( e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT ) {
					int x = 0, y = 0;
					SDL_GetMouseState( &x, &y );
					// currentState->mouseUp( x, y );
				}
			}
			// currentState->update();
			// currentState->display();

			display();

			// GameState *nextState = currentState->getNextState();
			// if( nextState != NULL ) {
			//	 delete currentState;
			//	 currentState = nextState;
			//	 currentState->reshape(SCREEN_WIDTH, SCREEN_HEIGHT);
			// }

			// if( currentState->getQuit() )
			//	 quit = true;

			if( glGetError() != GL_NO_ERROR )
				std::cout << "OpenGL Error Occurred" << std::endl;
			
			SDL_GL_SwapWindow( gWindow );
		}
		
		SDL_StopTextInput();
	}

	close();

	return 0;
}


