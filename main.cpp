#if 0
#!/bin/sh
g++ -g -Wall -lGLU -lftgl `sdl-config --cflags` models.cpp Transformation.cpp ctexture.cpp cgame.cpp cboard.cpp ccamera.cpp cbutton.cpp main.cpp -o main `sdl-config --libs` -lSDL_net -lSDL_image

exit
#endif


#include "SDL.h"
#include "SDL_opengl.h"
#define NO_SDL_GLEXT
#include "SDL_image.h"
#include "SDL_thread.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include <FTGL/ftgl.h>

#include "models.h"
#include "Transformation.h"
#include "ccamera.h"
#include "ctexture.h"
#include "cboard.h"
#include "cgame.h"
#include "game_mainmenu.h"
#include "poller.h"

#include "main.h"

#   define FTGL_RENDER_ALL   0xffff

using namespace std; 
using namespace FTGL; 


int mainLoop( ){
	while( 1 ){ 

		// must pump the eventLoop
		printf( "\nmainLoop has the semaphore\n" );
		SDL_mutexP( availableEvents );
		if( queueNonEmpty ){ 
			SDL_CondWait( drainedPump, availableEvents );
		}
		SDL_PumpEvents( );
		queueNonEmpty = true; 

		SDL_mutexV( availableEvents );
		SDL_CondSignal( donePumping );
		static int numTimes =0;
		numTimes++;
		// done pumping the eventLoop

		game_Generic = CGame::getGameByGameState( gameState ); 
		game_Generic->callRenderScene( );


		// change this a bit to use SDL_GetTicks( )
		SDL_Delay( TIME_SLICE );

		continue;

	}

	return 1; 
}

void cleanUp( ){ 
	SDL_Quit( ); 
	printf( "\nCleaned up. \n" ); 

	return; 
}

void initThreads( ){ 
	poller_thread = SDL_CreateThread( poller_0, NULL );
	availableEvents = SDL_CreateMutex( );
	donePumping = SDL_CreateCond( );
	drainedPump = SDL_CreateCond( );

	return;
}

void initLighting( ){ 
	glEnable( GL_LIGHTING );

	glEnable( GL_NORMALIZE );

	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

	float cmbr[4] = { 0.4, 0.4, 0.4, 1.0 };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, cmbr );

	float l1Amb[4] = { 0.4, 0.4, 0.4, 1.0 };
	float l1Diff[4] = { 0.4, 0.4, 0.4, 1.0 };
	float l1Spec[4] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv( GL_LIGHT0, GL_AMBIENT, l1Amb );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, l1Diff );
	glLightfv( GL_LIGHT0, GL_SPECULAR, l1Spec );
	float l1Pos[4] = { 0.0, 0.0, 50.0, 1.0 };
	glLightfv( GL_LIGHT0, GL_POSITION, l1Pos );
	glEnable( GL_LIGHT0 );

	return; 
}

void setup_rc( ){ 
	glEnable( GL_TEXTURE_2D );

	glEnable( GL_DEPTH_TEST ); 
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	glShadeModel( GL_SMOOTH ); 

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable( GL_POLYGON_SMOOTH ); 
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	initLighting( );
	
	return; 
}

void initObjeks( ){ 
	board.init( BOARD_WIDTH, BOARD_LENGTH, BOARD_THICKNESS ); 

	font.FaceSize(30);
	if( font.Error( ) ){ 
		exit( 0 ); 
	}


	return; 
}

void initGameStates( ){ 
	NSGame_MainMenu::init( );

	return;
}

float get_GW( ){ 
	const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo( ); 
	return vidInfo->current_w;
}

float get_GH( ){ 
	const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo( ); 
	return vidInfo->current_h;
}

int main( int argc, char *argv[] ){ 

        if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 ) {
                printf("Unable to initialize SDL: %s\n", SDL_GetError());
                return 1;
        }
         
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 16 );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // *new*

        G_screen = SDL_SetVideoMode( GW, GH, 32, SDL_OPENGL ); // *changed*
	SDL_WM_SetCaption( "first_sdl_ogl", NULL );

        setup_rc( ); 

	initObjeks( );
	initGameStates( );
	initThreads( );

	mainLoop( );

	cleanUp( ); 

        return 0; 
}
