#include "include/bot.h"
#include "include/AI.h"

int LEFT = 0, RIGHT = 0;
const int SAN_BALL = 555;
KeyMap LeftKeys = { SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D };
KeyMap RightKeys = { SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT };

unordered_map<PlayerAction, ActionData> actionData = {
    { PlayerAction::Idle,           {12, "assets/playerIdle.png"} },
    { PlayerAction::MoveForward,    {12, "assets/playerRun.png"} },
    { PlayerAction::MoveBackward,   {12, "assets/playerRun.png"} },
    { PlayerAction::Spike,          {13, "assets/playerSmash.png"} },
    { PlayerAction::Block,          {13, "assets/playerBlock.png"} },
    { PlayerAction::Dive,           {15, "assets/playerSlide.png"} },
    { PlayerAction::Pass,           {11, "assets/playerReception.png"} },
};

Character:: Character(SDL_Renderer* gRenderer, tuple<int, int> position, tuple<int, int> patrol_range, string name, bool is_control){
    ActionData action = actionData[this->status];
    this->position = position;
    this->pos_0 = position;
    this->patrol_range = patrol_range;
    this->max_frame = action.maxframe;
    this->surface = loadTexture(action.surface, gRenderer);
    this->srcRect = {0, 0, 32, 48};
    this->dstRect = {float(get<0>(position)), float(get<1>(position)), 32*2, 48*2};
    this->name = name;
    this->gRenderer = gRenderer;
    this->is_control = is_control;
    this->AIcontrol = nullptr;
    if(this->name == "Char1" || this->name == "Char2" ){
        this->flipType = SDL_FLIP_NONE;
        this->keymap = LeftKeys;
        this->muiten = loadTexture("assets/mt2.png", gRenderer);
    }
    else{
        this->flipType = SDL_FLIP_HORIZONTAL;
        this->keymap = RightKeys;
        this->speed = - this->speed;
        this->muiten = loadTexture("assets/mt2.png", gRenderer);
    }
}
Character:: ~Character(){
    SDL_DestroyTexture(this->surface);
    if (this->AIcontrol) {
        delete this->AIcontrol;
        this->AIcontrol = nullptr;
    }
}
bool Character::isLeft(){
    return this->keymap == LeftKeys;
}
void Character::update_position(){
    this->current_frame = (this->current_frame +1) % this->max_frame;
    this->srcRect.x = this->current_frame*32;
    int minX = get<0>(this->patrol_range);
    int maxX = get<1>(this->patrol_range);
    bool dk1 = (!this->isLeft())? get<0>(this->position) > minX : get<0>(this->position) < maxX;
    bool dk2 = (!this->isLeft())? get<0>(this->position) < maxX : get<0>(this->position) > minX;
    if(this->status == PlayerAction::MoveForward && dk1) get<0>(this->position) += this->speed;
    else if(this->status == PlayerAction::MoveBackward  && dk2) get<0>(this->position) -= (this->speed*3)/5;
    else if (this->status == PlayerAction::Spike ){
        int tmp = get<0>(this->position) + this->speed*2/5;
        dk1 = (!this->isLeft())? tmp > minX : tmp < maxX;
        if(dk1){get<0>(this->position) = tmp;}
        if(this->current_frame < 6){
            // cout<<"nhay"<<endl;
            get<1>(this->position) -= 30;
        }   
        else if(this->current_frame > 6){
            // cout<<"ha"<<endl;
            get<1>(this->position) += 30;
        }
    }
    
}
void Character::render(){
    this->dstRect.x = get<0>(this->position);
    this->dstRect.y = get<1>(this->position);
    if(this->is_control) 
        SDL_RenderTexture(this->gRenderer, this->muiten, new SDL_FRect{0,0,980,980}, new SDL_FRect{float(get<0>(this->position))+10,float(get<1>(this->position))-50,32*2-20, 40});
    // SDL_RenderTexture(this->gRenderer, this->surface, &srcRect, &dstRect);
    SDL_RenderTextureRotated(this->gRenderer, this->surface, &srcRect, &dstRect,0,NULL,this->flipType);
    // SDL_BlitSurfaceScaled(this->surface, &this->srcRect, screenSurface, &this->dstRect, SDL_SCALEMODE_LINEAR);
}

