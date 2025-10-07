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

enum GameState {START, RUN, PAUSE, SETUP};
string gameStateToString(GameState state) {
    switch (state) {
        case START: return "START";
        case RUN:   return "RUN";
        case PAUSE: return "PAUSE";
        case SETUP: return "SETUP";
        default:    return "UNKNOWN";
    }
}
Uint32 countdown_start_time = 0;
//Frees media and shuts down SDL
void close();
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
GameState game = START;
SDL_Scancode lastkey;
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
	VCourtTexture = loadTexture( "assets/beach1.png", gRenderer );
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
void drawCountdown(int countdown);
void drawMap(){
	SDL_FRect srcRect = {0, 0, 400, 430};
	SDL_FRect dstRect = {0, 0, 1000, 750};
	SDL_RenderTexture(gRenderer, VCourtTexture, &srcRect, &dstRect);
	srcRect = {((netframe/3)%6)*45.0f, 0, 45, 450};
	dstRect = {485, 400, 45, 350};
	SDL_RenderTexture(gRenderer, VNetTexture, &srcRect, &dstRect);
	netframe = ((netframe +1))%18;
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
void drawPause(Uint32 start){
	Uint32 now = SDL_GetTicks();
    int elapsed = (now - start) / 1000; 
    int remaining = 3 - elapsed;    
	if(remaining > 0) drawCountdown(remaining);  
	else{
		game = SETUP;
	}       
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
			characters.push_back( new Character(gRenderer, make_tuple(250,500), make_tuple(0,430)));
			characters.push_back( new Character(gRenderer, make_tuple(750,500), make_tuple(530,950), "Char3"));
			characters.push_back( new Character(gRenderer, make_tuple(50,500), make_tuple(0,430), "Char2", false));
			characters.push_back( new Character(gRenderer, make_tuple(950,500), make_tuple(530,950), "Char4", false));
			Ball* ball = nullptr;    //70,300
			while( quit == false ){ 
				// cout<<gameStateToString(game)<<endl;
				SDL_KeyboardEvent key;
				Uint64 frameStart = SDL_GetTicks();
				while( SDL_PollEvent( &e ) ){ 
					if( e.type == SDL_EVENT_QUIT ) quit = true; 
					else if ((e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) && game == RUN){
						for (auto character : characters) {
							if(e.key.scancode == SDL_SCANCODE_R && lastkey != SDL_SCANCODE_R) {
								cout<<"RRRRRRRRRRRRRRRRR"<<endl;
								character->changeControl();
							}
    					}
						lastkey = e.key.scancode;
						key = e.key;
					}
					else if (game == START && e.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
						float x = e.button.x, y=e.button.y;
						if(x<650 && x>350 && y<425 && y>325){
							game = SETUP;
						}
					}
					else if ( e.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
						cout<< e.button.x<<endl;
					}

				}
				drawMap();
				if(game == START || game == SETUP){
					drawCountdown(-1);
					if(game == START) SDL_RenderTexture(gRenderer, TS, new SDL_FRect{0,0,500,500}, new SDL_FRect{350,325,300,100});
					else{
						if(ball){
							delete ball;
							ball = nullptr;
						}
						ball = new Ball(gRenderer, make_tuple(0,400));
						cout<<"Create new ball"<<endl;
					} 
				}
				if(game != START){
					for (auto character : characters) {
						if(game == SETUP){
							character->setPosition();
							character->SetAIControl(ball);
							cout<<"Set AI"<<endl;
						}
						character->getKeyboardEvent(key);
						character->update_position();
						character->render();
						if(game == RUN && !ball->Isdead()){
							ball->checkCollision(character); // nguoi danh dc bong
						}
					}
					if(game != SETUP){          // RUN, PAUSE
						if(!ball->Isdead()){
							if(ball->update_position() && game != PAUSE){
								countdown_start_time = SDL_GetTicks();
								game = PAUSE;
							} 
							ball->render();
						}
						TSL = assets[LEFT];
						TSR = assets[RIGHT+10];
						if(game != RUN) drawPause(countdown_start_time);
						//Update screen
						drawTS();
					}
					else{ game = RUN;}
					
				}
				
						
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
