/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, and strings
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "bot.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed PNG image
SDL_Surface* VCourtSurface = NULL;
SDL_Surface* VNetSurface = NULL;
int netframe = 0;
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( !SDL_Init( SDL_INIT_VIDEO ) )
	{
		SDL_Log( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
		if( gWindow == NULL )
		{
			SDL_Log( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG surface
	VCourtSurface = loadSurface( "assets/beachbkgIso.png" );
	VNetSurface = loadSurface( "assets/net0.png" );
	if( VCourtSurface == NULL )
	{
		SDL_Log( "Failed to load PNG image!\n" );
		success = false;
	}

	return success;
}
void drawMap(){
	SDL_Rect srcRect = {0, 0, 400, 430};
	SDL_Rect dstRect = {0, 0, 1000, 750};
	SDL_BlitSurfaceScaled(VCourtSurface, &srcRect, gScreenSurface, &dstRect, SDL_SCALEMODE_LINEAR);
	srcRect = {(int(netframe/3)%6)*45, 0, 45, 450};
	dstRect = {450, 60, 100, 690};
	SDL_BlitSurfaceScaled(VNetSurface, &srcRect, gScreenSurface, &dstRect, SDL_SCALEMODE_LINEAR);
	netframe = ((netframe +1))%18;
}
void close()
{
	//Free loaded image
	SDL_DestroySurface( VCourtSurface );
	VCourtSurface = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		SDL_Log( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			SDL_Log( "Failed to load media!\n" );
		}
		else
		{	
			//Hack to get window to stay up
			const int targetFPS = 15; // FPS mục tiêu
    		const Uint64 frameTime = 1000 / targetFPS; // Thời gian mỗi khung hình (ms)
            SDL_Event e; bool quit = false;
			Character* character = new Character(make_tuple(50,400), make_tuple(0,410));
			Ball* ball = new Ball(make_tuple(0,400));
			while( quit == false ){ 
				Uint64 frameStart = SDL_GetTicks();
				while( SDL_PollEvent( &e ) ){ 
					if( e.type == SDL_EVENT_QUIT ) quit = true; 
					else if (e.type ==  SDL_EVENT_KEY_DOWN){
						character->getKeyboardEvent(e.key);
					}
					else if (e.type ==  SDL_EVENT_KEY_UP){
						character->getKeyboardEvent(e.key);
					}
					else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
						std::cout << "Mouse button pressed at: ("
                              << e.button.x << ", " << e.button.y << ")\n";
					}

				}
				drawMap();
				character->update_position();
				character->render(gScreenSurface);
				if(!ball->Isdead()){
					ball->update_position();
					ball->render(gScreenSurface);
				}
				SDL_UpdateWindowSurface(gWindow);
				Uint64 frameEnd = SDL_GetTicks(); // Thời gian kết thúc khung hình
				Uint64 frameDuration = frameEnd - frameStart; // Thời gian xử lý khung hình

				if (frameDuration < frameTime) {
					SDL_Delay(frameTime - frameDuration); // Tạm dừng để đạt FPS mục tiêu
        		} 
			}
			delete character;
			delete ball;
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
