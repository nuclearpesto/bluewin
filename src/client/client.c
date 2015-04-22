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
void write_to_server(int socket, char s[], int strlen);
void read_from_server(int socket, char *response);

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
  char string[] = {"{\"cmd\":\"msg\", \"message\":\"hello\"}"};
  char *response;
  int size = sizeof(string);
  char c;
  
  while(1){
    scanf("%c" , &c);
    write_to_server(s, string, strlen(string));
    read_from_server(s, response);
    }

}



void clear_str(char str[], int size){
  int i;
  for(i=0; i<size; i++){
    str[i]=0;
  }
  return;
}

void write_to_server(int socket, char s[], int len){
    int temp = strlen(s);
    write(socket, &temp, sizeof(int));
    write(socket, s, temp);
}

void read_from_server(int socket, char *r){

    int temp=0;
    char test[100];
    read(socket, &temp, sizeof(int));
    printf("%d bytes to read\n ", temp);
    if((r= calloc(temp+1, sizeof(char)))==NULL){
      printf("out of memory\n");
    }
    printf("r pointer is %p \n", r);
    fflush(stdout);
    read(socket, r, temp);
    *( r+temp)='\0';
    printf("read response : %s\n",r);
    fflush(stdout);
    free(r);

}
