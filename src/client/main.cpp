//
//  gui.cpp
//  bluewin_gui
//
//  Created by Rasmus Jansson on 05/05/15.
//  Copyright (c) 2015 Rasmus Jansson. All rights reserved.
//
#include "main.h"

#define MAXMESSAGES 100
#define REFRESHTIME 200
bool writeText = false;
int field = 0;
int nrMessages=0;

//Render text flag
bool renderText = false;

//Main loop flag
bool quit = false;

typedef struct screen{
    int w,h;
}Screen;

typedef struct button{
    int x,y,w,h;
}Button;

Screen windowSize = {windowSize.w=400 , windowSize.h=800};
user_s clientUsr;
Message_s msg;

//std::string outputMessageText = "";
//std::string outputMessageOtherText = " ";
std::string otherUser = "";

//Login Button constants
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 73;
const int TOTAL_BUTTONS = 1;

//Room button constants
const int ROOM_BUTTON_WIDTH = 400;
const int ROOM_BUTTON_HEIGHT = 75;
const int ROOM_BUTTON_TOTAL = 100;

enum LButtonSprite{
    BUTTON_SPRITE_MOUSE_DEFAULT = 0,
    BUTTON_SPRITE_MOUSE_PRESS = 1,
    BUTTON_SPRITE_TOTAL = 2
};

//Texture wrapper class
class LTexture{
public:
    //Initialize variables
    LTexture();

    //Deallocate memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
    //Creates image from font string
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor ,TTF_Font* gFont);
#endif

    //Deallocates texture
    void free();

    //Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders texture at given point
    void render(int x,int y,SDL_Rect* clip = NULL, double angle = 0.0,SDL_Point* center = NULL,SDL_RendererFlip flip = SDL_FLIP_NONE);

    //Gets image dimensions
    int getWidth();
    int getHeight();

private:
    //The actual hardware texture
    SDL_Texture* mTexture;

    //Image dimensions
    int mWidth;
    int mHeight;
};

//The mouse button
class LButton{
public:
    //initialize internal varibles
    LButton();
    //Set top left position
    void setPosition(int x,int y);
    //Handles mouse event
    void handleEvent(json_t* globalRoomArr,bool *loginCheck,bool* createCheck, TCPsocket *sd, SDL_Event* e, int* screenShow, Button* button, int i,std::string* inputUsernameText,std::string* inputPasswordText,std::string* inputRetypePasswordText,std::string* outputPasswordText,std::string* outputRetypePasswordText,std::string* newRoomNameText,json_t *masterobj);
    //Shows button sprite
    void render(int* screenShow, int* element);

private:
    //Top left position
    SDL_Point mPosition;
    //Currently used global sprite
    LButtonSprite mCurrentSprite;
    //If button has focus
    bool focusText;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed image
SDL_Surface* gCurrentSurface = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

//Globally used font
TTF_Font* gFont = NULL;
TTF_Font* gDefaultFont = NULL;
TTF_Font* gLargeBoldFont = NULL;
TTF_Font* gLargeFont = NULL;

//Rendered texture
LTexture gTextTexture;

//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;
LTexture gMainTexture;
LTexture gCreateAccountTexture;
LTexture gChattroomTexture;
LTexture gUsernameTextTexture;
LTexture gPasswordTextTexture;
LTexture gRetypePasswordTexture;
LTexture gMessageTextTexture;
LTexture gCreateRoomNameTextTexture;
LTexture gUserInRoomTexture;

//Login button sprites
SDL_Rect gLoginSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gLoginButtonSpriteSheetTexture;

SDL_Rect gRoomSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gRoomButtonSpriteSheetTexture;

SDL_Rect gLogoutSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gLogoutButtonSpriteSheetTexture;

SDL_Rect gCreateRoomSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gCreateRoomButtonSpriteSheetTexture;

//Buttons objects
LButton gLoginButtons[TOTAL_BUTTONS];
LButton gLogoutButton[TOTAL_BUTTONS];
LButton gRoomButtons[ROOM_BUTTON_TOTAL];
LButton gFieldButtons[2];
LButton gMessageFieldButton[1];
LButton gCreateRoomButton[1];
LButton gCreateRoomNameFieldButton[1];

LTexture::LTexture(){
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture(){
    //Deallocate
    free();
}

bool LTexture::loadFromFile( std::string path ){
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ){
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }else{
        //Color key image
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format,0,255,0 ));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL ){
            printf("Unable to create texture from %s! SDL Error: %s\n",path.c_str(),SDL_GetError());
        }else{
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor , TTF_Font* gFont){
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont,textureText.c_str(),textColor);
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }

    //Return success
    return mTexture != NULL;
}
#endif

