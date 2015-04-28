#include <pthread.h>
#include <string.h>
#include "misc.h"
#include "server.h"
#include "clienthandler.h"
#include "rooms.h"


void add_room(char *roomName){
  pthread_mutex_lock(&roomsStackMutex);
  int count = pop(&availableRoomNr);
  strcpy(roomsArr[count].name, roomName);
  roomsArr[count].nrOfCurrentConns = 0;
  pthread_mutex_unlock(&roomsStackMutex);

}
void delete_room(char *roomName){
  int index=0;
  
  pthread_mutex_lock(&roomsStackMutex);
  index = find_index_of_room(roomName, THREAD_COUNT);
  strcpy('\0',roomsArr[index].name );
  push(&availableRoomNr, index);
  pthread_mutex_unlock(&roomsStackMutex); 
}

void join_room(char *roomName, clients_t * client ){
  int index = find_index_of_room(roomName,THREAD_COUNT);
  pthread_mutex_lock(&roomsStackMutex);
  roomsArr[index].connected[roomsArr[index].nrOfCurrentConns] =client; 
  roomsArr[index].nrOfCurrentConns++;
  pthread_mutex_unlock(&roomsStackMutex);
}

int find_index_of_room(char *roomName, int arrLen){
  int i = -1;
  for(i = 0; i<arrLen; i++){
    if(strcmp(roomsArr[i].name, roomName) == 0){
      break;
    }
  }
  return i;
}
