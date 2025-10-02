#include "include/bot.h"

const int SAN_BALL = 455;

unordered_map<PlayerAction, ActionData> actionData = {
    { PlayerAction::Idle,           {12, loadSurface("assets/playerIdle.png")} },
    { PlayerAction::MoveForward,    {12, loadSurface("assets/playerRun.png")} },
    { PlayerAction::MoveBackward,   {12, loadSurface("assets/playerRun.png")} },
    { PlayerAction::Spike,          {13, loadSurface("assets/playerSmash.png")} },
    { PlayerAction::Block,          {13, loadSurface("assets/playerBlock.png")} },
    { PlayerAction::Dive,           {15, loadSurface("assets/playerSlide.png")} },
    { PlayerAction::Pass,           {11, loadSurface("assets/playerReception.png")} },
};

Character:: Character(tuple<int, int> position, tuple<int, int> patrol_range,string name){
    ActionData action = actionData[this->status];
    this->position = position;
    this->patrol_range = patrol_range;
    this->max_frame = action.maxframe;
    this->surface = action.surface;
    this->srcRect = {0, 0, 32, 48};
    this->dstRect = {get<0>(position), get<1>(position), 32*2, 48*2};
    this->name = name;
}
Character:: ~Character(){
    SDL_DestroySurface(this->surface);
}
void Character::update_position(){
    this->current_frame = (this->current_frame +1) % this->max_frame;
    this->srcRect.x = this->current_frame*32;
    if(this->status == PlayerAction::MoveForward  && get<0>(this->position) < get<1>(this->patrol_range)) get<0>(this->position) += this->speed;
    else if(this->status == PlayerAction::MoveBackward  && get<0>(this->position) > get<0>(this->patrol_range)) get<0>(this->position) -= (this->speed*3)/5;
    else if (this->status == PlayerAction::Spike ){
        int tmp = get<0>(this->position) + this->speed*2/5;
        if(tmp < get<1>(this->patrol_range)){get<0>(this->position) = tmp;}
        if(this->current_frame < 6){
            // cout<<"nhay"<<endl;
            get<1>(this->position) -= 15;
        }   
        else if(this->current_frame > 6){
            // cout<<"ha"<<endl;
            get<1>(this->position) += 15;
        }
    }
    
}
void Character::render(SDL_Surface* screenSurface){
    this->dstRect.x = get<0>(this->position);
    this->dstRect.y = get<1>(this->position);

    SDL_BlitSurfaceScaled(this->surface, &this->srcRect, screenSurface, &this->dstRect, SDL_SCALEMODE_LINEAR);
}
void Character::getKeyboardEvent(SDL_KeyboardEvent keyEvent){
    PlayerAction curr_status = this->status;
    if (keyEvent.type == SDL_EVENT_KEY_UP){
        if(this->y0){ 
                // cout<<this->y0<<endl;
                get<1>(this->position) = this->y0;
                this->y0=0;
            }
        this->status = PlayerAction::Idle;
    }
    else{
        if(keyEvent.scancode == SDL_SCANCODE_W){
            if(!this->y0){ 
                this->y0 = get<1>(this->position);
                // cout<<this->y0<<endl;
            }
            this->status = PlayerAction::Spike;
        }
        else{
            if(this->y0){ 
                // cout<<this->y0<<endl;
                get<1>(this->position) = this->y0;
                this->y0=0;
            }
            if(keyEvent.scancode == SDL_SCANCODE_A){
                this->status = PlayerAction::MoveBackward;
            }
            else if(keyEvent.scancode == SDL_SCANCODE_D){
                this->status = PlayerAction::MoveForward;
            }
            else if(keyEvent.scancode == SDL_SCANCODE_S){
                this->status = PlayerAction::Pass;
            }
            else{this->status = PlayerAction::Idle;}
        }
    }
    
    if(curr_status != this->status) this->current_frame = 0;
    ActionData action = actionData[status];
    this->max_frame = action.maxframe;
    this->surface = action.surface;
}

CharCollisionBall Character::checkCollision(const SDL_Rect& b){
    CharCollisionBall ret;
    SDL_Rect a;
    if(this->status == PlayerAction::Pass){
        a = {this->dstRect.x + 35, this->dstRect.y + 45, 30,30};
    }
    else{
        a ={0,0,0,0};
    }
    if(a.x + a.w < b.x || a.x > b.x + b.w || a.y + a.h < b.y || a.y > b.y + b.h){ 
        ret.is_collision = false;
    }
    else{
        ret.is_collision = true;
        ret.v0 = 80;
        ret.alpha = 85*M_PI/180;
    }
    return ret;
}





struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
    Vec2() = default;
    Vec2(float _x, float _y): x(_x), y(_y) {}
    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
    Vec2 operator*(float s) const { return {x*s, y*s}; }
    Vec2& operator+=(const Vec2& o){ x+=o.x; y+=o.y; return *this;}
    Vec2& operator*=(float s){ x*=s; y*=s; return *this;}
    Vec2 normalize() const {
        double mag = std::sqrt(x * x + y * y);
        return mag > 0 ? Vec2(x / mag, y / mag) : *this;
    }
    double dodaibinh() const {return x*x + y*y;}
    double dot(const Vec2& v) const { return x * v.x + y * v.y ; } // tích vô hướng
};



MotionEquation::MotionEquation(float alpha, float v0,float x0,float y0){
    this->v0 = v0;
    this->x0 = x0;
    this->y0 = y0;
    this->alpha = alpha;
    this->a = -9.8f / (2 * this->v0 * this->v0 * cos(this->alpha)*cos(this->alpha));
    this->b = tan(this->alpha);
    this->c = 0.0f;
}

MotionEquation::~MotionEquation(){
}

string MotionEquation::print(){
    return to_string(a) + " x^2 + " + to_string(b) + " x + " + to_string(c);
}

tuple<int,int> MotionEquation::position(float dt){
    float x,y;
    if(this->alpha == 0){
        x = (this->v0)* this->idx *dt;
        y = 0;
    }
    else{
        x = (this->v0 * cos(this->alpha))* this->idx *dt;
        y = -(this->a * x*x + this->b * x + this->c);   
    }
    if (this->idx == 1){
        this->x0 = this->x0 - x;
        this->y0 = this->y0 - y;
    }
    this->idx +=2;
    return make_tuple(static_cast<int>(this->x0 + x),static_cast<int>(this->y0 + y));
}
Vec2 MotionEquation::direction_vector(float x0){
    // he so goc tai x0
    float k = 2* this->a * x0 + this->b;
    return Vec2(1, k);
}








Ball:: Ball(tuple<int, int> position){
    this->current_frame = 0;
    this->position = position;
    this->max_frame = 8;
    this->surface = loadSurface("assets/ballRoll.png");
    this->srcRect = {0, 5, 15, 15};
    this->dstRect = {get<0>(position), get<1>(position), 15*2, 15*2};
    this->motition = new MotionEquation(85*M_PI/180, 40, get<0>(position), get<1>(position));
    // cout<< this->motition->print()<<endl;
}
Ball:: ~Ball(){
    SDL_DestroySurface(this->surface);
    delete this->motition;
}
void Ball::update_position(){
    this->current_frame = (this->current_frame +1) % (this->max_frame*2);
    tuple<int,int> pos_ball = this->motition->position(0.067);
    // cout << get<0>(pos_ball) << "  " << get<1>(pos_ball)<<endl;
    this->position = pos_ball;
    if(get<1>(this->position) > SAN_BALL) {
        get<1>(this->position) = SAN_BALL;
        this->collide("SAN");
    }
    else if (487 <= get<0>(this->position) + 30 && 502 >= get<0>(this->position) + 30 &&
         364 <= get<1>(this->position) + 30 && 479 >= get<1>(this->position) + 30) {
        // cout << "hahahaha" << endl;
        this->collide("Net");
    }
    this->srcRect.x = (int(this->current_frame/2)%this->max_frame)*15;
}

void Ball::collide(string str){
    Vec2 Oy,Ox;
    float base = 0;
    if(str == "SAN"){
        Oy = Vec2(0, -1);
        Ox = Vec2(1, 0);
    }
    else{
        Oy = Vec2(-1, 0);
        Ox = Vec2(0, -1);
        base = 3*M_PI/2;
    }
    this->isdead++;
    cout<<"hahahah"<<endl;
    Vec2 a = this->motition->direction_vector(get<0>(this->position));
    Vec2 a2 = a - Oy.normalize() * (2.0f * a.dot(Oy.normalize()));
    float costheta = sqrt(a2.dot(Ox)*a2.dot(Ox)/a2.dodaibinh()*Ox.dodaibinh());
    // cout<<"cosx "<<costheta<<endl;
    float theta = acos(costheta);
    if(str == "Net") theta = base - theta;
    if(theta > M_PI/2 && base == 0){theta = 0;}
    // cout<<"theta "<<theta<<endl;
    double v0 = this->motition->getV0() * 0.75;
    delete this->motition;
    this->motition = new MotionEquation(theta, v0, get<0>(this->position), get<1>(this->position));
}

void Ball::render(SDL_Surface* screenSurface){
    this->dstRect.x = get<0>(this->position);
    this->dstRect.y = get<1>(this->position);
    SDL_BlitSurfaceScaled(this->surface, &this->srcRect, screenSurface, &this->dstRect, SDL_SCALEMODE_LINEAR);
}

void Ball::checkCollision(Character* character){
    CharCollisionBall ele = character->checkCollision(this->dstRect);
    if(ele.is_collision){
        delete this->motition;
        motition = new MotionEquation(ele.alpha, ele.v0, get<0>(this->position), get<1>(this->position));
    }
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