void LTexture::free(){
    //Free texture if it exists
    if( mTexture != NULL ){
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue ){
    //Modulate texture rgb
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending ){
    //Set blending function
    SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha ){
    //Modulate texture alpha
    SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip ){
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set clip rendering dimensions
    if( clip != NULL ){
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth(){
    return mWidth;
}

int LTexture::getHeight(){
    return mHeight;
}

LButton::LButton(){
    mPosition.x = 0;
    mPosition.y = 0;

    mCurrentSprite = BUTTON_SPRITE_MOUSE_DEFAULT;
}

void LButton::setPosition(int x, int y){
    mPosition.x = x;
    mPosition.y = y;
}
std::string getInfoJsonRoom(json_t *roomArr,int i){
    //Försöka få användarnamnet
    std::string info=" ";
    const char* hej;
    const json_t* stuffInfo;
    json_t* stuff;
    int size;
    size=json_array_size(roomArr);
    //printf("%d\n",size);
    stuff=json_array_get(roomArr, i);
	hej = json_string_value(stuff);
    //printf("kan vara här\n");
    //printf("%s\n",hej);
    if (hej!=NULL) {
        std::string tmp(hej);
        info=tmp;
        //printf("Username: %s\n",info.c_str());
    }else{
        info=" ";
    }
    //printf("kan vara här\n");
    return info;
}

void sendLogin(int* screenShow,std::string* inputUsernameText,std::string* inputPasswordText,json_t* masterobj,bool* loginCheck,TCPsocket* sd){
    int c=0;
    if (*screenShow==0) {
        send_login(masterobj,inputUsernameText,inputPasswordText,sd);
    }
    while(!*loginCheck && c<20){
        system("sleep(1)");
        c++;
        printf("%d\n", c);
        if(*loginCheck){
            break;
        }
    }
    if (*loginCheck) {
		collect_rooms(masterobj, sd);
        *screenShow=2;
        clientUsr.username=(char*)inputUsernameText->c_str();
        clientUsr.password=(char*)inputPasswordText->c_str();
		clientUsr.room = "default";
        //loginCheck=false;
    }
}

void createUser(int* screenShow,std::string* inputUsernameText,std::string* inputPasswordText,json_t* masterobj,bool* createCheck,TCPsocket* sd){
    int c=0;
    user_s usr;
    usr.username=(char*)inputUsernameText->c_str();
    usr.password=(char*)inputPasswordText->c_str();
    if (*screenShow==1) {
        //Undefined symbols
        add_user(masterobj,&usr,sd);
    }
    while(!*createCheck && c<20){
        system("sleep(1)");
        c++;
        printf("%d\n", c);
        if(*createCheck){
            break;
        }
    }
    if (*createCheck) {
        *screenShow=0;
    }
}

void LButton::handleEvent(json_t *globalRoomArr, bool *loginCheck,bool* createCheck, TCPsocket *sd, SDL_Event* e,int* screenShow, Button* button,int selected,std::string* inputUsernameText,std::string* inputPasswordText,std::string* inputRetypePasswordText,std::string* outputPasswordText,std::string* outputRetypePasswordText,std::string* newRoomNameText,json_t *masterobj){
    //if mouse event happend
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP ) {
        //Get mouse position
        int x,y;
        SDL_GetMouseState(&x, &y);
        //Check if mouse is in button
        bool inside = true;
        //Mouse is left of the button
        if (x<mPosition.x) {
            inside = false;
        }else if (x > mPosition.x + button->w){
            //Mouse is right of the button
            inside = false;
        }else if (y<mPosition.y){
            //Mouse above the button
            inside = false;
        }else if (y>mPosition.y + button->h){
            //Mouse below the button
            inside = false;
        }
        //Mouse is outside button
        if (!inside) {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_DEFAULT;
            focusText = false;
        }else{
            //Mouse is inside button
            //set mouse over sprite
            switch (e->type) {
                case SDL_MOUSEBUTTONDOWN:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_PRESS;
                    break;

                case SDL_MOUSEBUTTONUP:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_DEFAULT;
                    if (!writeText) {
                        focusText=true;
                        writeText=true;
                    }
                    if (*screenShow==0) {
                        switch (selected) {
                            case 0://Login button
                                sendLogin(screenShow, inputUsernameText, inputPasswordText, masterobj,loginCheck,sd);
                                break;

                            case 1://Username text field
                                field=0;
                                break;

                            case 2://Password text field
                                field=1;
                                break;

                            case 3://Lost password
                                printf("Lost password button\n");
                                break;

                            case 4://Create account
                                printf("Create account button\n");
                                *inputUsernameText="";
                                *inputPasswordText="";
                                *inputRetypePasswordText="";
                                *outputRetypePasswordText="";
                                *outputPasswordText="";
                                *screenShow=1;
                                break;

                            default:
                                break;
                        }
                    }else if(*screenShow==1){
                        switch (selected) {
                            case 0://Create account button
                                printf("Create account button\n");
                                createUser(screenShow, inputUsernameText, inputPasswordText, masterobj,createCheck, sd);
                                break;
                                
                            case 1://Back button
                                printf("Back button\n");
                                *inputUsernameText="";
                                *inputPasswordText="";
                                *inputRetypePasswordText="";
                                *outputRetypePasswordText="";
                                *outputPasswordText="";
                                *screenShow=0;
                                break;
                                
                            case 2://Username text field
                                printf("Username text field\n");
                                field=0;
                                break;
                            
                            case 3://Password text field
                                printf("Password text field\n");
                                field=1;
                                break;
                            
                            case 4://Re-type password text field
                                printf("Re-type password text field\n");
                                field=3;
                                break;
                                
                            default:
                                break;
                        }
                    }else if (*screenShow==2){
                        switch (selected) {
                            case 0://Logout button
                                *screenShow=0;
                                *loginCheck=false;
								logout(masterobj, sd);
                                printf("Logout button\n");
                                break;
                                
                            case 1://Create room button
                                printf("Create room button\n");
                                //char* room = (char*)newRoomNameText->c_str();
                                add_room(masterobj,(char*)newRoomNameText->c_str(), sd);
                                *newRoomNameText=" ";
                                break;
                                
                            case 2://Name new room button
                                printf("Name of new room button\n");
                                field=4;
                                break;
                                
                            case 3://Message text field
                                field=2;
								collect_rooms(masterobj, sd);
                                printf("Message text field button\n");
                                break;
                            default:
                                printf("Room %d button\n", selected -4);
								switch_room(masterobj,(char *)getInfoJsonRoom(globalRoomArr, selected-4).c_str(), sd);
								clientUsr.room=getInfoJsonRoom(globalRoomArr,selected-4);
								break;
                        }
                    }

                break;
            }
        }
    }
}

void LButton::render(int* screenShow,int *element){
    if (*screenShow == 0) {
        //show current login button sprite
        gLoginButtonSpriteSheetTexture.render(mPosition.x, mPosition.y,&gLoginSpriteClips[mCurrentSprite]);
    }else if (*screenShow == 1){
        if (*element==0) {
            //show current login button sprite
            gLoginButtonSpriteSheetTexture.render(mPosition.x, mPosition.y,&gLoginSpriteClips[mCurrentSprite]);
        }else if (*element==1){
            //Show current logoff button sprite
            gLogoutButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gLogoutSpriteClips[mCurrentSprite]);
        }
    }else if (*screenShow == 2){
        if (*element==0) {
            //Show current logoff button sprite
            gLogoutButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gLogoutSpriteClips[mCurrentSprite]);
        }else if (*element==1){
            //Show current create room button sprite
            gCreateRoomButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gCreateRoomSpriteClips[mCurrentSprite]);
        }else if (*element==2){
            //Show current room button sprite
            gRoomButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gRoomSpriteClips[mCurrentSprite]);
        }
    }
}

void getText(std::string text, TTF_Font* gFont){
    //Get text
    SDL_Color textColor = {0,0,0};
    if (!gTextTexture.loadFromRenderedText(text, textColor,gFont)) {
        printf("Failed to render text texture!\n");
    }
}

