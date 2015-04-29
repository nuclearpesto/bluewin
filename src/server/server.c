
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_thread.h>
#include "misc.h"
#include "clienthandler.h"
#include "server.h"
#include "rooms.h"
clients_t clientsArr[THREAD_COUNT];
room_t roomsArr[THREAD_COUNT];
stack availableClientNr, availableRoomNr;
SDL_mutex *clientsStackMutex, *roomsStackMutex;
SDL_Thread *threadIds;
  
int main(int argc, char **argv){
  int port, t;
    int i;
   IPaddress ip;
   TCPsocket servsock, acceptsock;
  SDLNet_SocketSet set;
  
  if(argc<2){
    printf("usage: server [port]");
    exit(1);
  }

  port=atoi(argv[1]);
	 if(SDL_Init(0)==-1) {
		printf("SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}
	if(SDLNet_Init()==-1) {
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
		exit(2);
	}
 
  createstack(&availableClientNr, THREAD_COUNT);
  createstack(&availableRoomNr, THREAD_COUNT);
  printf("created stacks\n");
  fflush(stdout);
  fill_int_stack(&availableClientNr, THREAD_COUNT);
  fill_int_stack(&availableRoomNr, THREAD_COUNT);
  printf("filled stacks\n");
  fflush(stdout);
  clientsStackMutex = SDL_CreateMutex();
  roomsStackMutex = SDL_CreateMutex();
  printf("initialized mutexes\n");
  fflush(stdout);
  add_room("default");
  printf("created default room\n");
  fflush(stdout);
	
	set = SDLNet_AllocSocketSet(1);
 if(SDLNet_ResolveHost(&ip,NULL,port)==-1) {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    exit(1);
	}

	servsock=SDLNet_TCP_Open(&ip);
	if(!servsock) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(2);
	}
  
  
  
if(SDLNet_ResolveHost(&ip,NULL,9999)==-1) {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    exit(1);
}
	SDLNet_TCP_AddSocket(set, servsock);
 
  while(1){
	if(SDLNet_CheckSockets(set, 100)){
	   acceptsock=SDLNet_TCP_Accept(servsock);
		if(!acceptsock) {
			printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
		}
		else {
			// communicate over new_tcpsock
		
			printf("adding a client\n");
			fflush(stdout);
			add_Client(acceptsock, &availableClientNr);
			printf("created thread\n");
			fflush(stdout);

		}
	  }
	}
}


