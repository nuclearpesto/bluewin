#ifndef ROOMS
#define ROOMS
#define MAX_CLIENTS_PER_ROOM 100

struct Room{
  char name [255];
  clients_t *connected[MAX_CLIENTS_PER_ROOM];
  int nrOfCurrentConns;
}; typedef struct Room room_t;




extern pthread_mutex_t roomsStackMutex;
extern room_t roomsArr[];
extern stack availableRoomNr; 
void add_room(char *roomName);
void delete_room(char *roomName);
void join_room(char *roomName, clients_t * client );
int find_index_of_room(char *roomName, int arrLen);

#endif