void getPromptText(std::string text, TTF_Font* gFont, int select){
    if (select==0) {
        //Get prompt text
        SDL_Color textColor = {0,0,0};
        if (!gUsernameTextTexture.loadFromRenderedText(text, textColor, gFont)) {
            printf("Failed to render Username text texture!\n");
        }
    }else if (select==1){
        //Get prompt text
        SDL_Color textColor = {0,0,0};
        if (!gPasswordTextTexture.loadFromRenderedText(text, textColor, gFont)) {
            printf("Failed to render Password text texture!\n");
        }
    }else if (select==2){
        //Get prompt text
        SDL_Color textColor = {0,0,0};
        if (!gMessageTextTexture.loadFromRenderedText(text, textColor, gFont)) {
            printf("Failed to render Message text texture!\n");
        }
    }else if (select==3){
        //Get prompt text
        SDL_Color textColor = {0,0,0};
        if (!gRetypePasswordTexture.loadFromRenderedText(text, textColor, gFont)) {
            printf("Failed to render retype Password text texture!\n");
        }
    }else if (select==4){
        //Get prompt text
        SDL_Color textColor = {0,0,0};
        if (!gCreateRoomNameTextTexture.loadFromRenderedText(text, textColor, gFont)) {
            printf("Failed to render Create room name text texture!\n");
        }
    }
}

void getTextString(std::string* inputText, SDL_Event e, SDL_Color textColor,std::string* password){
    //Render text flag
    //bool renderText = false;

    //Special key input
    if (e.type == SDL_KEYDOWN){
        //Handle backspace
        if (e.key.keysym.sym == SDLK_BACKSPACE && inputText->length()>0) {
            //Lop off character
            if(field==0 || field==2 || field==4){
                inputText->pop_back();
            }else if (field==1 || field==3) {
                inputText->pop_back();
                password->pop_back();
                printf("%s\n",password->c_str());
            }
            printf("%s\n",inputText->c_str());
            renderText = true;

        }//Handle copy
        else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL){
            if (field==0 || field==2 || field==4) {
                SDL_SetClipboardText(inputText->c_str());
            }else if (field==1 || field==3) {
                SDL_SetClipboardText(inputText->c_str());
                SDL_SetClipboardText(password->c_str());
            }
        }//Handle paste
        else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL){
            if (field==0 || field==2 || field==4) {
                *inputText = SDL_GetClipboardText();
            }else if (field==1 || field==3) {
                *inputText = SDL_GetClipboardText();
                *password = SDL_GetClipboardText();
            }
            renderText = true;
        }
    }//Special text input event
    else if (e.type == SDL_TEXTINPUT){
        //Not copy or pasting
        if (!((e.text.text[0]=='c' || e.text.text[0]=='C') && (e.text.text[0]=='v' || e.text.text[0]=='V') && SDL_GetModState() & KMOD_CTRL) ) {
            //Append character
            if (field==0 || field==2 || field==4) {
                *inputText += e.text.text;
            }else if (field==1 || field==3) {
                *password += "*";
                *inputText += e.text.text;
                printf("%s\n",password->c_str());
            }
            printf("%s\n",inputText->c_str());
            renderText = true;
        }
    }

    //Rerender text if needed
    if (renderText) {
        //Text is not empty
        if (*inputText !="" || *password !="") {
            if (field==0) {
                //Render new text, username
                inputText->erase(std::remove(inputText->begin(), inputText->end(), ' '), inputText->end());
                gUsernameTextTexture.loadFromRenderedText(inputText->c_str(), textColor, gDefaultFont);
            }else if(field==1){
                //Render new text, passw0rd
                password->erase(std::remove(password->begin(), password->end(), ' '), password->end());
                gPasswordTextTexture.loadFromRenderedText(password->c_str(), textColor, gDefaultFont);
            }else if (field==2){
                //Render new text, message
                /*if (strcmp(" ",inputText[0].c_str())==0) {
                    inputText->erase(inputText->begin());
                }*/
                gMessageTextTexture.loadFromRenderedText(inputText->c_str(), textColor, gDefaultFont);
            }else if (field==3){
                gRetypePasswordTexture.loadFromRenderedText(password->c_str(), textColor, gDefaultFont);
            }else if (field==4){
                gCreateRoomNameTextTexture.loadFromRenderedText(inputText->c_str(), textColor, gDefaultFont);
            }
        }//Text is empty
        else{
            if (field==0) {
                //Render space texture, username
                gUsernameTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
            }else if(field==1){
                //Render space texture, password
                gPasswordTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
            }else if (field==2){
                //Render space texture, message
                gMessageTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
            }else if (field==3){
                gRetypePasswordTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
            }else if (field==4){
                gCreateRoomNameTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
            }
        }
    }
}

