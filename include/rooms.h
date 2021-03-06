/*
Author David Boeryd
*/
#ifndef ROOMS
#define ROOMS
#define MAX_CLIENTS_PER_ROOM 100
#define MAX_ROOMS 100
#include "clienthandler.h"

struct Room{
  char name [ROOM_NAME_SIZE];
  clients_t *connected[MAX_CLIENTS_PER_ROOM];
  int nrOfCurrentConns;
}; typedef struct Room room_t;




extern SDL_mutex *roomsStackMutex;
extern room_t roomsArr[];
extern stack availableRoomNr; 
void add_room(char *roomName); //gets next available nr in roomstack, sets the name of the room to the provided roomname
							  // does nothing if no available rooms
void delete_room(char *roomName); // sets roomArr[index].roomname to '\0' if room is found otherwise does nothing
void join_room(char *roomName, clients_t * client ); // adds client pointer to end of  connected member of the glboal roomArr and increments nrConnectedClients member
int find_index_of_room(char *roomName, int arrLen); // returns index of first instance of roomArr who's name matches roomName returns -1 if no matching room found;
void leave_room(clients_t * client);  //  overwrites all connected member arr of room from index where client was found with next member and decrements nrConnectedClients member
void switch_room(char * roomname, clients_t *client); // runs leave_room and then join_room;
json_t * find_existing_rooms(int arrLen); // returns jsonarray pointer which contains all rooms currently with name not set to '\0' returns null if no rooms;
json_t *get_users_in_room(char *strroom); //returns json array pointer which contains all usernames of users in specified room, contains null if room not found or no users 
void  init_rooms(room_t *rooms, int len); //sets all roomnames to '\0', all client pointers in each room to NULL and nrCurrentConns to 0 
#endif
