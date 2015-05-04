#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string.h>
#include <stdbool.h>
#include <jansson.h>
#include "misc.h"
#include "server.h"
#include "clienthandler.h"
#include "rooms.h"


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
  index = find_index_of_room(roomName, THREAD_COUNT);
  strcpy('\0',roomsArr[index].name );
  push(&availableRoomNr, index);
  SDL_UnlockMutex(roomsStackMutex);
}

void join_room(char *roomName, clients_t * client ){
  int index = find_index_of_room(roomName,THREAD_COUNT);
  SDL_LockMutex(roomsStackMutex);
  roomsArr[index].connected[roomsArr[index].nrOfCurrentConns] =client;
  roomsArr[index].nrOfCurrentConns++;
  SDL_UnlockMutex(roomsStackMutex);
}

void leave_room(char*roomName, clients_t * client){
  int index = find_index_of_room(roomName,THREAD_COUNT);
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
  printf("room we are looking for is named %s \n", roomName);
  for(i = 0; i<arrLen; i++){
    if(strcmp(roomsArr[i].name, roomName) == 0){
		printf("roomname of %d is :%s\n",i, roomsArr[i].name);
		fflush(stdout);
		break;
    }
  }
  printf("returning roomindex %d\n ", i);
  
  return i;
}