bool init(Screen windowSize){
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }else{
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            printf("Warning: Linear texture filtering not enabled");
        }

        //Create window
        gWindow = SDL_CreateWindow( "BlueWin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowSize.w, windowSize.h, SDL_WINDOW_RESIZABLE );
        if( gWindow == NULL ){
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }else{
            //Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL) {
                printf("Renderer could not be created! SDL Error: %s\n",SDL_GetError());
                success = false;
            }else{
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags )) {
                    printf("SDL_images could not initialize! SDL_image error %s\n",IMG_GetError());
                    success = false;
                }

                //Initialize SDL_ttf
                if (TTF_Init() == -1) {
                    printf("SDL_rrf could not initialize! SDL_ttf Error: %s\n",TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia(){
    //Loading success flag
    bool success = true;

    //Get all the images that should be on login screen
    //Load Login texture
    if (!gBackgroundTexture.loadFromFile("bluewinimg/login.png")) {
        printf("Failed to load login texture image!\n");
        success = false;
    }

    //Load main texture
    if (!gMainTexture.loadFromFile("bluewinimg/main.png")) {
        printf("Failed to load main texture\n");
        success = false;
    }

    //Load Login sprites
    if (!gLoginButtonSpriteSheetTexture.loadFromFile("bluewinimg/loginbutton.png")) {
        printf("Failed to load login button sprite texture\n");
        success = false;
    }else{
        //Set sprites
        for (int i = 0; i < BUTTON_SPRITE_TOTAL; i++) {
            gLoginSpriteClips[i].x=0;
            gLoginSpriteClips[i].y=i*73;
            gLoginSpriteClips[i].w=BUTTON_WIDTH;
            gLoginSpriteClips[i].h=BUTTON_HEIGHT;
        }
    }
    
    //Get all the images that should be on the create account screen
    //Load screen texture
    if (!gCreateAccountTexture.loadFromFile("bluewinimg/createaccount.png")) {
        printf("Failed to load create account texture\n");
        success=false;
    }
    
    //Get all the images that should be on main screen
    //Load rooms texture
    if (!gFooTexture.loadFromFile("bluewinimg/main.png")) {
        printf("Failed to load main texture\n");
        success = false;
    }

    if (!gRoomButtonSpriteSheetTexture.loadFromFile("bluewinimg/roombutton.png")) {
        printf("Failed to load rooms button sprite texture\n");
        success = false;
    }else{
        //Set sprites
        for (int i = 0; i < BUTTON_SPRITE_TOTAL; i++) {
            gRoomSpriteClips[i].x=0;
            gRoomSpriteClips[i].y=i*75;
            gRoomSpriteClips[i].w=ROOM_BUTTON_WIDTH;
            gRoomSpriteClips[i].h=ROOM_BUTTON_HEIGHT;
        }
    }

    if (!gLogoutButtonSpriteSheetTexture.loadFromFile("bluewinimg/logout.png")) {
        printf("Failed to load Logout button sprite texture\n");
        success=false;
    }else{
        //Set sprites
        for (int i = 0; i < BUTTON_SPRITE_TOTAL; i++) {
            gLogoutSpriteClips[i].x=0;
            gLogoutSpriteClips[i].y=i*50;
            gLogoutSpriteClips[i].w=150;
            gLogoutSpriteClips[i].h=50;
        }
    }
    
    if (!gCreateRoomButtonSpriteSheetTexture.loadFromFile("bluewinimg/createroombutton.png")) {
        printf("Failed to load create room button sprite texture\n");
        success=false;
    }else{
        //Set sprites
        for (int i = 0; i < BUTTON_SPRITE_TOTAL; i++) {
            gCreateRoomSpriteClips[i].x=0;
            gCreateRoomSpriteClips[i].y=i*50;
            gCreateRoomSpriteClips[i].w=100;
            gCreateRoomSpriteClips[i].h=50;
        }
    }
    
    //Get all the images that should be on the chatt screen
    if (!gChattroomTexture.loadFromFile("bluewinimg/chatroom.png")) {
        printf("Failed to load chatroom texture\n");
        success=false;
    }

    //Open the font
    gDefaultFont = TTF_OpenFont("fonts/quicksand/quicksand-regular.otf", 20);
    if (gDefaultFont == NULL) {
        printf("Failed to load quicksand default font! SDL_ttf Error: %s\n",TTF_GetError());
        success = false;
    }
    gLargeBoldFont = TTF_OpenFont("fonts/quicksand/quicksand-bold.otf", 34);
    if (gDefaultFont == NULL) {
        printf("Failed to load quicksand large bold font! SDL_ttf Error: %s\n",TTF_GetError());
        success = false;
    }
    gLargeFont = TTF_OpenFont("fonts/quicksand/quicksand-regular.otf", 34);
    if (gDefaultFont == NULL) {
        printf("Failed to load quicksand large font! SDL_ttf Error: %s\n",TTF_GetError());
        success = false;
    }

    return success;
}

void close(){
    //Free loaded image
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;
    gLoginButtonSpriteSheetTexture.free();
    gRoomButtonSpriteSheetTexture.free();
    gCreateRoomButtonSpriteSheetTexture.free();
    gFooTexture.free();
    gBackgroundTexture.free();
    gMainTexture.free();
    gChattroomTexture.free();
    gTextTexture.free();
    gUsernameTextTexture.free();
    gPasswordTextTexture.free();
    gMessageTextTexture.free();
    gCreateRoomNameTextTexture.free();
    gUserInRoomTexture.free();

    //Free global font
    TTF_CloseFont(gDefaultFont);
    TTF_CloseFont(gLargeBoldFont);
    TTF_CloseFont(gLargeFont);
    gDefaultFont = NULL;
    gLargeBoldFont = NULL;
    gLargeFont = NULL;

    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    gRenderer=NULL;

    //Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* loadTexture(std::string path){
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specfied path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n",path.c_str(),IMG_GetError());
    }else{
        //Create texture from surface pixels
        newTexture =SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n",path.c_str(), SDL_GetError());
        }
        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}
void append_messageArr(json_t* messageArr, user_s * usr, std::string *text, SDL_mutex *mesageArrMutex){
	json_t * buildingblock, *current;
	buildingblock = json_object();
	current = json_string(usr->username);
	json_object_set_new(buildingblock, "username" ,current);
	current = json_string(text->c_str());
	json_object_set_new(buildingblock, "message" ,current);
	SDL_LockMutex(mesageArrMutex);
	json_array_append_new(messageArr, buildingblock);
	SDL_UnlockMutex(mesageArrMutex);
}

void eventHandler(json_t * globalRoomArr, json_t * messageArr ,SDL_mutex *mesageArrMutex, bool *loginCheck,bool* createCheck, TCPsocket *sd, int* screenShow,int totalButtons, int totalFields, Button fieldButton, Button logoutButton,Button buttonTypeWide,Button buttonTypeSmall,Button messageButton,Button createRoomButton,Button createRoomFieldButton,std::string* inputUsernameText,std::string* inputPasswordText,std::string* inputRetypePasswordText,std::string* outputPasswordText,std::string* outputRetypePasswordText,std::string* inputMessageText,std::string* outputMessageText,std::string* newRoomNameText ,SDL_Event e, json_t *masterobj){
    //Event handler
    //SDL_Event e;
    int totalElements = totalButtons + totalFields;

    //Set text color as black
    SDL_Color textColor = {0,0,0,0xff};

    //Handle events on queue
    //while( SDL_PollEvent( &e ) != 0 ){
        //User requests quit
        if( e.type == SDL_QUIT ){
            quit = true;
        }else if (*screenShow==0) {
            totalElements=1+totalButtons+totalFields;
            //Generate all elements in login screen
            for (int i = 0; i< totalElements; i++) {
                if (i<totalButtons) {
                    gLoginButtons[i].handleEvent(globalRoomArr, loginCheck,createCheck, sd, &e,screenShow,&buttonTypeSmall,i,inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText,masterobj);
                }else if(i<totalButtons+totalFields){
                    gFieldButtons[i-totalButtons].handleEvent(globalRoomArr, loginCheck,createCheck, sd, &e, screenShow, &fieldButton,i,inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText,masterobj);
                }
            }
            //Get user input text
            if (writeText) {
                if (field==0) {
                    getTextString(inputUsernameText,e,textColor,outputPasswordText);
                }else if (field==1){
                    getTextString(inputPasswordText, e, textColor,outputPasswordText);
                }
            }
            if( e.type == SDL_KEYDOWN ){
                switch( e.key.keysym.sym ){
                    case SDLK_TAB:
                        if (field==0) {
                            field=1;
                        }else if (field==1){
                            field=0;
                        }
                        break;

                    case SDLK_RETURN:
                        sendLogin(screenShow, inputUsernameText, inputPasswordText, masterobj,loginCheck,sd);
                        break;

                    default:
                        break;
                }
            }
        }else if(*screenShow == 1){
            totalElements=1+totalButtons+totalFields;
            //Generate all elements in login screen
            for (int i = 0; i< totalElements; i++) {
                if (i<1) {
                    gLoginButtons[i].handleEvent(globalRoomArr,loginCheck,createCheck, sd, &e,screenShow,&buttonTypeSmall,i,inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText,masterobj);
                }else if (i<totalButtons){
                    gLogoutButton[i].handleEvent(globalRoomArr,loginCheck,createCheck, sd, &e, screenShow, &logoutButton, i, inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText, masterobj);
                }else if(i<totalButtons+totalFields){
                    gFieldButtons[i-totalButtons].handleEvent(globalRoomArr,loginCheck,createCheck, sd, &e, screenShow, &fieldButton,i,inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText,masterobj);
                }
            }
            
            if (writeText) {
                if (field==0) {
                    getTextString(inputUsernameText,e,textColor,outputPasswordText);
                }else if (field==1){
                    getTextString(inputPasswordText, e, textColor,outputPasswordText);
                }else if (field==3){
                    getTextString(inputRetypePasswordText, e, textColor, outputRetypePasswordText);
                }
            }
            
            if( e.type == SDL_KEYDOWN ){
                switch( e.key.keysym.sym ){
                    case SDLK_TAB:
                        if (field==0) {
                            field=1;
                        }else if (field==1){
                            field=3;
                        }else if (field==3){
                            field=0;
                        }
                        break;
                        
                    case SDLK_RETURN:
                        createUser(screenShow, inputUsernameText, inputPasswordText, masterobj, createCheck, sd);
                        break;
                        
                    default:
                        break;
                }
            }
        }else if(*screenShow == 2){
            totalElements=1+totalButtons+totalFields;
            //Generate all elements in main screen
            for (int i =0; i<totalElements; i++) {
                if (i<1) {
                    gLogoutButton[i].handleEvent(globalRoomArr,loginCheck,createCheck, sd, &e, screenShow, &logoutButton, i,inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText,masterobj);
                }else if(i<2){
                    gCreateRoomButton[i-1].handleEvent(globalRoomArr, loginCheck, createCheck, sd, &e, screenShow,&createRoomButton , i, inputUsernameText, inputPasswordText, inputRetypePasswordText, outputPasswordText, outputRetypePasswordText,newRoomNameText, masterobj);
                }else if (i<3){
                    gCreateRoomNameFieldButton[i-2].handleEvent(globalRoomArr, loginCheck, createCheck, sd, &e, screenShow, &createRoomFieldButton, i, inputUsernameText, inputPasswordText, inputRetypePasswordText, outputPasswordText, outputRetypePasswordText,newRoomNameText, masterobj);
                }else if(i<2+totalFields){
                    gMessageFieldButton[i-3].handleEvent(globalRoomArr,loginCheck,createCheck,sd,&e, screenShow, &messageButton, i, inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText, masterobj);
                }else if (i<totalButtons){
                    gRoomButtons[i-2-totalFields].handleEvent(globalRoomArr,loginCheck,createCheck, sd, &e, screenShow,&buttonTypeWide,i,inputUsernameText,inputPasswordText, inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,newRoomNameText,masterobj);
                }
            }
            if (writeText) {
                if (field==2) {
                    getTextString(inputMessageText,e,textColor,outputPasswordText);
                }else if (field==4){
                    getTextString(newRoomNameText, e, textColor, outputPasswordText);
                }
            }
            
            if( e.type == SDL_KEYDOWN ){
                switch( e.key.keysym.sym ){
                    case SDLK_RETURN:
                        if (field==2) {
                            if (*inputMessageText=="" || *inputMessageText==" ") {
                                printf("Cant send empty message\n");
                            }else{
                                printf("You sent a message!\n");
                                *outputMessageText=*inputMessageText;
                                *inputMessageText=" ";
                                printf("%s\n",outputMessageText->c_str());
                                append_messageArr(messageArr, &clientUsr, outputMessageText, mesageArrMutex);
                                msg.message=(char*)outputMessageText->c_str();
                                msg.room=(char *)clientUsr.room.c_str();
                                write_message(masterobj, &clientUsr, msg, sd);
                            }
                        }else if (field==4){
                            printf("This aint working yet\n");
                        }
                        break;

                    default:
                        break;
                }
            }
        }
}


std::string getInfoJson(json_t *messageArr,char* cmd,int i){
    //Försöka få användarnamnet
    std::string info=" ";
    const char* hej;
    const json_t* stuffInfo;
    json_t* stuff;
    int size;
    size=json_array_size(messageArr);
    //printf("%d\n",size);
    stuff=json_array_get(messageArr, i);
    stuffInfo=json_object_get(stuff, cmd);
    hej=json_string_value(stuffInfo);
    //printf("kan vara här\n");
    //printf("%s\n",hej);
    if (hej!=NULL) {
        std::string tmp(hej);
        info=tmp;
        //printf("Username: %s\n",info.c_str());
    }else{
        info=" ";
    }
    //printf("kan vara här\n");
    return info;
}

void fill_message_arr(std::string messages[],std::string newMessage){
    std::string tmp;
    
    for (int i=MAXMESSAGES; i>0; i--) {
        printf("%d\n",i);
        messages[i]=messages[i-1];
    }
    
    messages[0]=newMessage;
}

void loginScreen( int* totalButtons, int* totalFields,int* screenShow,Screen windowSize,Button loginButton, Button fieldButton,std::string inputUsernameText, std::string inputPasswordText){
    *totalButtons=1;
    *totalFields=4;
    int element = 0;
    SDL_SetWindowSize(gWindow, windowSize.w, windowSize.h);
    
    //Render background texture to screen
    gBackgroundTexture.render(0,0);
    
    //Get login button
    gLoginButtons[0].render(screenShow,&element);
    gLoginButtons[0].setPosition(loginButton.x, loginButton.y);
    getText("Login", gLargeBoldFont);
    gTextTexture.render((windowSize.w-gTextTexture.getWidth())/2, ((loginButton.h - gTextTexture.getHeight())/2)+loginButton.y);

    //Positionate text fields
    gFieldButtons[0].setPosition(fieldButton.x,300);
    gFieldButtons[1].setPosition(fieldButton.x,420);

    if (inputUsernameText=="" || inputUsernameText==" ") {
        inputUsernameText=" ";
        getPromptText("Username", gDefaultFont,0);
        gUsernameTextTexture.render(fieldButton.x+10,320);
    }else{
        //Render text texture
        gUsernameTextTexture.render(fieldButton.x+10, 320);
    }

    if (inputPasswordText=="" || inputPasswordText==" ") {
        inputPasswordText=" ";
        getPromptText("Password", gDefaultFont,1);
        gPasswordTextTexture.render(fieldButton.x+10, 440);
    }else{
        //Render text texture
        gPasswordTextTexture.render(fieldButton.x+10, 440);
    }

    //Render current frame
    getText("BlueWin", gLargeBoldFont);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,100);
    getText("Username",gLargeBoldFont);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,230);
    getText("Password",gLargeBoldFont);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,380);
    getText("Lost password",gDefaultFont);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,(windowSize.h - 30));
    gFieldButtons[2].setPosition((windowSize.w - gTextTexture.getWidth())/2,(windowSize.h - gTextTexture.getHeight()));
    //printf("%d\n",gTextTexture.getHeight());
    getText("Create account",gDefaultFont);
    gFieldButtons[3].setPosition((windowSize.w - gTextTexture.getWidth())/2,(windowSize.h - gTextTexture.getHeight())-60);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,(windowSize.h - 60));
    //gLoginButtons[2].setPosition((windowSize.w - gTextTexture.getWidth())/2,(windowSize.h - gTextTexture.getHeight())-60);
    //printf("%d\n",gTextTexture.getHeight());
}

