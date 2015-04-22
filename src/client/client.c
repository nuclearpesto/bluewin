#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <json-c/json.h>

#define SOCK_PATH "/tmp/shdsocket"
#define READ_BUF_SIZE 10000
#define ENT "\nENDOFTRANS\n\0"
#define EXIT_FROM_CLIENT "[CMD]_EXIT_FROM_CLIENT"

struct Message{
  char message[255];

};typedef struct Message Message_s;

void clear_str(char str[], int size);
void write_to_server(int socket, char s[], int strlen);
void read_from_server(int socket, char *response);
void user_input(char* msg);
char* serialize(Message_s msg);

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
  char *string;
  char *response;
  int size = sizeof(string);
    Message_s msg;

  while(1){
    user_input(msg.message);
    string = serialize(msg);
    write_to_server(s, string, strlen(string));
    read_from_server(s, response);
    free(response);
    }

}



void clear_str(char *str, int size){
  int i;
  for(i=0; i<size; i++){
    str[i]=0;
  }
  return;
}

void write_to_server(int socket, char *s, int len){
    write(socket, &len, sizeof(int));
    write(socket, s, len);
}

void read_from_server(int socket, char *response){

    int temp;
    read(socket, &temp, sizeof(int));
    response= (char *)malloc(temp+1);
    read(socket,response, temp );
    printf("read response : %s\n",*response);

}

void user_input(char* msg){
    fgets(msg, 254, stdin);
    printf("%s", msg);

}

char* serialize(Message_s msg){
    json_object *obj;
    json_object *string;
    char *json_string;
    obj = json_object_new_object();
    string = json_object_new_string(msg.message);
    json_object_object_add(obj, "message", string);
    json_string = json_object_to_json_string(obj);
    return json_string;
}
