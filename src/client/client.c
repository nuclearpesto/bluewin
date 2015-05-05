#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
<<<<<<< HEAD
#include <SDL2/SDL_net.h>
=======
>>>>>>> 7687fa6020f0c96214a464bfc9a9870c19915acb
#include <stdbool.h>
#include "debug.h"

#ifdef __APPLE__
#include <SDL2_net/SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

#define READ_BUF_SIZE 1000

bool login = false;

struct Message{
  char message[255];

};typedef struct Message Message_s;

struct User{
    char username[20];
    char password[20];
}; typedef struct User user_s;

void clear_str(char str[], int size);
void write_to_server(json_t *masterobj, TCPsocket socket);
char* read_from_server(TCPsocket socket, char *response);
void user_input(char* msg);
void user_login(user_s *usr);
void serialize_message(json_t *masterobj, Message_s msg);
void serialize_cmd(json_t *masterobj, char *cmd);
void serialize_password(json_t *masterobj, user_s *usr );
void serialize_username(json_t *masterobj, user_s *usr );
void message_printer(json_t *masterobj);
int readThread (void * p);
void send_login(json_t * masterobj, user_s *usr, TCPsocket sd);
void add_user(json_t * masterobj, user_s *usr, TCPsocket sd);
void clear_input();


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
  user_s usr;
  int choice;

  int size = sizeof(string);
  Message_s msg;
  json_t *masterobj;

    masterobj = json_object();
    SDL_CreateThread(readThread, "reader", &sd);

    printf("0: login\n 1: new user\n:");
    fscanf(stdin, "%d", &choice);
    switch(choice){
    case 0:
      clear_input();
      send_login(masterobj, &usr, sd);
      break;
    case 1:
      clear_input();
      add_user(masterobj, &usr, sd);
      send_login(masterobj, &usr, sd);
      break;
    }


    printf("Welcome!\n");
  while(1){
    user_input(msg.message);
    if(msg.message == "exit\n"){
			exit(0);
		}
    serialize_message(masterobj, msg);
    serialize_cmd(masterobj, "msg");
	//printf("%s\n", string);
    write_to_server(masterobj, sd);
    }

}


int readThread (void * p){
	TCPsocket *sd = (TCPsocket *) p;
	char *response;
	char *string;
	json_t *masterobj;
	json_t *keycheckobj;
	masterobj = json_object();
	while(1){

	  string = read_from_server(*sd, response);
	  //D(printf("recieved %s\n", string));
	  masterobj = json_loads(string, 0, NULL);
	  if(masterobj == NULL){
	    free(string);
	  }
	  else{
	    free(string);
	    if((keycheckobj = json_object_get(masterobj, "login")) != NULL){
	      if(json_is_true(keycheckobj)==1){
		login = true;
	      }
	    }

	  }
	  message_printer(masterobj);
	  //printf("gonna free");
	  //printf("freed\n");
   }
}



void clear_str(char *str, int size){
  int i;
  for(i=0; i<size; i++){
    str[i]=0;
  }
  return;
}
void clear_input(){
  char c='\0';
  while(c!='\n'){
    scanf("%c",&c);
  }
}


void send_login(json_t * masterobj, user_s *usr, TCPsocket sd){
  int c;
  while(!login){
        user_login(usr);
        //printf("%s", usr.username);
        serialize_username(masterobj, usr);
        //printf("user\n");
        fflush(stdout);
        serialize_password(masterobj, usr);
        //printf("pass\n");
        fflush(stdout);
        serialize_cmd(masterobj, "login");
        //printf("cmd\n");
        fflush(stdout);
        write_to_server(masterobj, sd);

        c = 0;
        while(!login && c<10 ){
           sleep(1);
            c++;
            printf("%d\n", c);
            if(login){
                break;
            }
        }
    }

}


void add_user(json_t * masterobj, user_s *usr, TCPsocket sd){

  user_login(usr);
        //printf("%s", usr.username);
  serialize_username(masterobj, usr);
        //printf("user\n");
  fflush(stdout);
  serialize_password(masterobj, usr);
        //printf("pass\n");
  fflush(stdout);
  serialize_cmd(masterobj, "add user");
        //printf("cmd\n");
  fflush(stdout);
  write_to_server(masterobj, sd);



}


void write_to_server(json_t *masterobj, TCPsocket socket){
    char *json_s;
    int len;
    json_s = json_dumps(masterobj, 0);
    //kryptera
    //encrypt_Handler(json_s);
    //puts(json_s);//kontroll
    len = strlen(json_s);
	//printf("len is %d", len);
    SDLNet_TCP_Send(socket, &len, sizeof(int));
	SDLNet_TCP_Send(socket, json_s, len);
    //printf("%s", json_s);
}

char* read_from_server( TCPsocket socket, char *response){

    int temp;
    SDLNet_TCP_Recv(socket, &temp, sizeof(int));
    response = (char *)malloc(temp+1);
    SDLNet_TCP_Recv(socket,response, temp );
	response[temp] = '\0';
	//printf("read response : %s\n",response);
    //dekryptera
    //decrypt_Handler(response);
    return response;
}

void user_input(char* msg){
	fgets(msg, 254, stdin);
	msg[strlen(msg)-1]='\0';
    //printf("%s", msg);

}

void serialize_message(json_t *masterobj, Message_s msg){
    char str1[10];
    int ret;
    json_t *string;
/*    strcpy(str1, "exit\n");
    ret = strcmp(msg.message, str1);
    if(ret == 0){
        string = json_string(msg.message);
        json_object_set_new(masterobj, "cmd", string);
        }
    else{
*/
        string = json_string(msg.message);
        json_object_set_new(masterobj, "message", string);
        string = json_string("default");
        json_object_set_new(masterobj, "room", string);
//        }
}

void message_printer(json_t *masterobj){
  json_t *rec_message, *rec_username;
    char string_resp[1000];
    char username[255];
    if((rec_message = json_object_get(masterobj, "message"))!= NULL){
      rec_username = json_object_get(masterobj, "username");
        strcpy(string_resp, json_string_value(rec_message));
	strcpy(username, json_string_value(rec_username));
	printf("%s : %s\n",username, string_resp);
	fflush(stdout);
    }
}

void user_login(user_s *usr){
    printf("Username: ");
    user_input(usr->username);
    printf("Password: ");
    user_input(usr->password);
}

void serialize_username(json_t *masterobj, user_s *usr ){
    json_t *string;
    string = json_string(usr->username);
    //printf("json string");
    json_object_set(masterobj, "username", string);
}

void serialize_password(json_t *masterobj, user_s *usr ){
    json_t *string;
    string = json_string(usr->password);
    json_object_set(masterobj, "password", string);
}

void serialize_cmd(json_t *masterobj, char *cmd ){
    json_t *string;
    string = json_string(cmd);
    json_object_set(masterobj, "cmd", string);
}

/* string_convert(std::string s,char* msg[20]){
    int i = 0, j = s.size();
    for(i;i <= j;i++){
        msg[i] = s[i];
    }
    //printf("%s", msg);
}*/