void createAccountScreen(int* totalButtons, int* totalFields,int* screenShow,Screen windowSize,Button loginButton, Button fieldButton,Button logoutButton,std::string inputUsernameText, std::string inputPasswordText,std::string inputRetypePasswordText){
    *totalButtons=2;
    *totalFields=3;
    int element=0;
    SDL_SetWindowSize(gWindow, windowSize.w, windowSize.h);
    
    //Render background texture
    gCreateAccountTexture.render(0, 0);
    
    //Create create button
    gLoginButtons[0].setPosition((windowSize.w-loginButton.w)/2, 570);
    gLoginButtons[0].render(screenShow,&element);
    getText("Create", gLargeFont);
    gTextTexture.render((windowSize.w-gTextTexture.getWidth())/2, ((loginButton.h - gTextTexture.getHeight())/2)+570);
    
    //Create back button
    element=1;
    gLogoutButton[0].setPosition((windowSize.w-logoutButton.w)-3, 3);
    gLogoutButton[0].render(screenShow, &element);
    getText("Back", gLargeFont);
    gTextTexture.render((windowSize.w - logoutButton.w)+((logoutButton.w - gTextTexture.getWidth())/2)-3, (logoutButton.h - gTextTexture.getHeight())/2);
    
    //Positionate text fields
    gFieldButtons[0].setPosition(fieldButton.x,230);
    gFieldButtons[1].setPosition(fieldButton.x,360);
    gFieldButtons[2].setPosition(fieldButton.x, 490);
    
    //Render text
    getText("Username", gLargeBoldFont);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,230-gTextTexture.getHeight()-5);
    getText("Password", gLargeBoldFont);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,360-gTextTexture.getHeight()-5);
    getText("Re-type password", gLargeBoldFont);
    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,490-gTextTexture.getHeight()-5);
    
    if (inputUsernameText=="" || inputUsernameText==" ") {
        inputUsernameText=" ";
        getPromptText("Username", gDefaultFont,0);
        gUsernameTextTexture.render(fieldButton.x+10,230+20);
    }else{
        //Render text texture
        gUsernameTextTexture.render(fieldButton.x+10, 230+20);
    }
    
    if (inputPasswordText=="" || inputPasswordText==" ") {
        inputPasswordText=" ";
        getPromptText("Password", gDefaultFont,1);
        gPasswordTextTexture.render(fieldButton.x+10,360+20);
    }else{
        //Render text texture
        gPasswordTextTexture.render(fieldButton.x+10, 360+20);
    }
    
    if (inputRetypePasswordText=="" || inputRetypePasswordText==" ") {
        inputRetypePasswordText=" ";
        getPromptText("Re-type password", gDefaultFont,3);
        gRetypePasswordTexture.render(fieldButton.x+10,490+20);
    }else{
        //Render text texture
        gRetypePasswordTexture.render(fieldButton.x+10, 490+20);
    }
    
}

