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
// enum State = {START, PAUSE, RUN};
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();
void drawNumber(int x, int y, int number, int size);
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed PNG image
SDL_Texture* VCourtTexture = NULL;
SDL_Texture* VNetTexture = NULL;
SDL_Texture* TSL,*TSR,*TS = NULL;
int netframe = 0;
unordered_map<int, SDL_Texture*> assets;
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
	assets = {
    { 0,	loadTexture("assets/orange_0.png", gRenderer ) },
    { 1,    loadTexture("assets/orange_1.png", gRenderer ) },
    { 2,   	loadTexture("assets/orange_2.png", gRenderer ) },
    { 3,    loadTexture("assets/orange_3.png", gRenderer ) },
    { 4,    loadTexture("assets/orange_4.png", gRenderer ) },
    { 5,    loadTexture("assets/orange_5.png", gRenderer ) },
    { 6,    loadTexture("assets/orange_6.png", gRenderer ) },
	{ 7,	loadTexture("assets/orange_7.png", gRenderer ) },
    { 8,    loadTexture("assets/orange_8.png", gRenderer ) },
    { 9,   	loadTexture("assets/orange_9.png", gRenderer ) },
    
	{ 10,	loadTexture("assets/green_0.png", gRenderer ) },
    { 11,   loadTexture("assets/green_1.png", gRenderer ) },
    { 12,   loadTexture("assets/green_2.png", gRenderer ) },
    { 13,   loadTexture("assets/green_3.png", gRenderer ) },
    { 14,   loadTexture("assets/green_4.png", gRenderer ) },
    { 15,   loadTexture("assets/green_5.png", gRenderer ) },
    { 16,   loadTexture("assets/green_6.png", gRenderer ) },
	{ 17,	loadTexture("assets/green_7.png", gRenderer ) },
    { 18,   loadTexture("assets/green_8.png", gRenderer ) },
    { 19,   loadTexture("assets/green_9.png", gRenderer ) }};
	TS = loadTexture("assets/bangtiso.png", gRenderer );

	return success;
}
void drawTS(int size = 0){
	SDL_RenderTexture(gRenderer, TS, new SDL_FRect{0,0,500,500}, new SDL_FRect{400,-30,200,200});
	SDL_RenderTexture(gRenderer, TSL, new SDL_FRect{0,0,300,300}, new SDL_FRect{407,37,100,100});
	SDL_RenderTexture(gRenderer, TSR, new SDL_FRect{0,0,300,300}, new SDL_FRect{493,37,100,100});
}
void drawMap(){
	SDL_FRect srcRect = {0, 0, 400, 430};
	SDL_FRect dstRect = {0, 0, 1000, 750};
	SDL_RenderTexture(gRenderer, VCourtTexture, &srcRect, &dstRect);
	srcRect = {((netframe/3)%6)*45.0f, 0, 45, 450};
	dstRect = {450, 60, 100, 690};
	SDL_RenderTexture(gRenderer, VNetTexture, &srcRect, &dstRect);
	netframe = ((netframe +1))%18;
	// drawTS();
}

void drawCountdown(int countdown) {
	// Vẽ nền mờ
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 150);
	SDL_FRect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_RenderFillRect(gRenderer, &overlay);

	// Vẽ số đếm ngược lớn
	if (countdown > 0) {
		float centerX = SCREEN_WIDTH / 2 - 150;
		float centerY = SCREEN_HEIGHT / 2 - 150;
		SDL_RenderTexture(gRenderer, assets[countdown], new SDL_FRect{0,0,300,300}, new SDL_FRect{centerX,centerY,300,300});
	}
}
void drawNumber(int x, int y, int num, int size) {
	
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
			const int targetFPS = 20; // FPS mục tiêu
    		const Uint64 frameTime = 1000 / targetFPS; // Thời gian mỗi khung hình (ms)
            SDL_Event e; bool quit = false;
			vector<Character*> characters;
			characters.push_back( new Character(gRenderer, make_tuple(50,400), make_tuple(0,410)));
			characters.push_back( new Character(gRenderer, make_tuple(950,400), make_tuple(540,950), "Char3"));
			Ball* ball = new Ball(gRenderer, make_tuple(70,300));    //70,300
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
				if(!ball){
					ball = new Ball(gRenderer, make_tuple(70,300));
				}
				drawMap();
				for (auto character : characters) {
					character->update_position();
					character->render();
					if(!ball->Isdead()){
						ball->checkCollision(character);
					}
				}
				if(!ball->Isdead()){
					ball->update_position();
					ball->render();
				}
				TSL = assets[LEFT];
				TSR = assets[RIGHT+10];
				
				//Update screen
				drawTS();

				// drawCountdown(3);
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
