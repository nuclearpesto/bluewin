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
#include "clientcrypt.h"



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

//void send_login(json_t *masterobj,std::string* inputUsernameText, std::string* inputPasswordText, TCPsocket *sd);

void send_login(json_t *masterobj,std::string* inputUsernameText, std::string* inputPasswordText, TCPsocket *sd, SDL_mutex *writeMutex); /* setializes cmd, password, username and runs write to server  */

void clear_str(char str[], int size); //sets a string to \0 in all places
void write_to_server(json_t *masterobj, TCPsocket *socket, SDL_mutex *writeMutex); //writes a json string to server
char* read_from_server(TCPsocket socket, char *response, int *numBytesRead); //reads a certain number of bytes from socket
void user_input(char* msg); //does nothing
void serialize_message(Message_s msg); //
void serialize_cmd(json_t *masterobj, char *cmd); //adds cmd key-value pair to recieved  json object
void serialize_password(json_t *masterobj,user_s *usr ); //adds passsword key-value pair to recieved  json object
void serialize_username(json_t *masterobj,user_s *usr );//adds username key-value pair to recieved  json object
void serialize_room(json_t *masterobj, char* room); //adds room key-value pair to recieved  json object
void message_printer(json_t *masterobj);	//prints username and message values of recieved json object
void logout(json_t *masterobj, TCPsocket *socket, SDL_mutex *writeMutex); // runs serialize_cmd with char cmd set to logout, then writes to server
int readThread (void * p); /*reading threads function, runs read_from_server continuosly, if it fails to read
							it closes the socket and exits.
							if it successfully recieves something, tries to load it as a json object and then tries to do something according to the cmd value
							if it does not find cmd value or cannot load as json it discards the malformed information and continues to read from the socket.
							*/
TCPsocket initClient( audiostruct_t *audiostruct, bool *createCheck,  bool *loginCheck, json_t * globalUsersInRoomArr, json_t *globalRoomArr, json_t *messageArr, SDL_mutex * messageArrMutex, SDL_mutex *writeMutex); 
/*creates the socket and tries to connect to the server. Then creates the audio and read threads*/void add_user(json_t * masterobj, user_s *usr, TCPsocket* sd, SDL_mutex *writeMutex); /* serializes cmd, username and password and runs write_to _server*/
void collect_rooms(json_t *masterobj, TCPsocket *socket, SDL_mutex *writeMutex); /*serializes cmd and writes to server*/
void switch_room(json_t *masterobj, char *room,  TCPsocket *socket, SDL_mutex *writeMutex);/*serializes cmd, room and runs write_to_server*/
void delete_room(json_t *masterobj, char *room, TCPsocket *socket, SDL_mutex *writeMutex); /*serializes cmd, room and runs write_to_server*/
void add_room(json_t *masterobj, char *room, TCPsocket *socket, SDL_mutex *writeMutex);/*serializes cmd, room and runs write_to_server*/
void write_message(json_t *masterobj, user_s *usr, Message_s msg, TCPsocket *socket, SDL_mutex *writeMutex);/*serializes cmd, room, username, message and runs write_to_server*/
void get_users_in_room(json_t *masterobj, char *room, TCPsocket *socket, SDL_mutex *writeMutex); /*serializes cmd, room and runs write_to_server*/

#endif /* defined(__bluewinclient__client__) */