void mainScreen(json_t* globalRoomArr, json_t *globalUsersInRoomArr, json_t *messageArr,int* totalButtons, int* totalFields,int nrRooms,int* screenShow,Screen windowSize,Button logoutButton,Button buttonTypeWide,Button messageButton,Button createRoomButton,Button createRoomFieldButton,std::string* inputMessageText,std::string* outputMessageText ,std::string* outputMessageOtherText,std::string* newRoomNameText){
    int buttX=0,buttY=200,element,space=0,box=0,messageAreaSize=530,userInRoom=4,originSizeWidth=0,originSizeHeight=0;
	nrRooms = json_array_size(globalRoomArr);   
    *totalButtons=1+nrRooms+1+1+1;
    *totalFields=1+1;
    //clientUsr.username="Hejsan";
    char* cmd;
    bool newMessage = false;
    std::string messages[MAXMESSAGES];
    std::string username[MAXMESSAGES];
    std::string tmp;
    SDL_SetWindowSize(gWindow, windowSize.w+640, windowSize.h);
    
    //Everyting on the list side(left)
    //Render rooms texture to screen
    gFooTexture.render(0, 0);

    //Render Username
    std::string user(clientUsr.username);
    getText(user, gDefaultFont);
    gTextTexture.render(10, 10);
    
    //Render and positionate logoff button
    element=0;
    gLogoutButton[0].render(screenShow,&element);
    gLogoutButton[0].setPosition(windowSize.w - logoutButton.w-3,3);
    getText("Logout", gLargeFont);
    gTextTexture.render((windowSize.w - logoutButton.w)+((logoutButton.w - gTextTexture.getWidth())/2)-3, (logoutButton.h - gTextTexture.getHeight())/2);
    
    //Render and positionate create room button
    element=1;
    gCreateRoomButton[0].render(screenShow, &element);
    gCreateRoomButton[0].setPosition((windowSize.w - createRoomButton.w)-20, 140);
    getText("Create", gDefaultFont);
    gTextTexture.render((windowSize.w - createRoomButton.w)+((createRoomButton.w - gTextTexture.getWidth())/2)-20, (createRoomButton.h - gTextTexture.getHeight())/2+140);
    
    //Render and positionate create room text field
    gCreateRoomNameFieldButton[0].setPosition(10, 140);
    if (*newRoomNameText=="" || *newRoomNameText==" ") {
        *newRoomNameText=" ";
        getPromptText("Name of new room", gDefaultFont, 4);
        gCreateRoomNameTextTexture.render(createRoomFieldButton.x+10, createRoomFieldButton.y+10);
    }else{
        gCreateRoomNameTextTexture.render(createRoomFieldButton.x+10, createRoomFieldButton.y+10);
    }
    
    element=2;
    //Render room buttons
    for (int i = 0; i < nrRooms; ++i) {
        gRoomButtons[i].render(screenShow,&element);
        //Positionate room buttons
        gRoomButtons[i].setPosition(0, (buttY+(75*i)));
        //Get text on button
        std::ostringstream stream;
        //stream << "Chat room " << i+1;
        //std::string text = stream.str();
        std::string text = getInfoJsonRoom(globalRoomArr, i);
        getText(text, gLargeFont);
        gTextTexture.render((0+((buttonTypeWide.w - gTextTexture.getWidth())/2)), (buttY+((buttonTypeWide.h - gTextTexture.getHeight())/2)+(buttonTypeWide.h*i)));
    }
    
    //Everything on the chattroom side(right)
    //Render chattroom background
    gChattroomTexture.render(windowSize.w, 0);
    
    //getText(clientUsr.username, gDefaultFont);
    //gTextTexture.render(windowSize.w + 20, 20);
    for (int i = 0; i < userInRoom; i++) {
        std::ostringstream stream;
        if (i==0) {
            stream << "• " << clientUsr.username;
            std::string usernameClient = stream.str();
            getText(usernameClient, gDefaultFont);
            gTextTexture.render(windowSize.w + 20, 20);
            printf("%s\n",usernameClient.c_str());
            originSizeWidth=gTextTexture.getWidth();
            originSizeHeight=gTextTexture.getHeight();
        }else if (i>0){
            stream << "• " << otherUser;
            std::string usernameOther = stream.str();
            getText(usernameOther, gDefaultFont);
            gTextTexture.render((windowSize.w + 20)+(i*(originSizeWidth+50)), 20);
            originSizeWidth+=gTextTexture.getWidth();
        }
    }
    
    gMessageFieldButton[0].setPosition(windowSize.w+90,685);
    if (*inputMessageText=="" || *inputMessageText==" ") {
        *inputMessageText=" ";
        getPromptText("Message", gDefaultFont, 2);
        gMessageTextTexture.render(messageButton.x,messageButton.y);
    }else{
        gMessageTextTexture.render(messageButton.x, messageButton.y);
    }

    //When user send message
    /*if (*outputMessageText=="" || *outputMessageText==" ") {
        //printf("There are no user messages\n");
        *outputMessageText=" ";
    }else{
        getText(user, gDefaultFont);
        gTextTexture.render(windowSize.w+400, (windowSize.h-200)-gUsernameTextTexture.getHeight());
        getText(*outputMessageText, gLargeFont);
        gTextTexture.render(windowSize.w+400, windowSize.h-200);
    }*/

    
    
    nrMessages=json_array_size(messageArr);
    //When user recives message
    box=0;
    if ((*outputMessageOtherText=="" || *outputMessageOtherText==" ")){// || (otherUser=="" || otherUser==" ")) {
        *outputMessageOtherText=" ";
        otherUser=" ";
    }else{
        getText(*outputMessageOtherText, gLargeFont);
        box+=gTextTexture.getHeight();
        getText(otherUser, gDefaultFont);
        box+=gTextTexture.getHeight();
        if ((box+space)<=messageAreaSize) {
            space+=box;
        }
    }
    for (int i = nrMessages-1; i>=0; i--) {
        cmd="message";
        *outputMessageOtherText=getInfoJson(messageArr,cmd,i);
        cmd="username";
        otherUser=getInfoJson(messageArr,cmd,i);
        if(space<=messageAreaSize){
            if (*outputMessageOtherText=="" || *outputMessageOtherText==" ") {
                *outputMessageOtherText=" ";
                otherUser=" ";
            }else{
                if (clientUsr.username==otherUser) {
                    getText(*outputMessageOtherText, gLargeFont);
                    gTextTexture.render(windowSize.w+400, ((windowSize.h-100)-gTextTexture.getHeight())-(space));
                    getText(clientUsr.username, gDefaultFont);
                    gTextTexture.render(windowSize.w+400, ((windowSize.h-100)-gTextTexture.getHeight()+(box+gTextTexture.getHeight())-(box*2))-space);
                }else{
                    getText(*outputMessageOtherText, gLargeFont);
                    gTextTexture.render(windowSize.w+3, ((windowSize.h-100)-gTextTexture.getHeight())-(space));
                    getText(otherUser, gDefaultFont);
                    gTextTexture.render(windowSize.w+3, ((windowSize.h-100)-gTextTexture.getHeight()+(box+gTextTexture.getHeight())-(box*2))-space);
                }
                space+=box;
            }
        }else{
            break;
        }
    }
}

