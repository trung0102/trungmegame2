/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL3/SDL.h>
#include <stdio.h>
#include "bot.h"
//Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;


int main( int argc, char* args[] )
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	
	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Create window
		window = SDL_CreateWindow( "SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{	
			SDL_SetWindowPosition(window, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED);
			//Get window surface
			screenSurface = SDL_GetWindowSurface( window );

			//Fill the surface white
			// CONST COLOR
			const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(screenSurface->format);
			Uint32 colorWhite = SDL_MapRGB(format, NULL, 0xFF, 0xFF, 0xFF);
			Uint32 colorRed = SDL_MapRGB(format, NULL, 0xFF, 0x00, 0x00);
			Uint32 colorBlue = SDL_MapRGB(format, NULL, 0x00, 0x00, 0xFF);
			Uint32 colorBlack = SDL_MapRGB(format, NULL, 0x00, 0x00, 0x00);

			SDL_FillSurfaceRect( screenSurface, NULL, colorWhite );
			
			//Update the surface
			SDL_UpdateWindowSurface( window );
            
            //Hack to get window to stay up
			const int targetFPS = 15; // FPS mục tiêu
    		const Uint64 frameTime = 1000 / targetFPS; // Thời gian mỗi khung hình (ms)
            SDL_Event e; bool quit = false;
			Character* character = new Character(make_tuple(500,500), make_tuple(0,1000));
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

				}
				SDL_FillSurfaceRect(screenSurface, NULL, colorWhite);
				character->update_position();
				character->render(screenSurface);
				SDL_UpdateWindowSurface(window);
				Uint64 frameEnd = SDL_GetTicks(); // Thời gian kết thúc khung hình
				Uint64 frameDuration = frameEnd - frameStart; // Thời gian xử lý khung hình

				if (frameDuration < frameTime) {
					SDL_Delay(frameTime - frameDuration); // Tạm dừng để đạt FPS mục tiêu
        } 
			}
			delete character;
		}
	}

	//Destroy window
	SDL_DestroyWindow( window );


	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}


// /*This source code copyrighted by Lazy Foo' Productions 2004-2024
// and may not be redistributed without written permission.*/

// //Using SDL, SDL_image, and strings
// #include <SDL3/SDL.h>
// #include <SDL3/SDL_main.h>
// #include <SDL3_image/SDL_image.h>
// #include <string>

// //Screen dimension constants
// const int SCREEN_WIDTH = 640;
// const int SCREEN_HEIGHT = 480;

// //Starts up SDL and creates window
// bool init();

// //Loads media
// bool loadMedia();

// //Frees media and shuts down SDL
// void close();

// //Loads individual image
// SDL_Surface* loadSurface( std::string path );

// //The window we'll be rendering to
// SDL_Window* gWindow = NULL;
	
// //The surface contained by the window
// SDL_Surface* gScreenSurface = NULL;

// //Current displayed PNG image
// SDL_Surface* gPNGSurface = NULL;

// bool init()
// {
// 	//Initialization flag
// 	bool success = true;

// 	//Initialize SDL
// 	if( !SDL_Init( SDL_INIT_VIDEO ) )
// 	{
// 		SDL_Log( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
// 		success = false;
// 	}
// 	else
// 	{
// 		//Create window
// 		gWindow = SDL_CreateWindow( "SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
// 		if( gWindow == NULL )
// 		{
// 			SDL_Log( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
// 			success = false;
// 		}
// 		else
// 		{
// 			//Get window surface
// 			gScreenSurface = SDL_GetWindowSurface( gWindow );
// 		}
// 	}

// 	return success;
// }

// bool loadMedia()
// {
// 	//Loading success flag
// 	bool success = true;

// 	//Load PNG surface
// 	gPNGSurface = loadSurface( "bot.png" );
// 	if( gPNGSurface == NULL )
// 	{
// 		SDL_Log( "Failed to load PNG image!\n" );
// 		success = false;
// 	}

// 	return success;
// }

// void close()
// {
// 	//Free loaded image
// 	SDL_DestroySurface( gPNGSurface );
// 	gPNGSurface = NULL;

// 	//Destroy window
// 	SDL_DestroyWindow( gWindow );
// 	gWindow = NULL;

// 	//Quit SDL subsystems
// 	SDL_Quit();
// }

// SDL_Surface* loadSurface( std::string path )
// {
// 	//The final optimized image
// 	SDL_Surface* optimizedSurface = NULL;

// 	//Load image at specified path
// 	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
// 	if( loadedSurface == NULL )
// 	{
// 		SDL_Log( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError() );
// 	}
// 	else
// 	{
// 		//Convert surface to screen format
// 		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format );
// 		if( optimizedSurface == NULL )
// 		{
// 			SDL_Log( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
// 		}

// 		//Get rid of old loaded surface
// 		SDL_DestroySurface( loadedSurface );
// 	}

// 	return optimizedSurface;
// }

// int main( int argc, char* args[] )
// {
// 	//Start up SDL and create window
// 	if( !init() )
// 	{
// 		SDL_Log( "Failed to initialize!\n" );
// 	}
// 	else
// 	{
// 		//Load media
// 		if( !loadMedia() )
// 		{
// 			SDL_Log( "Failed to load media!\n" );
// 		}
// 		else
// 		{	
// 			//Main loop flag
// 			bool quit = false;

// 			//Event handler
// 			SDL_Event e;

// 			//While application is running
// 			while( !quit )
// 			{
// 				//Handle events on queue
// 				while( SDL_PollEvent( &e ) != 0 )
// 				{
// 					//User requests quit
// 					if( e.type == SDL_EVENT_QUIT )
// 					{
// 						quit = true;
// 					}
// 				}

// 				//Apply the PNG image
// 				SDL_BlitSurface( gPNGSurface, NULL, gScreenSurface, NULL );
			
// 				//Update the surface
// 				SDL_UpdateWindowSurface( gWindow );
// 			}
// 		}
// 	}

// 	//Free resources and close SDL
// 	close();

// 	return 0;
// }
