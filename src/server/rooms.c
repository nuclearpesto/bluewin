#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>
#include "misc.h"
#include "server.h"
#include "clienthandler.h"
#include "rooms.h"
#include "debug.h"

void add_room(char *roomName){
  SDL_LockMutex(roomsStackMutex);
  int count = pop(&availableRoomNr);
  strcpy(roomsArr[count].name, roomName);
  roomsArr[count].nrOfCurrentConns = 0;
  SDL_UnlockMutex(roomsStackMutex);

}
void delete_room(char *roomName){
  int index=0;

  SDL_LockMutex(roomsStackMutex);
  index = find_index_of_room(roomName, MAX_ROOMS);
  strcpy('\0',roomsArr[index].name );
  push(&availableRoomNr, index);
  SDL_UnlockMutex(roomsStackMutex);
}

void join_room(char *roomName, clients_t * client ){
  int index = find_index_of_room(roomName,MAX_ROOMS);
  SDL_LockMutex(roomsStackMutex);
  strcpy(client->currentRoom,roomName );
  D(printf("adding client to room %s", roomName));
  roomsArr[index].connected[roomsArr[index].nrOfCurrentConns] =client;
  roomsArr[index].nrOfCurrentConns++;
  SDL_UnlockMutex(roomsStackMutex);
}


void switch_room(char *roomName, clients_t * client ){
  leave_room(client);
  join_room(roomName, client);
}

void leave_room(clients_t * client){
  int index = find_index_of_room(client->currentRoom,MAX_ROOMS);
   int i, found=0;
  SDL_LockMutex(roomsStackMutex);
   for(i=0; i<roomsArr[index].nrOfCurrentConns; i++){
	if(client ==roomsArr[index].connected[i]){
		found = 1;
	}
	if(found){
		roomsArr[index].connected[i]=roomsArr[index].connected[i+1];
	}

   }
  roomsArr[index].nrOfCurrentConns--;
  SDL_UnlockMutex(roomsStackMutex);

}

int find_index_of_room(char *roomName, int arrLen){
  int i = 0;
  D(printf("room we are looking for is named %s \n", roomName));
  for(i = 0; i<arrLen; i++){
    if(strcmp(roomsArr[i].name, roomName) == 0){
      D(printf("roomname of %d is :%s\n",i, roomsArr[i].name));
		fflush(stdout);
		break;
    }
  }
  D(printf("returning roomindex %d\n ", i));
  
  return i;
}
