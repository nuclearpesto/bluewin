//
//  gui.cpp
//  bluewin_gui
//
//  Created by Rasmus Jansson on 05/05/15.
//  Copyright (c) 2015 Rasmus Jansson. All rights reserved.
//
#include "main.h"

bool writeText = false;
int field = 0;

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

//Login Button constants
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 75;
const int TOTAL_BUTTONS = 1;

//Room button constants
const int ROOM_BUTTON_WIDTH = 400;
const int ROOM_BUTTON_HEIGHT = 75;
const int ROOM_BUTTON_TOTAL = 1;

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
    void handleEvent(bool *loginCheck, TCPsocket *sd, SDL_Event* e, int* screenShow, Button* button, int i,std::string* inputUsernameText, std::string*
                     inputPasswordText,json_t *masterobj);
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
LTexture gChattroomTexture;
LTexture gUsernameTextTexture;
LTexture gPasswordTextTexture;
LTexture gMessageTextTexture;

//Login button sprites
SDL_Rect gLoginSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gLoginButtonSpriteSheetTexture;

SDL_Rect gRoomSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gRoomButtonSpriteSheetTexture;

SDL_Rect gLogoutSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gLogoutButtonSpriteSheetTexture;

//Buttons objects
LButton gLoginButtons[TOTAL_BUTTONS];
LButton gLogoutButton[TOTAL_BUTTONS];
LButton gRoomButtons[ROOM_BUTTON_TOTAL];
LButton gFieldButtons[2];
LButton gMessageFieldButton[1];

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

void sendLogin(int* screenShow,std::string* inputUsernameText,std::string* inputPasswordText,json_t* masterobj,bool* loginCheck){
    int c=0;
    if (*screenShow==0) {
        send_login(masterobj,inputUsernameText,inputPasswordText);
    }
    while(!loginCheck && c<10 ){
        //Sleep(1);
        c++;
        //printf("%d\n", c);
        if(loginCheck){
            break;
        }
    }
    if (loginCheck) {
        *screenShow=1;
        //loginCheck=false;
    }
}

