#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SOCK_PATH "/tmp/shdsocket"
#define READ_BUF_SIZE 10000
#define ENT "\nENDOFTRANS\n\0"
#define EXIT_FROM_CLIENT "[CMD]_EXIT_FROM_CLIENT"

void clear_str(char str[], int size);

int main(int argc, char *argv[])
{

  if(argc<3){
    printf("too few args , specify inetaddr and port as args");
    exit(1);
  }
  
  int s, t, len, i, e=0, port=atoi(argv[2]);
  /* char inet_adr[16]=argv[1]; */
  
  struct sockaddr_in remote;
  char str[READ_BUF_SIZE]={0};
  char r[READ_BUF_SIZE]={0};

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  
  printf("Trying to connect...\n");
  
  remote.sin_family = AF_INET;
  remote.sin_port=htons(port);
  remote.sin_addr.s_addr=inet_addr(argv[1]);
  if (connect(s, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
    perror("connect");
    exit(1);
  }
  
  printf("Connected.\n");
  fflush(stdout);
  char string[] = {"{\"message\":\"hello\"}"};
  char *response, c;
  int size = sizeof(string);
  
  while(1){
    scanf(&c);
    printf("writing %d bytes", sizeof(string));
    fflush(stdout);
    write(s, &size, sizeof(int));
    write(s, string, sizeof(string));
    read(s, &size, sizeof(int));
    response=malloc(size+1);
    read(s,response, size );
    printf("read response : %s\n",response);
    free(response);
  }
  
}



void clear_str(char str[], int size){
  int i;
  for(i=0; i<size; i++){
    str[i]=0;
  }
  return;
}