void runingGui(int * refreshCounter, json_t* globalUsersInRoomArr, json_t *globalRoomArr, SDL_mutex *mesageArrMutex, json_t *messageArr,bool* loginCheck,bool* createCheck, TCPsocket* sd,int* screenShow,int* totalButtons, int* totalFields,int nrRooms,Screen windowSize,Button loginButton ,Button fieldButton, Button logoutButton,Button buttonTypeWide,Button buttonTypeSmall,Button messageButton,Button createRoomButton,Button createRoomFieldButton,std::string* inputUsernameText,std::string* inputPasswordText,std::string* inputRetypePasswordText,std::string* outputPasswordText,std::string* outputRetypePasswordText,std::string* inputMessageText,std::string* outputMessageText ,std::string* outputMessageOtherText,std::string* newRoomNameText,SDL_Event* e, json_t *masterobj){
	
    while( SDL_PollEvent( e ) != 0 ){
        eventHandler(globalRoomArr, messageArr, mesageArrMutex, loginCheck,createCheck, sd, screenShow, *totalButtons, *totalFields, fieldButton, logoutButton, buttonTypeWide, buttonTypeSmall,messageButton,createRoomButton,createRoomFieldButton, inputUsernameText, inputPasswordText,inputRetypePasswordText, outputPasswordText,outputRetypePasswordText,inputMessageText,outputMessageText,newRoomNameText,*e,masterobj);
    }
	
    //Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);

    //Render texture to screen
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

    if(*screenShow==0){
        loginScreen(totalButtons, totalFields, screenShow, windowSize, loginButton, fieldButton, *inputUsernameText, *inputPasswordText);
    }else if (*screenShow==1){
        createAccountScreen(totalButtons, totalFields, screenShow, windowSize, loginButton, fieldButton,logoutButton, *inputUsernameText, *inputPasswordText, *inputRetypePasswordText);
    }else{
		(*refreshCounter)++;
		//printf("%d\n", *refreshCounter);
		if(*refreshCounter>REFRESHTIME){
			collect_rooms(masterobj, sd);
			//getusersinroom();
			*refreshCounter=0;
		}
		mainScreen(globalRoomArr, globalUsersInRoomArr, messageArr,totalButtons, totalFields, nrRooms, screenShow, windowSize, logoutButton, buttonTypeWide,messageButton,createRoomButton,createRoomFieldButton,inputMessageText,outputMessageText,outputMessageOtherText,newRoomNameText);
	}
	
    //Update screen
    SDL_RenderPresent(gRenderer);
}

