//
//  main.cpp
//  bluewin_gui
//
//  Created by Rasmus Jansson on 17/04/15.
//  Copyright (c) 2015 Rasmus Jansson. All rights reserved.
//

#ifdef __APPLE__
//If you are on mac
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <string>
#endif

#ifdef __MINGW32__
//If you are on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#endif

bool writeText = false;
int field = 0;

typedef struct screen{
    int w,h;
}Screen;

typedef struct button{
    int x,y,w,h;
}Button;

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
    void handleEvent(SDL_Event* e, int* screenShow, Button* button, int i);
    //Shows button sprite
    void render(int screenShow);
    
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

//

//Rendered texture
LTexture gTextTexture;

//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;
LTexture gMainTexture;
LTexture gPromtTextTexture;
LTexture gInputTextTexture;

//Login button sprites
SDL_Rect gLoginSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gLoginButtonSpriteSheetTexture;

SDL_Rect gRoomSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gRoomButtonSpriteSheetTexture;

//Buttons objects
LButton gLoginButtons[TOTAL_BUTTONS];
LButton gRoomButtons[ROOM_BUTTON_TOTAL];
LButton gFieldButtons[2];


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

void action(int* e){
    if (*e == 0) {
        printf("Hej 0");
        *e=1;
        
    }else if (*e == 1){
        printf("Hej 1");
        *e=0;
    }
}

void LButton::handleEvent(SDL_Event* e,int* screenShow, Button* button,int selected){
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
                    switch (selected) {
                        case 0:
                            field=0;
                            break;
                            
                        case 1:
                            field=1;
                            break;
                            
                        default:
                            break;
                    }
                    //focusText=true;
                    action(screenShow);
                    
                    break;
            }
        }
    }
}

