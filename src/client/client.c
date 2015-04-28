#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//#include <json-c/json.h>
#include <jansson.h>

#define SOCK_PATH "/tmp/shdsocket"
#define READ_BUF_SIZE 10000
#define ENT "\nENDOFTRANS\n\0"
#define EXIT_FROM_CLIENT "[CMD]_EXIT_FROM_CLIENT"

#include <SDL2/SDL_net.h>

struct Message{
  char message[255];

};typedef struct Message Message_s;

void clear_str(char str[], int size);
void write_to_server(TCPsocket socket, char *s, int strlen);
char* read_from_server(TCPsocket socket, char *response);
void user_input(char* msg);
char* serialize(Message_s msg);
void message_printer(char *response);



int main(int argc, char *argv[])
{
    IPaddress ip;
    TCPsocket sd;
  if(argc<3){
    printf("too few args , specify inetaddr and port as args");
    exit(1);
  }

  int t, len, i, e=0, port=atoi(argv[2]);

  /* char inet_adr[16]=argv[1]; */

  char str[READ_BUF_SIZE]={0};
  char r[READ_BUF_SIZE]={0};

if(SDLNet_Init() < 0){
    printf("stderr, SDLNet_Init: %s\n", SDLNet_GetError());
    exit(1);
}


  printf("Trying to connect...\n");
if(SDLNet_ResolveHost(&ip, argv[1], port) < 0){
    fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    exit(1);
}

if(!(sd = SDLNet_TCP_Open(&ip))){
    fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
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
    write_to_server(sd, string, strlen(string));
    if(msg.message == "exit\n"){
        exit(0);
        }
    /*else if(string == "file"){


        }*/
    else{
        read_from_server(sd, response);
        message_printer(string);
        free(response);
        }
    }

}



void clear_str(char *str, int size){
  int i;
  for(i=0; i<size; i++){
    str[i]=0;
  }
  return;
}

void write_to_server(TCPsocket socket, char *s, int len){
    SDLNet_TCP_Send(socket, &len, sizeof(int));
    SDLNet_TCP_Send(socket, s, len);
}

char* read_from_server(TCPsocket socket, char *response){

    int temp;
    SDLNet_TCP_Recv(socket, &temp, sizeof(int));
    response = (char *)malloc(temp+1);
    SDLNet_TCP_Recv(socket,response, temp );
    printf("read response : %s\n",response);
    return response;
}

void user_input(char* msg){
    fgets(msg, 254, stdin);
    //printf("%s", msg);

}

char* serialize(Message_s msg){
    char str1[10];
    int ret;
    json_t *obj;
    json_t *string;
    char *json_s;
    obj = json_object();
    strcpy(str1, "exit\n");
    ret = strcmp(msg.message, str1);
    if(ret == 0){
        string = json_string(msg.message);
        json_object_set_new(obj, "cmd", string);
        json_s = json_dumps(obj, 0);
        }
    else{
        string = json_string(msg.message);
        json_object_set_new(obj, "message", string);
        string = json_string("msg");
        json_object_set_new(obj, "cmd", string);
        json_s = json_dumps(obj, 0);
        }
    return json_s;
}

void message_printer(char *response){
    json_t *rec_obj;
    json_t *rec_message;
    char string_resp[1000];
    rec_obj = json_loads(response, 0, NULL);
    if((rec_message = json_object_get(rec_obj, "message"))!= NULL){
        strcpy(string_resp, json_string_value(rec_message));
        printf("%s", string_resp);
    }
}