int initGui( bool *createCheck, TCPsocket * sd, bool *loginCheck, json_t * globalUsersInRoomArr, json_t *globalRoomArr, json_t *messageArr, SDL_mutex *messageArrMutex){
    bool test=true;
    //Start up SDL and create window
    if( !init(windowSize) ){
        printf( "Failed to initialize!\n" );
        test=false;
    }else{
        //Load media
        if( !loadMedia() ){
            printf( "Failed to load media!\n" );
            test=false;
        }else{
            //Connect to server
            if (!((*sd)=initClient(createCheck, loginCheck,globalUsersInRoomArr, globalRoomArr, messageArr, messageArrMutex ))) {
                printf("Failed to connect to server\n");
                test=false;
            }
        }
    }
    return test;
}

int main(int argc, char *argv[]){
    //Initialize varibles
	TCPsocket sd;
    int screenShow = 0,totalButtons=0,totalFields=0,nrRooms=1, refreshCounter=0;//,field = 0;
    Screen windowSize;
    windowSize.w=400;
    windowSize.h=800;
    //screenShow=2;

    Button buttonTypeSmall,buttonTypeWide,fieldButton,loginButton,logoutButton,createRoomButton,messageButton,createRoomFieldButton;
    buttonTypeSmall.w=200;
    buttonTypeSmall.h=75;
    buttonTypeWide.w=400;
    buttonTypeWide.h=75;
    fieldButton.w=300;
    fieldButton.h=60;
    fieldButton.x=50;
    loginButton.w=200;
    loginButton.h=73;
    loginButton.x=100;
    loginButton.y=500;
    logoutButton.w=150;
    logoutButton.h=50;
    logoutButton.y=0;
    logoutButton.x=windowSize.w-logoutButton.w;
    createRoomButton.w=100;
    createRoomButton.h=50;
    messageButton.h=80;
    messageButton.w=460;
    messageButton.x=windowSize.w+90;
    messageButton.y=685;
    createRoomFieldButton.w=250;
    createRoomFieldButton.h=50;
    createRoomFieldButton.x=10;
    createRoomFieldButton.y=140;

    json_t *masterobj = json_object();
	json_t *globalUsersInRoomArr =json_array();
	json_t *globalRoomArr=json_array();
	json_t *messageArr=json_array();
	SDL_mutex *messageArrMutex = SDL_CreateMutex();
    bool loginCheck = false;
    bool createCheck = true;
	//set default room at start so that client correctly sends messages
	
    //Start up SDL and create window
    if( !initGui(&createCheck, &sd, &loginCheck, globalUsersInRoomArr, globalRoomArr, messageArr, messageArrMutex) ){
        printf( "Failed to initialize!\n" );
    }else{
        //Main loop flag
        //bool quit = false;
		
        //Event handler
        SDL_Event e;

        //The current input text
        std::string inputUsernameText = "";
        std::string inputPasswordText = "";
        std::string inputRetypePasswordText = "";
        std::string outputPasswordText = "";
        std::string outputRetypePasswordText = "";
        std::string inputMessageText = "";
        std::string outputMessageOtherText = "";
        std::string outputMessageText = "";
        std::string newRoomNameText = "";
        //Enable text input
        SDL_StartTextInput();

        //While application is running
        while( !quit ){
            runingGui(&refreshCounter,globalUsersInRoomArr, globalRoomArr,messageArrMutex,messageArr,&loginCheck,&createCheck, &sd, &screenShow, &totalButtons, &totalFields, nrRooms, windowSize, loginButton, fieldButton, logoutButton, buttonTypeWide, buttonTypeSmall,messageButton,createRoomButton,createRoomFieldButton, &inputUsernameText, &inputPasswordText, &inputRetypePasswordText, &outputPasswordText,&outputRetypePasswordText,&inputMessageText,&outputMessageText,&outputMessageOtherText,&newRoomNameText, &e, masterobj);
        }
        //Disable text input
        SDL_StopTextInput();
    }

    //Free resources and close SDL
    close();

    return 0;
}
