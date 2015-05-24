//
//  client.h
//  bluewinclient
//
//  Created by Rasmus Jansson on 07/05/15.
//  Copyright (c) 2015 Rasmus Jansson. All rights reserved.
//

#ifndef __bluewinclient__client__
#define __bluewinclient__client__

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>
#include "debug.h"
#include <algorithm>
#include <portaudio.h>
#include "debug.h"
#include "audio.h"


#ifdef __APPLE__
#include <SDL2_net/SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif
#define PASSWORDSIZE 250

struct Readstruct{
	TCPsocket sd;
	bool *loginCheck, *createCheck;
	json_t * globalUsersInRoomArr, *globalRoomArr, *messageArr;
	SDL_mutex *messageArrMutex;
	audiostruct_t *audiostruct;
};typedef struct Readstruct Readstruct;

struct Message{
    char *message;
	char *room;

};typedef struct Message Message_s;

typedef struct User{
    char *username;
    char *password;
	std::string room;
    int currentRoom;
}user_s;

//TCPsocket initClient(bool *loginCheck);
void send_login(json_t *masterobj,std::string* inputUsernameText, std::string* inputPasswordText, TCPsocket *sd);

void clear_str(char str[], int size);
void write_to_server(json_t *masterobj, TCPsocket *socket);
char* read_from_server(TCPsocket socket, char *response, int *numBytesRead);
void user_input(char* msg);
void serialize_message(Message_s msg);
void serialize_cmd(json_t *masterobj, char *cmd);
void serialize_password(json_t *masterobj,user_s *usr );
void serialize_username(json_t *masterobj,user_s *usr );
void serialize_room(json_t *masterobj, char* room);
void message_printer(json_t *masterobj);
void logout(json_t *masterobj, TCPsocket *socket);
int readThread (void * p);
TCPsocket initClient(audiostruct_t *audiostruct, bool *createCheck,  bool *loginCheck, json_t * globalUsersInRoomArr, json_t *globalRoomArr, json_t *messageArr, SDL_mutex * messageArrMutex);
void send_login(json_t *masterobj,std::string* inputUsernameText, std::string* inputPasswordText, TCPsocket sd);
void add_user(json_t * masterobj, user_s *usr, TCPsocket* sd);
void clear_input();
void string_convert(std::string s,char msg[20]);
void collect_rooms(json_t *masterobj, TCPsocket *socket);
void switch_room(json_t *masterobj, char *room,  TCPsocket *socket);
void add_room(json_t *masterobj, char *room, TCPsocket *socket);
void write_message(json_t *masterobj, user_s *usr, Message_s msg, TCPsocket *socket);
void get_users_in_room(json_t *masterobj, char *room, TCPsocket *socket);

#endif /* defined(__bluewinclient__client__) */
