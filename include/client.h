//
//  client.h
//  bluewinclient
//
//  Created by Rasmus Jansson on 07/05/15.
//  Copyright (c) 2015 Rasmus Jansson. All rights reserved.
//

#ifndef __bluewinclient__client__
#define __bluewinclient__client__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//#include <json-c/json.h>
#include <jansson.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>
#include "debug.h"
#include "main.h"
#include <algorithm>
#include "debug.h"

#ifdef __APPLE__
#include <SDL2_net/SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

struct Readstruct{
	TCPsocket sd;
	bool *loginCheck;
	
};typedef struct Readstruct Readstruct;

TCPsocket initClient(bool *loginCheck);
void send_login(json_t *masterobj,std::string* inputUsernameText, std::string* inputPasswordText, TCPsocket *sd);



#endif /* defined(__bluewinclient__client__) */
