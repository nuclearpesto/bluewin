
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_thread.h>
#include <jansson.h>
#include <stdbool.h>
#include "misc.h"
#include "server.h"
#include "clienthandler.h"
#include "rooms.h"
#include "crypt.h"
#include "users.h"

int init_clientHandler( ){

}


void add_Client(TCPsocket socket, stack *s){

  SDL_LockMutex(clientsStackMutex);
  int count = pop(s);
  printf("creating client with index %d\n", count);
  fflush(stdout);
  //clientsArr[count].inet_addr = 0;
  clientsArr[count].socket = socket;
  join_room("default", &clientsArr[count]);
  threadIds = SDL_CreateThread(handle, "handle", (void *) &clientsArr[count]) ;
  SDL_DetachThread(threadIds);
  SDL_UnlockMutex(clientsStackMutex);

}

int find_index_of_client(clients_t *client){
  int i =0;
  for(i=0; i<THREAD_COUNT; i++){
    if(clientsArr[i].socket==client->socket){
      break;
    }
  }
  return i;
}



int remove_Client(clients_t *client){
  int index=0;
  index = find_index_of_client(client);
  SDL_LockMutex(clientsStackMutex);
  push(&availableClientNr, index);
  printf("removing client with index : %d\n", index);
  leave_room("default", client);
  SDLNet_TCP_Close(client->socket);

  SDL_UnlockMutex(clientsStackMutex);
}

int handle( void *args ){

  clients_t *client =  (clients_t * )args ;
  char* messagepointer;
  json_t *recieved_obj;
  json_t *recv_json_cmd, *recv_json_message;
  json_error_t jsonError;
  printf("started handling\n");
  fflush(stdout);
  while( (messagepointer=read_client_message(client->socket))!=NULL){
	  printf("got something ");
	  //DEKRYPTERA!!!!
	  //decrypt_Handler(messagepointer);
	  printf("recieved this:  %s\n", messagepointer);
		fflush(stdout);
		if((recieved_obj = json_loads(messagepointer,0, &jsonError))!=NULL){
			printf("recieved json:  %s\n", messagepointer);
			fflush(stdout);
			if((recv_json_cmd= json_object_get(recieved_obj, "cmd"))!=NULL){
			  if(strcmp("exit\n", json_string_value(recv_json_cmd))==0){
				remove_Client(client);
				return 1;
			  }
			  else if(strcmp("msg", json_string_value(recv_json_cmd)) == 0){
				  printf("found msg");
				  handle_message(recieved_obj, client);
				  fflush(stdout);
			  }
			  else if(strcmp("login", json_string_value(recv_json_cmd)) == 0){
				  printf("found login");
				  handle_login(recieved_obj, client);
				  fflush(stdout);
			  }
			}
		}
		free(messagepointer);
	}
  remove_Client(client);
  return 1;
}

void handle_message(json_t *recieved_obj, clients_t *client){
	json_t *recv_json_cmd;
	if(recv_json_cmd = json_object_get(recieved_obj, "message")){
			  SerializableMessage_t response;
			  response.client = client;
			  strcpy(response.message, json_string_value(recv_json_cmd));
			  printf("copied string");
			  fflush(stdout);
			  recv_json_cmd=json_object_get(recieved_obj, "room");
			  strcpy( response.roomname, json_string_value(recv_json_cmd));
			  SDL_Thread *id;
			  printf("creating writethread\n");
			  id= SDL_CreateThread(write_to_client,"writer", &response); //create writethread
			  SDL_DetachThread(id);
			}
}

void handle_login(json_t * recieved_obj, clients_t *client){
	json_t *writeobj, * username, * password, *json_login_val;
	bool success = false;
	char *strusn, *strpass;
	username = json_object_get(recieved_obj, "username");
	password = json_object_get(recieved_obj,"password");
	strusn = json_string_value(username);
	strpass = json_string_value(password);
	printf("username befor login func is:  %s \n", strusn);
	if(username!=NULL && password!=NULL){
		success =login(strusn, strpass);
	}
	client->loggin=success;
	if(success){
		strcpy(client->username, json_string_value(username));
	}
	writeobj = json_object();
	json_login_val = json_boolean(success);
	json_object_set(writeobj, "login", json_login_val);
	json_object_set(writeobj, "username",username);
	char * jsonString = json_dumps(writeobj, 0);
	SerializedMessage_t sermes;
	strcpy(sermes.jsonstring,jsonString);
	sermes.size = strlen(jsonString);
	write_server_message(&sermes, client->socket);





}


int write_to_client(void *args){

    printf("gonna write\n");

  SerializableMessage_t *p  = (SerializableMessage_t *)args;
	json_int_t x = 1;
  json_t *messageobj = json_object();

  json_t *usn = json_string((p->client->username));
  json_t *mes = json_string((p->message));
  json_t *chrom = json_string(p->roomname);
  json_t *fromserv = json_integer(x);

  json_object_set(messageobj, "fromserv", fromserv);
  json_object_set(messageobj,"username", usn );
  json_object_set(messageobj, "chroom", chrom );
  json_object_set(messageobj, "message", mes );

  printf("created jsonobj\n");
  fflush(stdout);
  const char *json_string = json_dumps(messageobj, 0);

  int val = 1;

  printf("jsonstr size %d\ncontains %s", sizeof(json_string), json_string);
  SerializedMessage_t sermes;
  strcpy (sermes.jsonstring, json_string);
  sermes.size= strlen(sermes.jsonstring)+1;

  write_to_room(p->roomname, &sermes, p->client);
  //write_server_message(&sermes, p->client->socket);

	return 1;
}
void write_to_room(char* roomname, SerializedMessage_t * sermes, clients_t * sender){
  //KRYPTERA
  //encrypt_Handler(sermes);
  int index=find_index_of_room(roomname, THREAD_COUNT);
  printf("found room index %d\n", index);
  int i =0;
  printf("currentCons of room %d is %d\n", index, roomsArr[index].nrOfCurrentConns);
  SDL_LockMutex(roomsStackMutex);
  for(i =0; i<roomsArr[index].nrOfCurrentConns; i++){
    if(roomsArr[index].connected[i]!=sender){
		printf("%p and %p are not same", sender, roomsArr[index].connected[i]);
      write_server_message(sermes,roomsArr[index].connected[i]->socket ); //ändra sermes till krypterad text variabel
    }
    else{
      printf("same as sendere\n");
    }
  }
  SDL_UnlockMutex(roomsStackMutex);
}



char* read_client_message( TCPsocket socket){
  int tmp_buf=0;
  char* p;
  printf("reading\n");
  fflush(stdout);
  if( SDLNet_TCP_Recv(socket, &tmp_buf, sizeof(int))>0){
    p = (char *) malloc(tmp_buf+1);
	printf("gonna read %d bytes\n", tmp_buf);
    tmp_buf = SDLNet_TCP_Recv(socket, p, tmp_buf);
    printf("read %d bytes\n", tmp_buf);
	*(p+tmp_buf)='\0';
    return p;
  }
  printf("returning null\n");
  fflush(stdout);
  return NULL;
}

 void write_server_message( SerializedMessage_t *message, TCPsocket socket){

  SDLNet_TCP_Send(socket, &(message->size), sizeof(int));
  SDLNet_TCP_Send(socket, message->jsonstring, message->size);
}