void Character::getKeyboardEvent(SDL_KeyboardEvent keyEvent){
    PlayerAction curr_status = this->status;
    if(this->is_control){
        if (keyEvent.type == SDL_EVENT_KEY_UP){
            if(this->y0){ 
                    // cout<<this->y0<<endl;
                    get<1>(this->position) = this->y0;
                    this->y0=0;
                }
            this->status = PlayerAction::Idle;
        }
        else{
            if(keyEvent.scancode == this->keymap.up){
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
                if(keyEvent.scancode == this->keymap.left){
                    this->status = PlayerAction::MoveBackward;
                }
                else if(keyEvent.scancode == this->keymap.right){
                    this->status = PlayerAction::MoveForward;
                }
                else if(keyEvent.scancode == this->keymap.down){
                    this->status = PlayerAction::Pass;
                }
                else{
                    this->status = PlayerAction::Idle;
                }
            }
        }
    }
    else{
        // cout<< this->name<<"   ";
        if(!this->AIcontrol) cout<<"Not AI"<<endl;
        this->status = this->AIcontrol->Control();
    }
    
    
    if(curr_status != this->status) this->current_frame = 0;
    ActionData action = actionData[this->status];
    this->max_frame = action.maxframe;
    this->surface = loadTexture(action.surface, this->gRenderer);
}

CharCollisionBall Character::checkCollision(const SDL_FRect& b){
    CharCollisionBall ret;
    SDL_FRect a = {0,0,0,0};
    if(this->status == PlayerAction::Pass){
        a = (this->isLeft())?SDL_FRect{this->dstRect.x + 45, this->dstRect.y + 65, 20,10}:SDL_FRect{this->dstRect.x-1, this->dstRect.y + 65, 20,10};
    }
    else if(this->status == PlayerAction::Spike){
        if(this->current_frame >5 && this->current_frame <8){
            a = (this->isLeft())?SDL_FRect{this->dstRect.x + 40, this->dstRect.y + 20, 10,20}:SDL_FRect{this->dstRect.x+4, this->dstRect.y + 20, 10,20};
        }
    }
    if(a.x + a.w < b.x || a.x > b.x + b.w || a.y + a.h < b.y || a.y > b.y + b.h){ 
        ret.is_collision = false;
    }
    else{
        ret.is_collision = true;
        ret.v0 = (this->status == PlayerAction::Spike)?200:80;
        int value = rand() % (80 - 60 + 1) + 60;    // random tu 75 den 85
        // cout<<value<<endl;
        ret.alpha = (this->isLeft())?value*M_PI/180:(180-value)*M_PI/180;
    }
    ret.action = this->status;
    return ret;
}
void Character::SetAIControl(Ball* ball){
        if (this->AIcontrol) {
            delete this->AIcontrol;
            this->AIcontrol = nullptr;
        }
        this->AIcontrol = new AIControl(this, ball);
        if(this->AIcontrol) cout<<"Da co AI"<<endl;
    }
void Character::UpdateAI(){
        if(!this->is_control) this->AIcontrol->UpdateAI();
    }







MotionEquation::MotionEquation(float alpha, float v0,float x0,float y0, Vec2 a){
    this->v0 = v0;
    this->x0 = x0;
    this->y0 = y0;
    this->alpha = alpha;
    if(a == Vec2(0,0)){
        this->a = -9.8f / (2 * this->v0 * this->v0 * cos(this->alpha)*cos(this->alpha));
        this->b = tan(this->alpha);
        this->c = 0.0f;
    }
    else{
        this->a = 0;
        this->b = a.y/a.x;
        this->c = (-a.y*x0+a.x*y0)/a.x;
    }
    this->direction_vec = a;
}

MotionEquation::~MotionEquation(){
}

string MotionEquation::print(){
    return to_string(a) + " x^2 + " + to_string(b) + " x + " + to_string(c);
}

