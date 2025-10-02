/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, and strings
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "bot.h"
#include <vector>

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

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed PNG image
SDL_Texture* VCourtTexture = NULL;
SDL_Texture* VNetTexture = NULL;
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
		//Create window and renderer
		if( !SDL_CreateWindowAndRenderer( "SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &gWindow, &gRenderer ) )
		{
			SDL_Log( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Initialize renderer color
			SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;
	
	//Load PNG surface
	VCourtTexture = loadTexture( "assets/beachbkgIso.png", gRenderer );
	VNetTexture = loadTexture( "assets/net0.png", gRenderer );
	if( VCourtTexture == NULL )
	{
		SDL_Log( "Failed to load PNG image!\n" );
		success = false;
	}

	return success;
}
void drawMap(){
	SDL_FRect srcRect = {0, 0, 400, 430};
	SDL_FRect dstRect = {0, 0, 1000, 750};
	SDL_RenderTexture(gRenderer, VCourtTexture, &srcRect, &dstRect);
	srcRect = {((netframe/3)%6)*45.0f, 0, 45, 450};
	dstRect = {450, 60, 100, 690};
	SDL_RenderTexture(gRenderer, VNetTexture, &srcRect, &dstRect);
	netframe = ((netframe +1))%18;
}
void close()
{
	//Free loaded image
	SDL_DestroyTexture( VCourtTexture );
	VCourtTexture = NULL;
	SDL_DestroyTexture( VNetTexture );
	VNetTexture = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

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
			const int targetFPS = 25; // FPS mục tiêu
    		const Uint64 frameTime = 1000 / targetFPS; // Thời gian mỗi khung hình (ms)
            SDL_Event e; bool quit = false;
			vector<Character*> characters;
			characters.push_back( new Character(gRenderer, make_tuple(50,400), make_tuple(0,410)));
			Ball* ball = new Ball(make_tuple(0,400));
			while( quit == false ){ 
				Uint64 frameStart = SDL_GetTicks();
				while( SDL_PollEvent( &e ) ){ 
					if( e.type == SDL_EVENT_QUIT ) quit = true; 
					else if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP){
						for (auto character : characters) {
        					character->getKeyboardEvent(e.key);
    					}
					}
					else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
						std::cout << "Mouse button pressed at: ("
                              << e.button.x << ", " << e.button.y << ")\n";
					}

				}
				drawMap();
				for (auto character : characters) {
					character->update_position();
					character->render();
				// 	if(!ball->Isdead()){
				// 		ball->checkCollision(character);
				// 	}
				// }
				// if(!ball->Isdead()){
				// 	ball->update_position();
				// 	ball->render(gScreenSurface);
				}
				
				//Update screen
				SDL_RenderPresent( gRenderer );

				Uint64 frameEnd = SDL_GetTicks(); // Thời gian kết thúc khung hình
				Uint64 frameDuration = frameEnd - frameStart; // Thời gian xử lý khung hình

				if (frameDuration < frameTime) {
					SDL_Delay(frameTime - frameDuration); // Tạm dừng để đạt FPS mục tiêu
        		} 
			}
			for (auto c : characters) {
				delete c;
			}
			characters.clear();
			delete ball;
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
