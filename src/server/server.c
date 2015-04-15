
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
#include "clienthandler.h"
#include "server.h"

clients_t clients_arr[THREAD_COUNT];
int count;
int main(int argc, char **argv){
  int listensocket, acceptsocket, port, t; 
  struct sockaddr_in local, remote;
  pthread_t thread_ids[THREAD_COUNT];

  
  if(argc<2){
    printf("usage: server [port]");
    exit(1);
  }
  
  port=atoi(argv[1]);
  if((listensocket=socket(AF_INET, SOCK_STREAM, 0))==-1)  {
    perror("socket");
    exit(1);
  }
   
  
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
    clients_t *c=  create_client(count, acceptsocket);
    pthread_create(&thread_ids[count], NULL, &handle, c); 
    printf("created thread");
    fflush(stdout);
  }
  
    
}


clients_t* create_client(int count, int socket){
  clients_arr[count].inet_addr = 0;
  clients_arr[count].socket = socket;
  
  return &clients_arr[count];
}