//
//  gui.h
//  bluewin_gui
//
//  Created by Rasmus Jansson on 05/05/15.
//  Copyright (c) 2015 Rasmus Jansson. All rights reserved.
//

#ifndef __bluewin_gui__gui__
int mainUI(int nrRooms);
std::string inputUsernameText = "";
std::string inputPasswordText = "";
#define __bluewin_gui__gui__

#ifdef __APPLE__
//If you are on mac
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#endif

#ifdef __MINGW32__
//If you are on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#endif

#endif /* defined(__bluewin_gui__gui__) */