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
void add_room(char *roomName);
void delete_room(char *roomName);
void join_room(char *roomName, clients_t * client );
int find_index_of_room(char *roomName, int arrLen);
void leave_room(clients_t * client);
void switch_room(char * roomname, clients_t *client);
#endif
