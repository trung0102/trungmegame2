#include "include/bot.h"

// Character::Character(/* args */)
// {
// }

// Character::~Character()
// {
// }
unordered_map<PlayerAction, ActionData> actionData = {
    { PlayerAction::Idle,           {12, loadSurface("assets/playerIdle.png")} },
    { PlayerAction::MoveForward,    {12, loadSurface("assets/playerRun.png")} },
    { PlayerAction::MoveBackward,   {12, loadSurface("assets/playerRun.png")} },
    { PlayerAction::Spike,          {13, loadSurface("assets/playerSmash.png")} },
    { PlayerAction::Block,          {13, loadSurface("assets/playerBlock.png")} },
    { PlayerAction::Dive,           {15, loadSurface("assets/playerSlide.png")} },
    { PlayerAction::Pass,           {11, loadSurface("assets/playerReception.png")} },
};

Character:: Character(tuple<int, int> position, tuple<int, int> patrol_range){
    ActionData action = actionData[this->status];
    this->position = position;
    this->patrol_range = patrol_range;
    this->max_frame = action.maxframe;
    this->surface = action.surface;
    this->srcRect = {0, 0, 32, 48};
    this->dstRect = {get<0>(position), get<1>(position), 32*2, 48*2};
}
Character:: ~Character(){
    SDL_DestroySurface(this->surface);
}
void Character::update_position(){
    this->current_frame = (this->current_frame +1) % this->max_frame;
    this->srcRect.x = this->current_frame*32;
    if(this->status == PlayerAction::MoveForward  && this->dstRect.x < get<1>(this->patrol_range)) this->dstRect.x += this->speed;
    else if(this->status == PlayerAction::MoveBackward  && this->dstRect.x > get<0>(this->patrol_range)) this->dstRect.x -= (this->speed*3)/5;
    get<0>(this->position) = this->dstRect.x;
}
void Character::render(SDL_Surface* screenSurface){
    SDL_BlitSurfaceScaled(this->surface, &this->srcRect, screenSurface, &this->dstRect, SDL_SCALEMODE_LINEAR);
}
void Character::getKeyboardEvent(SDL_KeyboardEvent keyEvent){
    PlayerAction curr_status = this->status;
    if (keyEvent.type == SDL_EVENT_KEY_UP){
        this->status = PlayerAction::Idle;
    }
    else{
        switch (keyEvent.scancode){
        case SDL_SCANCODE_A:
            this->status = PlayerAction::MoveBackward;
            break;
        case SDL_SCANCODE_D:
            this->status = PlayerAction::MoveForward;
            break;
        case SDL_SCANCODE_S:
            this->status = PlayerAction::Pass;
            break;
        case SDL_SCANCODE_W:
            this->status = PlayerAction::Spike;
            break;
        case SDL_SCANCODE_UNKNOWN:
            this->status = PlayerAction::Idle;
            break;
        }
    }
    
    if(curr_status != this->status) this->current_frame = 0;
    ActionData action = actionData[status];
    this->max_frame = action.maxframe;
    this->surface = action.surface;
}

Ball:: Ball(tuple<int, int> position, int max_frame){
    this->current_frame = 0;
    this->position = position;
    this->max_frame = max_frame;
    this->surface = loadSurface("assets/playerRun.png");
    
}
Ball:: ~Ball(){
    SDL_DestroySurface(this->surface);
}
void Ball::update_position(){
    this->current_frame = (this->current_frame +1) % this->max_frame;
    get<0>(this->position) += 10;
    this->srcRect.x = this->current_frame*32;
    // this->dstRect.x += this->speed;
}
void Ball::render(SDL_Surface* screenSurface){
    SDL_BlitSurfaceScaled(this->surface, &this->srcRect, screenSurface, &this->dstRect, SDL_SCALEMODE_LINEAR);
}

SDL_Surface* loadSurface( std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		SDL_Log( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError() );
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, SDL_PIXELFORMAT_RGBA8888 );
		if( optimizedSurface == NULL )
		{
			SDL_Log( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_DestroySurface( loadedSurface );
	}

	return optimizedSurface;
}
