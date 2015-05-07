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
  if(count>=0){
    strcpy(roomsArr[count].name, roomName);
    roomsArr[count].nrOfCurrentConns = 0;
  }
  SDL_UnlockMutex(roomsStackMutex);

}
void delete_room(char *roomName){
  int index=0;

  SDL_LockMutex(roomsStackMutex);
  index = find_index_of_room(roomName, MAX_ROOMS);
  if(index>=0){
    strcpy('\0',roomsArr[index].name );
    roomsArr[index].nrOfCurrentConns = 0;
    push(&availableRoomNr, index);
  }
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
  bool success = false;
  for(i = 0; i<arrLen; i++){
    if(strcmp(roomsArr[i].name, roomName) == 0){
      D(printf("roomname of %d is :%s\n",i, roomsArr[i].name));
      fflush(stdout);
      success = true;
      break;
    }
  }
  D(printf("returning roomindex %d\n ", i));
  if(success){
    return i;
  }
  return -1;
}


void init_rooms(room_t rooms[], int len){
  int i =0, j =0;
  for(i=0; i<len; i++){
    rooms[i].name[0] = '\0';
    rooms[i].nrOfCurrentConns = 0;
    for(j=0; j<MAX_CLIENTS_PER_ROOM; j++){
      rooms[i].connected[j] = NULL;
    }
  }
  
}

json_t * find_existing_rooms(int arrLen){
  int i = 0;
  json_t *current, *available_rooms_arr = json_array();
  
  D(printf("gong to check array\n"));
  fflush(stdout);
  SDL_LockMutex(roomsStackMutex);
  D(printf("gong to check array\n"));
  fflush(stdout);
  for(i = 0; i<arrLen; i++){
    if(roomsArr[i].name[0]!= '\0'){
      D(printf("found room %s\n", roomsArr[i].name));
      fflush(stdout);
      current = json_string(roomsArr[i].name);
      json_array_append_new(available_rooms_arr,current);
    }
  }
  
  SDL_UnlockMutex(roomsStackMutex);
  if(!json_array_size(available_rooms_arr)){
    return NULL;
  }
  return available_rooms_arr;
}