void LButton::handleEvent(bool *loginCheck, TCPsocket *sd, SDL_Event* e,int* screenShow, Button* button,int selected,std::string* inputUsernameText, std::string* inputPasswordText,json_t *masterobj){
    //if mouse event happend
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP ) {
        //Get mouse position
        int x,y,c=0;
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
                                sendLogin(screenShow, inputUsernameText, inputPasswordText, masterobj,loginCheck);
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
                                break;
                                
                            default:
                                break;
                        }
                    }else if (*screenShow==1){
                        switch (selected) {
                            case 0://Logout button
                                *screenShow=0;
                                *loginCheck=false;
                                printf("Logout button\n");
                                break;
                                
                            case 1://Message text field
                                field=2;
                                printf("Message text field button\n");
                                break;
                                
                            case 2:
                                printf("Room 1 button\n");
                                break;
                                
                            case 3:
                                printf("Room 2 button\n");
                                break;
                                
                            case 4:
                                printf("Room 3 button\n");
                                break;
                                
                            case 5:
                                printf("Room 4 button\n");
                                break;
                                
                            case 6:
                                printf("Room 5 button\n");
                                break;
                                
                            case 7:
                                printf("Room 6 button\n");
                                break;
                                
                            
                            default:
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
            //Show current logoff button sprite
            gLogoutButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gLogoutSpriteClips[mCurrentSprite]);
        }else if (*element==1){
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
            printf("Failed to render Password text texture!\n");
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
            if(field==0 || field==2){
                inputText->pop_back();
            }else if (field==1) {
                inputText->pop_back();
                password->pop_back();
                printf("%s\n",password->c_str());
            }
            printf("%s\n",inputText->c_str());
            renderText = true;
            
        }//Handle copy
        else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL){
            if (field==0 || field==2) {
                SDL_SetClipboardText(inputText->c_str());
            }else if (field==1) {
                SDL_SetClipboardText(inputText->c_str());
                SDL_SetClipboardText(password->c_str());
            }
        }//Handle paste
        else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL){
            if (field==0 || field==2) {
                *inputText = SDL_GetClipboardText();
            }else if (field==1) {
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
            if (field==0 || field==2) {
                *inputText += e.text.text;
            }else if (field==1) {
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
                gMessageTextTexture.loadFromRenderedText(inputText->c_str(), textColor, gDefaultFont);
            }
        }//Text is empty
        else{
            if (field==0) {
                //Render space texture, username
                gUsernameTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
            }else{
                //Render space texture, password
                gPasswordTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
            }else if (field==2){
                //Render space texture, message
                gMessageTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
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
        printf("Failed to load main texture");
        success = false;
    }
    
    //Load Login sprites
    if (!gLoginButtonSpriteSheetTexture.loadFromFile("bluewinimg/loginbutton.png")) {
        printf("Failed to load login button sprite texture");
        success = false;
    }else{
        //Set sprites
        for (int i = 0; i < BUTTON_SPRITE_TOTAL; i++) {
            gLoginSpriteClips[i].x=0;
            gLoginSpriteClips[i].y=i*72;
            gLoginSpriteClips[i].w=BUTTON_WIDTH;
            gLoginSpriteClips[i].h=BUTTON_HEIGHT;
        }
    }
    
    //Get all the images that should be on main screen
    //Load rooms texture
    if (!gFooTexture.loadFromFile("bluewinimg/main.png")) {
        printf("Failed to load main texture");
        success = false;
    }

    if (!gRoomButtonSpriteSheetTexture.loadFromFile("bluewinimg/roombutton.png")) {
        printf("Failed to load rooms button sprite texture");
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
        printf("Failed to load Logout button sprite texture");
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

    //Get all the images that should be on the chatt screen
    if (!gChattroomTexture.loadFromFile("bluewinimg/chatroom.png")) {
        printf("Failed to load chatroom texture");
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
    gFooTexture.free();
    gBackgroundTexture.free();
    gMainTexture.free();
    gChattroomTexture.free();
    gTextTexture.free();
    gUsernameTextTexture.free();
    gPasswordTextTexture.free();
    
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

void eventHandler(bool *loginCheck, TCPsocket *sd, int* screenShow,int totalButtons, int totalFields, Button fieldButton, Button logoutButton,Button buttonTypeWide,Button buttonTypeSmall,Button messageButton,std::string* inputUsernameText,std::string* inputPasswordText,std::string* outputPasswordText,std::string inputMessageText,SDL_Event e, json_t *masterobj){
    //Event handler
    //SDL_Event e;
    int totalElements = totalButtons + totalFields,c=0;
    
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
                    gLoginButtons[i].handleEvent(loginCheck, sd, &e,screenShow,&buttonTypeSmall,i,inputUsernameText,inputPasswordText,masterobj);
                }else if(i<totalButtons+totalFields){
                    gFieldButtons[i-totalButtons].handleEvent(loginCheck, sd, &e, screenShow, &fieldButton,i,inputUsernameText,inputPasswordText,masterobj);
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
                        sendLogin(screenShow, inputUsernameText, inputPasswordText, masterobj,loginCheck);
                        break;
                        
                    default:
                        break;
                }
            }
        }else if(*screenShow == 1){
            totalElements=1+totalButtons;
            //Generate all elements in main screen
            for (int i =0; i<totalElements; i++) {
                if (i<1) {
                    gLogoutButton[i].handleEvent(loginCheck, sd, &e, screenShow, &logoutButton, i,inputUsernameText,inputPasswordText,masterobj);
                }else if(i<1+totalFields){
                    gMessageFieldButton[i-1].handleEvent(&e, screenShow, &messageButton, i, inputUsernameText, inputPasswordText, masterobj);
                }else if (i<totalButtons){
                    gRoomButtons[i-1-totalFields].handleEvent(loginCheck, sd, &e, screenShow,&buttonTypeWide,i,inputUsernameText,inputPasswordText,masterobj);
                }
            }
            if( e.type == SDL_KEYDOWN ){
                switch( e.key.keysym.sym ){
                    case SDLK_RETURN:
                        printf("You sent a message!\n");
                        
                        break;
                        
                    default:
                        break;
                }
            }
        }
        if (writeText) {
            if (field==0) {
                getTextString(inputUsernameText,e,textColor,outputPasswordText);
            }else if (field==1){
                getTextString(inputPasswordText, e, textColor,outputPasswordText);
            }else if (field==2){
                getTextString(inputMessageText, e, textColor, outputPasswordText);
            }
        }
    //}
}

void loginScreen( int* totalButtons, int* totalFields,int* screenShow,Screen windowSize,Button loginButton, Button fieldButton,std::string inputUsernameText, std::string inputPasswordText){
    *totalButtons=1;
    *totalFields=4;
    int element = 0;
    SDL_SetWindowSize(gWindow, windowSize.w, windowSize.h);
    //Render buttons
    for (int i = 0; i < *totalButtons; ++i) {
        gLoginButtons[i].render(screenShow,&element);
    }
    
    //Positionate login button
    gLoginButtons[0].setPosition(loginButton.x, loginButton.y);
    
    //Render background texture to screen
    gBackgroundTexture.render(0,0);
    
    //Positionate text fields
    gFieldButtons[0].setPosition(fieldButton.x,300);
    gFieldButtons[1].setPosition(fieldButton.x,420);
    
    if (inputUsernameText=="" || inputUsernameText==" ") {
        getPromptText("Username", gDefaultFont,0);
        gUsernameTextTexture.render(fieldButton.x+10,320);
    }else{
        //Render text texture
        gUsernameTextTexture.render(fieldButton.x+10, 320);
    }

    if (inputPasswordText=="" || inputPasswordText==" ") {
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

void mainScreen(int* totalButtons, int* totalFields,int nrRooms,int* screenShow,Screen windowSize,Button logoutButton,Button buttonTypeWide,Button messageButton,std::string inputMessageText){
    int buttX=0,buttY=200,element;
    *totalButtons=1+nrRooms;
    *totalFields=1;
    //std::string inputMessageText="";
    
    SDL_SetWindowSize(gWindow, windowSize.w+640, windowSize.h);
    //Render rooms texture to screen
    gFooTexture.render(0, 0);
    gChattroomTexture.render(windowSize.w, 0);
    
    //Render Username
    gUsernameTextTexture.render(10, 10);
    
    //Render and positionate logoff button
    element=0;
    gLogoutButton[0].render(screenShow,&element);
    gLogoutButton[0].setPosition(windowSize.w - logoutButton.w-3,3);
    getText("Logout", gLargeFont);
    gTextTexture.render((windowSize.w - logoutButton.w)+((logoutButton.w - gTextTexture.getWidth())/2)-3, (logoutButton.h - gTextTexture.getHeight())/2);
    
    element=1;
    gMessageFieldButton[0].setPosition(messageButton.x,messageButton.y);
    
    if (inputMessageText=="" || inputMessageText==" ") {
        getPromptText("Message", gDefaultFont, 2);
        gMessageTextTexture.render(messageButton.x,messageButton.y);
    }else{
        gMessageTextTexture.render(messageButton.x, messageButton.y);
    }
    
    //Render room buttons
    for (int i = 0; i < nrRooms; ++i) {
        gRoomButtons[i].render(screenShow,&element);
        //Positionate room buttons
        gRoomButtons[i].setPosition(buttX, (buttY+(buttonTypeWide.h*i)));
        //Get text on button
        std::ostringstream stream;
        stream << "Chat room " << i+1;
        std::string text = stream.str();
        getText(text, gLargeFont);
        gTextTexture.render((buttX+((buttonTypeWide.w - gTextTexture.getWidth())/2)), (buttY+((buttonTypeWide.h - gTextTexture.getHeight())/2)+(buttonTypeWide.h*i)));
    }
}

void runingGui(bool* loginCheck, TCPsocket* sd,int* screenShow,int* totalButtons, int* totalFields,int nrRooms,Screen windowSize,Button loginButton ,Button fieldButton, Button logoutButton,Button buttonTypeWide,Button buttonTypeSmall,Button messageButton,std::string* inputUsernameText,std::string* inputPasswordText,std::string* outputPasswordText,std::string* inputMessageText,SDL_Event* e, json_t *masterobj){
    //Render text flag
    //bool renderText = false;
    
    while( SDL_PollEvent( e ) != 0 ){
        eventHandler(loginCheck, sd, screenShow, *totalButtons, *totalFields, fieldButton, logoutButton, buttonTypeWide, buttonTypeSmall,messageButton, inputUsernameText, inputPasswordText, outputPasswordText,inputMessageText,*e,masterobj);
    }
    
    //Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);
    
    //Render texture to screen
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    
    if(*screenShow==0){
        loginScreen(totalButtons, totalFields, screenShow, windowSize, loginButton, fieldButton, *inputUsernameText, *inputPasswordText);
    }else{
        mainScreen(totalButtons, totalFields, nrRooms, screenShow, windowSize, logoutButton, buttonTypeWide,messageButton,*inputMessageText);
    }
    
    //Update screen
    SDL_RenderPresent(gRenderer);
}

int initGui(TCPsocket * sd, bool *loginCheck, json_t * globalUsersInRoomArr, json_t *globalRoomArr, json_t *messageArr, SDL_mutex *messageArrMutex){
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
            if (!((*sd)=initClient(loginCheck,globalUsersInRoomArr, globalRoomArr, messageArr, messageArrMutex ))) {
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
    int screenShow = 0,totalButtons=0,totalFields=0,nrRooms=3;//,field = 0;
    Screen windowSize;
    windowSize.w=400;
    windowSize.h=800;
    screenShow=1;
    
    Button buttonTypeSmall,buttonTypeWide,fieldButton,loginButton,logoutButton,messageButton;
    buttonTypeSmall.w=200;
    buttonTypeSmall.h=75;
    buttonTypeWide.w=400;
    buttonTypeWide.h=75;
    fieldButton.w=300;
    fieldButton.h=60;
    fieldButton.x=50;
    loginButton.w=200;
    loginButton.h=72;
    loginButton.x=100;
    loginButton.y=500;
    logoutButton.w=150;
    logoutButton.h=50;
    logoutButton.y=0;
    logoutButton.x=windowSize.w-logoutButton.w;
    messageButton.h=80;
    messageButton.w=460;
    messageButton.x=windowSize.w+90;
    messageButton.y=685;
    
    json_t *masterobj = json_object();
	json_t *globalUsersInRoomArr =json_array();
	json_t *globalRoomArr=json_array();
	json_t *messageArr=json_array();
	SDL_mutex *messageArrMutex = SDL_CreateMutex(); 
    bool loginCheck = false;
    
    //Start up SDL and create window
    if( !initGui(&sd, &loginCheck, globalUsersInRoomArr, globalRoomArr, messageArr, messageArrMutex) ){
        printf( "Failed to initialize!\n" );
    }else{
        //Main loop flag
        //bool quit = false;
            
        //Event handler
        SDL_Event e;
            
        //The current input text
        std::string inputUsernameText = "";
        std::string inputPasswordText = "";
        std::string outputPasswordText = "";
        std::string inputMessageText = "";
            
        //Enable text input
        SDL_StartTextInput();
            
        //While application is running
        while( !quit ){
            runingGui(&loginCheck, &sd, &screenShow, &totalButtons, &totalFields, nrRooms, windowSize, loginButton, fieldButton, logoutButton, buttonTypeWide, buttonTypeSmall,messageButton, &inputUsernameText, &inputPasswordText, &outputPasswordText,&inputMessageText, &e, masterobj);
        }
        //Disable text input
        SDL_StopTextInput();
    }

    //Free resources and close SDL
    close();
    
    return 0;
}
