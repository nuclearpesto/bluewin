
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>
#include <jansson.h>
#include "misc.h"
#include "clienthandler.h"
#include "server.h"
#include "rooms.h"
#include "users.h"
#include "debug.h"

clients_t clientsArr[THREAD_COUNT];
room_t roomsArr[THREAD_COUNT];
stack availableClientNr, availableRoomNr;
SDL_mutex *clientsStackMutex, *roomsStackMutex, *UsersDbMutex;
SDL_Thread *threadIds;

int main(int argc, char **argv){
  int port, t, pid, i;
  IPaddress ip;
  TCPsocket servsock, acceptsock;
  SDLNet_SocketSet set;
  FILE *fp;
  bool success;
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
  D( printf("created stacks\n"));
  fflush(stdout);
  fill_int_stack(&availableClientNr, THREAD_COUNT);
  fill_int_stack(&availableRoomNr, THREAD_COUNT);
  D(printf("filled stacks\n"));
  fflush(stdout);
  clientsStackMutex = SDL_CreateMutex();
  UsersDbMutex= SDL_CreateMutex();
  roomsStackMutex = SDL_CreateMutex();
  D(printf("initialized mutexes\n"));
  fflush(stdout);
  add_room("default");
  D(printf("created default room\n"));
  fflush(stdout);
  success =  users_init();

  if(!success){
	  perror("could not init users");
	  exit(1);
	}
	D(printf("checked db"));
  
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
	//print pid to file forrt killscript
	pid=getpid();
	fp = fopen(PIDFILELOC, "w");
	fprintf(fp, "%d", pid);
	fclose(fp);
	
  while(1){
	if(SDLNet_CheckSockets(set, 100)){
	   acceptsock=SDLNet_TCP_Accept(servsock);
		if(!acceptsock) {
			printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
		}
		else {
			// communicate over new_tcpsock

		  D(printf("adding a client\n"));
			fflush(stdout);
			add_Client(acceptsock, &availableClientNr);
			D(printf("created thread\n"));
			fflush(stdout);

		}
	  }
	}
}