tuple<int,int> MotionEquation::position(float dt){
    float x,y;
    if(this->direction_vec == Vec2(0,0)){
        if(this->alpha == 0){
            x = (this->v0)* this->idx *dt;
            y = 0;
        }
        else{
            x = (this->v0 * cos(this->alpha))* this->idx *dt;
            y = -(this->a * x*x + this->b * x + this->c);   
        }
    }
    else{
        float i = (this->alpha < M_PI/2)?1:-1;
        x = i*(this->v0)* this->idx *dt;
        y = (this->a * x*x + this->b * x + this->c);
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
    if(this->direction_vec == Vec2(0,0)){
        float k = 2* this->a * x0 + this->b;
        return Vec2(1, k);
    }
    return this->direction_vec;
}

Vec2 direction_vector_A_to_B(Vec2 A, Vec2 B){
    return B-A;
}

float MotionEquation::SolveEquation(float y){
    
    // cout<< y<<endl;
    if(this->a == 0){
        return (y-this->c)/this->b;
    }
    else{
        y = y - this->y0;
        float delta = this->b*this->b - 4*this->a*(this->c + y);
        float tu_so;
        float mau_so = 2*this->a;
        if(this->alpha < M_PI/2){
            tu_so = - this->b - sqrt(delta);
            return this->x0 + tu_so/mau_so;
        }
        else{
            tu_so = - this->b + sqrt(delta);
            return this->x0 + tu_so/mau_so;
        }
    }
}






Ball:: Ball(SDL_Renderer* gRenderer, tuple<int, int> position, string a){
    this->current_frame = 0;
    this->position = position;
    this->max_frame = 8;
    this->gRenderer = gRenderer;
    this->surface = loadTexture("assets/ballRoll.png", gRenderer);
    this->duanh = loadTexture("assets/duanh.png", gRenderer);
    this->dubao = loadTexture("assets/vtrondubao2.png", gRenderer);
    this->srcRect = {0, 5, 15, 15};
    this->dstRect = {float(get<0>(position)), float(get<1>(position)), 15*2, 15*2};
    float theta = (a=="LEFT")?1*M_PI/24:M_PI - 1*M_PI/24;
    // this->motition = new MotionEquation(theta, 100, get<0>(position), get<1>(position));
    Vec2 vec_a = direction_vector_A_to_B(Vec2(float(get<0>(position)), float(get<1>(position))), Vec2(500,470));
    this->motition = new MotionEquation(M_PI/4,100,get<0>(position), get<1>(position),vec_a);
    // cout<< this->motition->print()<<endl;
    this->y_dubao = SAN_BALL+30 - 5.66*4;
    this->x_dubao = this->motition->SolveEquation();
    for(int i=0;i<7;++i){
        this->queue_pos.push(make_tuple(-50,0));
    }
    this->queue_pos.push(this->position);
}
Ball:: ~Ball(){
    SDL_DestroyTexture(this->surface);
    delete this->motition;
}
bool Ball::update_position(){
    this->current_frame = (this->current_frame +1) % (this->max_frame*2);
    tuple<int,int> pos_ball = this->motition->position(0.067*1.5);
    // cout << get<0>(pos_ball) << "  " << get<1>(pos_ball)<<endl;
    this->position = pos_ball;
    this->srcRect.x = (int(this->current_frame/2)%this->max_frame)*15;
    if(get<1>(this->position) > SAN_BALL) {
        get<1>(this->position) = SAN_BALL;
        get<0>(this->position) = this->Get_x_dubao(SAN_BALL);
        this->collide("SAN");
        this->create_new_motition = true;
    }
    // else if (487 <= get<0>(this->position) + 30 && 502 >= get<0>(this->position) + 30 &&
    //      364 <= get<1>(this->position) + 30 && 479 >= get<1>(this->position) + 30) {
    //     // cout << "hahahaha" << endl;
    //     this->collide("NET");
    // }
    else{ this->create_new_motition = false; }
    this->queue_pos.pop();
    this->queue_pos.push(this->position);
    return !this->can_touch;
}

void Ball::collide(string str){
    Vec2 Oy,Ox;
    float base = 0;
    bool rotateLeft = (this->motition->getAlpha() > M_PI/2 && this->motition->getAlpha() < M_PI)? true:false;
    if(str == "SAN"){
        Oy = Vec2(0, -1);
        Ox = (rotateLeft)?Vec2(1, 0):Vec2(-1, 0);
    }
    else{
        Oy = (rotateLeft)?Vec2(-1, 0):Vec2(1, 0);
        Ox = (rotateLeft)?Vec2(0, -1):Vec2(0, 1);
        base = 3*M_PI/2;
    }
    if(rotateLeft && this->can_touch){
        if(this->GetX() < 20) LEFT++;
        else RIGHT++;
        this->can_touch = false;
    }
    else if(!rotateLeft && this->can_touch){
        if(this->GetX() > 950) RIGHT++;
        else LEFT++;
        this->can_touch = false;
    }
    this->isdead++;
    // cout<<"hahahah"<<endl;
    Vec2 a = this->motition->direction_vector(get<0>(this->position));
    Vec2 a2 = a - Oy.normalize() * (2.0f * a.dot(Oy.normalize()));
    float costheta = sqrt(a2.dot(Ox)*a2.dot(Ox)/a2.dodaibinh()*Ox.dodaibinh());
    // cout<<"cosx "<<costheta<<endl;
    float theta = (rotateLeft)?M_PI - acos(costheta) : acos(costheta);
    // cout<<"theta "<<theta<<endl;
    if(str == "NET") theta = (rotateLeft)?base + M_PI - theta : base - theta;
    if(base == 0){
        if(theta > M_PI/2 && !rotateLeft) theta = 0;
        else if(theta < M_PI/2 && rotateLeft) theta = M_PI;
    }
    // cout<<"theta "<<theta<<endl;
    float v0 = this->motition->getV0() * 0.75;
    delete this->motition;
    this->motition = new MotionEquation(theta, v0, get<0>(this->position), get<1>(this->position));
    this->x_dubao = this->motition->SolveEquation();
}

void Ball::render(){
    // cout<<this->x_dubao<<"   "<<this->y_dubao<<endl;
    // if(this->create_new_motition) cout<<"New Motition"<<endl;
    this->dstRect.x = get<0>(this->position);
    this->dstRect.y = get<1>(this->position);
    SDL_FRect srcR = {105, 5, 15, 15};
    SDL_FRect dstR = {float(get<0>(position)), float(get<1>(position)), 15*2, 15*2};
    queue <tuple<int, int>> tmp = this->queue_pos;
    for (int i=0;!tmp.empty(); i = (i+1)%16) {
        tuple<int, int> posi = tmp.front();
        srcR.x = 105 - (int(i/2)%8)*15;
        dstR = {float(get<0>(posi)), float(get<1>(posi)), 15*2, 15*2};
        tmp.pop();
        SDL_RenderTexture(this->gRenderer, this->duanh, &srcR, &dstR);
    }
    SDL_RenderTexture(this->gRenderer, this->surface, &srcRect, &dstRect);
    // this->print();
    // SDL_RenderTextureRotated(this->gRenderer, this->surface, &srcRect, &dstRect,0,NULL,SDL_FLIP_HORIZONTAL);
    // SDL_BlitSurfaceScaled(this->surface, &this->srcRect, screenSurface, &this->dstRect, SDL_SCALEMODE_LINEAR);
}
void Ball::renderBallEffects(){
    // SDL_RenderTextureRotated(this->gRenderer, this->dubao, new SDL_FRect{0,0,2048,2048}, new SDL_FRect{this->x_dubao-15,this->y_dubao+20,60,5.66*4},-15,NULL,SDL_FlipMode::SDL_FLIP_NONE);
    // cout<<"Du bao: "<<this->x_dubao<<endl;
    SDL_RenderTexture(this->gRenderer, this->dubao, new SDL_FRect{0,0,281,106}, new SDL_FRect{this->x_dubao-15,this->y_dubao+20,60,5.66*4});
}

void Ball::checkCollision(Character* character){
    if(!this->can_touch) return;
    CharCollisionBall ele = character->checkCollision(this->dstRect);
    if(ele.is_collision){
        delete this->motition;
        this->create_new_motition = true;
        this->char_create_motition = character->GetName();
        float y0 = get<1>(this->position);
        if(ele.action == PlayerAction::Pass){
            this->motition = new MotionEquation(ele.alpha, ele.v0, get<0>(this->position), y0);
            this->x_dubao = this->motition->SolveEquation();
        }    
        else{
            // cout<<"SPIKE"<<endl;
            Vec2 a = direction_vector_A_to_B(Vec2(float(get<0>(position)), float(get<1>(position))), Vec2(500,470));
            this->motition = new MotionEquation(ele.alpha, ele.v0,get<0>(position), y0,a);
            this->x_dubao = this->motition->SolveEquation();
        }
        // cout<< this->motition->print()<<endl;
    }
    else{
        if(this->create_new_motition && this->char_create_motition == character->GetName()){
            this->create_new_motition = false;
            this->char_create_motition = "";
        }
    }
}



SDL_Texture* loadTexture( std::string path, SDL_Renderer* gRenderer)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		SDL_Log( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError() );
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			SDL_Log( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_DestroySurface( loadedSurface );
	}

	return newTexture;
}