void LButton::render(int screenShow){
    if (screenShow == 0) {
        //show current login button sprite
        gLoginButtonSpriteSheetTexture.render(mPosition.x, mPosition.y,&gLoginSpriteClips[mCurrentSprite]);
    }else if (screenShow == 1){
        //Show current room button sprite
        gRoomButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gRoomSpriteClips[mCurrentSprite]);
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
        if (!gPromtTextTexture.loadFromRenderedText(text, textColor, gFont)) {
            printf("Failed to render promt text texture!\n");
        }
    }else if (select==1){
        //Get prompt text
        SDL_Color textColor = {0,0,0};
        if (!gInputTextTexture.loadFromRenderedText(text, textColor, gFont)) {
            printf("Failed to render promt text texture!\n");
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
    
    //Load Login texture
    //gTexture = loadTexture("bluewinimg/login.png");
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
            gLoginSpriteClips[i].y=i*73;
            gLoginSpriteClips[i].w=BUTTON_WIDTH;
            gLoginSpriteClips[i].h=BUTTON_HEIGHT;
        }
    }
    
    //Load rooms texture
    if (!gFooTexture.loadFromFile("bluewinimg/main.png")) {
        printf("Failed to load rooms texture");
        success = false;
    }
    if (!gRoomButtonSpriteSheetTexture.loadFromFile("bluewinimg/roombutton.png")) {
        printf("Failed to load rooms button sprite texture");
        success = false;
    }else{
        //Set sprites
        for (int i = 0; i < BUTTON_SPRITE_TOTAL; i++) {
            gRoomSpriteClips[i].x=0;
            gRoomSpriteClips[i].y=i*80;
            gRoomSpriteClips[i].w=ROOM_BUTTON_WIDTH;
            gRoomSpriteClips[i].h=ROOM_BUTTON_HEIGHT;
        }
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
    gTextTexture.free();
    gInputTextTexture.free();
    gPromtTextTexture.free();
    
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

int main( int argc, char* args[] ){
    //Initialize varibles
    int screenShow = 0,boxInput = 0;//,field = 0;
    Screen windowSize;
    windowSize.w=400;
    windowSize.h=800;

    Button buttonTypeSmall,buttonTypeWide,fieldButton,loginButton;
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
    
    //Start up SDL and create window
    if( !init(windowSize) ){
        printf( "Failed to initialize!\n" );
    }else{
        //Load media
        if( !loadMedia() ){
            printf( "Failed to load media!\n" );
        }else{
            //Main loop flag
            bool quit = false;
            
            //Event handler
            SDL_Event e;
            
            //Set text color as black
            SDL_Color textColor = {0,0,0,0xff};
            
            //The current input text
            std::string inputUsernameText = " ";
            std::string inputPasswordText = " ";
            
            //Enable text input
            SDL_StartTextInput();
            
            //While application is running
            while( !quit ){
                //Render text flag
                bool renderText = false;
                
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 ){
                    //User requests quit
                    if( e.type == SDL_QUIT ){
                        quit = true;
                    }
                    else if (screenShow==0) {
                        //Handle login button events
                        for (int i = 0; i < TOTAL_BUTTONS; i++) {
                            gLoginButtons[i].handleEvent(&e,&screenShow,&buttonTypeSmall,i);
                        }
                        //Handle username and password field
                        for (int i = 0; i < 3; i++) {
                            gFieldButtons[i].handleEvent(&e, &boxInput, &fieldButton,i);
                        }
                    }else if(screenShow == 1){
                        //Handle rooms button events
                        for (int i = 0; i < ROOM_BUTTON_TOTAL; i++) {
                            gRoomButtons[i].handleEvent(&e, &screenShow,&buttonTypeWide,i);
                        }
                    }
                    if (writeText) {
                        if (field==0) {
                            //Special key input
                            if (e.type == SDL_KEYDOWN){
                                //Handle backspace
                                if (e.key.keysym.sym == SDLK_BACKSPACE && inputUsernameText.length()>0) {
                                    //Lop off character
                                    inputUsernameText.pop_back();
                                    renderText = true;
                                }//Handle copy
                                else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL){
                                    SDL_SetClipboardText(inputUsernameText.c_str());
                                }//Handle paste
                                else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL){
                                    inputUsernameText = SDL_GetClipboardText();
                                    renderText = true;
                                }
                            }//Special text input event
                            else if (e.type == SDL_TEXTINPUT){
                                //Not copy or pasting
                                if (!((e.text.text[0]=='c' || e.text.text[0]=='C') && (e.text.text[0]=='v' || e.text.text[0]=='V') && SDL_GetModState() & KMOD_CTRL) ) {
                                    //Append character
                                    if (field==0) {
                                        inputUsernameText += e.text.text;
                                        printf("%s\n",inputUsernameText.c_str());
                                        renderText = true;
                                    }
                                }
                            }
                            
                            //Rerender text if needed
                            if (renderText) {
                                //Text is not empty
                                if (inputUsernameText !="") {
                                    //Render new text, username
                                    gPromtTextTexture.loadFromRenderedText(inputUsernameText.c_str(), textColor, gDefaultFont);
                                }//Text is empty
                                else{
                                    //Render space texture, username
                                    gPromtTextTexture.loadFromRenderedText(inputUsernameText.c_str(), textColor, gDefaultFont);
                                }
                             }
                        }else if (field==1){
                            //Special key input
                            if (e.type == SDL_KEYDOWN){
                                //Handle backspace
                                if (e.key.keysym.sym == SDLK_BACKSPACE && inputPasswordText.length()>0) {
                                    //Lop off character
                                    inputPasswordText.pop_back();
                                    renderText = true;
                                }//Handle copy
                                else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL){
                                    SDL_SetClipboardText(inputPasswordText.c_str());
                                }//Handle paste
                                else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL){
                                    inputPasswordText = SDL_GetClipboardText();
                                    renderText = true;
                                }
                            }//Special text input event
                            else if (e.type == SDL_TEXTINPUT){
                                //Not copy or pasting
                                if (!((e.text.text[0]=='c' || e.text.text[0]=='C') && (e.text.text[0]=='v' || e.text.text[0]=='V') && SDL_GetModState() & KMOD_CTRL) ) {
                                    //Append character
                                    inputPasswordText += e.text.text;
                                    printf("%s\n",inputPasswordText.c_str());
                                    renderText = true;
                                    }
                                }
                            }
                        //Rerender text if needed
                        if (renderText) {
                            //Text is not empty
                            if (inputPasswordText !="") {
                                //Render new text, username
                                gInputTextTexture.loadFromRenderedText(inputPasswordText.c_str(), textColor, gDefaultFont);
                            }//Text is empty
                            else{
                                //Render space texture, username
                                gInputTextTexture.loadFromRenderedText(inputPasswordText.c_str(), textColor, gDefaultFont);
                            }
                        }
                        
                        /*//Special key input
                        if (e.type == SDL_KEYDOWN){
                            //Handle backspace
                            if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length()>0) {
                                //Lop off character
                                inputText.pop_back();
                                renderText = true;
                            }//Handle copy
                            else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL){
                                SDL_SetClipboardText(inputText.c_str());
                            }//Handle paste
                            else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL){
                                inputText = SDL_GetClipboardText();
                                renderText = true;
                            }
                        }//Special text input event
                        else if (e.type == SDL_TEXTINPUT){
                            //Not copy or pasting
                            if (!((e.text.text[0]=='c' || e.text.text[0]=='C') && (e.text.text[0]=='v' || e.text.text[0]=='V') && SDL_GetModState() & KMOD_CTRL) ) {
                                //Append character
                                if (field==0) {
                                    inputUsernameText += e.text.text;
                                    printf("%s\n",inputUsernameText.c_str());
                                    renderText = true;
                                }else if (field==1){
                                    inputPasswordText += e.text.text;
                                    printf("%s\n",inputPasswordText.c_str());
                                    renderText = true;
                                }
                            }
                        }*/
                    }
                }
                
                //Rerender text if needed
                /*if (renderText) {
                    //Text is not empty
                    if (inputText !="") {
                        if (field==0) {
                            //Render new text, username
                            gPromtTextTexture.loadFromRenderedText(inputText.c_str(), textColor, gDefaultFont);
                        }else{
                            //Render new text, passw0rd
                            gInputTextTexture.loadFromRenderedText(inputText.c_str(), textColor, gDefaultFont);
                        }
                    }//Text is empty
                    else{
                        if (field==1) {
                            //Render space texture, username
                            gPromtTextTexture.loadFromRenderedText(inputText.c_str(), textColor, gDefaultFont);
                        }else{
                            //Render space texture, password
                            gInputTextTexture.loadFromRenderedText(" ", textColor, gDefaultFont);
                        }
                    }
                }*/
                
                //Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
                
                //Render texture to screen
                SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
                
                if(screenShow==0){
                    //Render buttons
                    for (int i = 0; i < TOTAL_BUTTONS; ++i) {
                        gLoginButtons[i].render(screenShow);
                    }
                    //Positionate login button
                    gLoginButtons[0].setPosition(loginButton.x, loginButton.y);
                    gLoginButtons[1].setPosition(loginButton.x, loginButton.y);
                    
                    //Render background texture to screen
                    gBackgroundTexture.render(0,0);
                    
                    //Positionate text fields
                    gFieldButtons[0].setPosition(fieldButton.x,300);
                    gFieldButtons[1].setPosition(fieldButton.x,420);
                    gFieldButtons[2].setPosition(fieldButton.x, 300);
                    
                    if (inputUsernameText=="" || inputUsernameText==" ") {
                        getPromptText("Username", gDefaultFont,0);
                        gPromtTextTexture.render(fieldButton.x+10,320);
                    }
                    
                    if (inputPasswordText=="" || inputPasswordText==" ") {
                        getPromptText("Password", gDefaultFont,1);
                        gInputTextTexture.render(fieldButton.x+10, 440);
                    }
                    
                    //Render text texture
                    gPromtTextTexture.render(fieldButton.x+10, 320);
                    gInputTextTexture.render(fieldButton.x+10, 440);
                    
                    //Render current frame
                    getText("BlueWin", gLargeBoldFont);
                    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,100);
                    getText("Username",gLargeBoldFont);
                    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,230);
                    getText("Password",gLargeBoldFont);
                    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,380);
                    getText("Lost password",gDefaultFont);
                    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,(windowSize.h - 30));
                    getText("Create account",gDefaultFont);
                    gTextTexture.render((windowSize.w - gTextTexture.getWidth())/2,(windowSize.h - 60));
                }else{
                    int buttX=0,buttY=200;
                    
                    //Render rooms texture to screen
                    gFooTexture.render(0, 0);
                    
                    //Render Username
                    gPromtTextTexture.render(10, 10);
                    
                    //Render buttons
                    for (int i = 0; i < TOTAL_BUTTONS; ++i) {
                        gRoomButtons[i].render(screenShow);
                    }
                    //Positionate room buttons
                    gRoomButtons[0].setPosition(buttX, buttY);
                    gRoomButtons[1].setPosition(buttX, buttY);
                }
                
                //Update screen
                SDL_RenderPresent(gRenderer);
            }
            //Disable text input
            SDL_StopTextInput();
        }
    }
    
    //Free resources and close SDL
    close();
    
    return 0;
}
