
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
stack available_client_nr;
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

    createstack(&available_client_nr, THREAD_COUNT);
    for(i=THREAD_COUNT;i>=0;i--)
    {
        push(&available_client_nr, i);
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
    clients_t *c=  create_client(acceptsocket, &available_client_nr);
    pthread_create(&thread_ids[count], NULL, &handle, c);
    printf("created thread");
    fflush(stdout);
  }


}


clients_t* create_client(int socket, stack *available_client_nr){
    int count = pop(available_client_nr);
  clients_arr[count].inet_addr = 0;
  clients_arr[count].socket = socket;

  return &clients_arr[count];
}


void createstack(stack *s,int capacity)
{
    s->current_place = (int *)malloc(sizeof(int )*capacity);
    s->capacity = capacity;
    s->size = 0;
}

int push(stack *s, int nr)
{

    if(s->size == s->capacity)
    {
        printf("Stack overflow");
        return 1;
    }
    *(s->current_place) = nr;
    s->size++;
    s->current_place++;
    return 0;
}

int pop(stack *s)
{
    if(s->size == 0)
    {
        return -1;
    }
    int nr = *(s->current_place);
    s->size--;
    s->current_place--;
    return nr;
}
