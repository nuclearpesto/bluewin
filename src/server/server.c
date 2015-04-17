
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <syslog.h>
#include <pwd.h>
#include <pthread.h>
#include "misc.h"
#include "clienthandler.h"
#include "server.h"
clients_t clientsArr[THREAD_COUNT];
int count;
stack availableClientNr;
pthread_mutex_t clientsStackMutex;
pthread_t threadIds[THREAD_COUNT];
  
int main(int argc, char **argv){
  int listensocket, acceptsocket, port, t;
  struct sockaddr_in local, remote;
    int i;
   
  if(argc<2){
    printf("usage: server [port]");
    exit(1);
  }

  port=atoi(argv[1]);
  if((listensocket=socket(AF_INET, SOCK_STREAM, 0))==-1)  {
    perror("socket");
    exit(1);
  }

  createstack(&availableClientNr, THREAD_COUNT);
  fill_int_stack(&availableClientNr, THREAD_COUNT);
  
  pthread_mutex_init(&clientsStackMutex, NULL);


  
  local.sin_family = AF_INET;
  local.sin_port = htons(port);
  local.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(listensocket,(struct sockaddr *)&local, sizeof(local))==-1){
    perror("bind");
    exit(1);
  }
   // listen
  if(listen(listensocket,5)==-1){
    perror("listen");
    exit(1);
  }

  while(1){
    t=sizeof(remote);
    if((acceptsocket=accept(listensocket,(struct sockaddr *)&remote, &t))==-1){
      perror("accept");
    }
    add_Client(acceptsocket, &availableClientNr);
    printf("created thread");
    fflush(stdout);
  }


}


