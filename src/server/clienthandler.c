
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_net.h>
#include <jansson.h>
#include <stdbool.h>
#include "misc.h"
#include "server.h"
#include "clienthandler.h"
#include "rooms.h"
#include "crypt.h"
#include "users.h"
#include "debug.h"

int init_clientHandler( ){

}


void add_Client(TCPsocket socket, stack *s){

  SDL_LockMutex(clientsStackMutex);
  int count = pop(s);
  D(printf("creating client with index %d\n", count));
  fflush(stdout);
  if(count >=0){
    clientsArr[count].socket = socket;
    threadIds = SDL_CreateThread(handle, "handle", (void *) &clientsArr[count]) ;
    SDL_DetachThread(threadIds);
  }
  else{
    perror("no available client slots");
      close(socket);
  }
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
  D(printf("removing client with index : %d\n", index));
  SDLNet_TCP_Close(client->socket);
  SDL_UnlockMutex(clientsStackMutex);
}

int handle( void *args ){

  clients_t *client =  (clients_t * )args ;
  char* messagepointer;
  json_t *recieved_obj;
  json_t *recv_json_cmd, *recv_json_message;
  json_error_t jsonError;
  D(printf("started handling\n"));
  fflush(stdout);
  while( (messagepointer=read_client_message(client->socket))!=NULL){
   // D(printf("got something "));
	  //DEKRYPTERA!!!!
    //D(printf("recieved this:  %s\n", messagepointer));
		fflush(stdout);
		if((recieved_obj = json_loads(messagepointer,0, &jsonError))!=NULL){
		  //D(printf("recieved json:  %s\n", messagepointer));
			fflush(stdout);
			if((recv_json_cmd= json_object_get(recieved_obj, "cmd"))!=NULL){
			   if(strcmp("msg", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("found msg\n"));
				  handle_message(recieved_obj, client);
				  fflush(stdout);
			  }
			  else if(strcmp("login", json_string_value(recv_json_cmd)) == 0  && client->loggin==false){
			    D(printf("found login\n"));
				  handle_login(recieved_obj, client);
				  fflush(stdout);
			  }
			  else if(strcmp("add user", json_string_value(recv_json_cmd)) == 0){
			    D(printf("found add user\n"));
				  handle_add_user(recieved_obj, client);
				  fflush(stdout);
			  }

			  else if(strcmp("logout",json_string_value(recv_json_cmd)) == 0 && client->loggin ==true){
			    D(printf("found logou\n"));
				  handle_logout( client);
				  fflush(stdout);
				  return 0;
			  }

			  else if(strcmp("get rooms", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("found get rooms\n"));
				  handle_get_rooms(recieved_obj, client);
				  fflush(stdout);
			  }
			  else if(strcmp("get users in room", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("found get users in room rooms\n"));
				  handle_get_users_in_room(recieved_obj, client);
				  fflush(stdout);
			  }
			  else if(strcmp("add room", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("found add room\n"));
				  handle_add_room(recieved_obj, client);
				  fflush(stdout);
			  }
			  else if(strcmp("delete room", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("found delete room\n"));
			    handle_delete_room(recieved_obj, client);
			    fflush(stdout);
			  }
			  else if(strcmp("switch room", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("found switch room\n"));
				  handle_switch_room(recieved_obj, client);
				  fflush(stdout);
			  }
			  else if(strcmp("send file", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("foudn send file\n"));
			  	  handle_send_file(recieved_obj, client);
			  	  fflush(stdout);
			  }
			  else if(strcmp("add call", json_string_value(recv_json_cmd)) == 0 && client->loggin == true){
			    D(printf("found call\n"));
			  	  handle_add_call(recieved_obj, client);
			  	  fflush(stdout);
			  }
			}
		}
		free(recieved_obj);
		free(messagepointer);
  }
  handle_logout(client);
  return 1;
}

void handle_message(json_t *recieved_obj, clients_t *client){
	json_t *recv_json_cmd;
	SDL_Thread *id;
	if(recv_json_cmd = json_object_get(recieved_obj, "message")){
	  SerializableMessage_t *response =(SerializableMessage_t *) malloc(sizeof(SerializableMessage_t));
	  response->client = client;
	  strcpy(response->message, json_string_value(recv_json_cmd));
	  D(printf("copied string\n"));
	  fflush(stdout);
	  recv_json_cmd=json_object_get(recieved_obj, "room");
	  strcpy( response->roomname, json_string_value(recv_json_cmd));
	  //D(printf("room in serializable message is %s\n", response.roomname));
	  D(printf("creating writethread\n"));
	  printf("roomname %s", response->roomname);
	  id= SDL_CreateThread(write_to_client,"writer", response); //create writethread
	  SDL_DetachThread(id);
	/*UNNECECARY TO CREATE NEW THREAD EVERY TIME, TODO make it run write to server from here*/
	}
}

void handle_login(json_t * recieved_obj, clients_t *client){
	json_t *writeobj, * username, * password, *json_login_val;
	bool success = false;
	char *strusn, *strpass;
	username = json_object_get(recieved_obj, "username");
	password = json_object_get(recieved_obj,"password");
	if(username!=NULL && password!=NULL){
		strusn = json_string_value(username);
		strpass = json_string_value(password);

		success =login(strusn, strpass);
	}

	if(success){
	  client->loggin=success;
	  strcpy(client->username, json_string_value(username));
	  join_room("default", client);
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
	free(writeobj);




}

void handle_switch_room(json_t * recieved_obj, clients_t *client){
	json_t *writeobj, *room;
	bool success = false;
	char *strroom;
	room = json_object_get(recieved_obj, "room");
	if(room!=NULL){
	  strroom = json_string_value(room);
	  switch_room( strroom, client);
	}
	//writeobj = json_object();
	//TODO RETURN SOMETHING TO CLIENT TO LET THEM KNOW IT WORKED
}


void handle_get_users_in_room(json_t * recieved_obj, clients_t *client){
  json_t *writeobj, *usersarr, *json_success, *room ;
  bool success = false;
  char *strroom;
  writeobj = json_object();
  D(printf("getting users from room\n"));
  fflush(stdout);
  room = json_object_get(recieved_obj,"room");
  strroom = json_string_value(room);
  if((usersarr=get_users_in_room(strroom))!=NULL){
    json_object_set_new(writeobj,"usersArr", usersarr);
    json_success = json_boolean(1);
    json_object_set_new(writeobj,"get users in room", json_success);

  }
  else{
    json_success = json_boolean(0);
    json_object_set_new(writeobj,"get users in room", json_success);
  }
  D(printf("writeobj = %p\n ", writeobj));
    fflush(stdout);
  char * jsonstr = json_dumps(writeobj, 0);
    D(printf("dumped\n "));
    fflush(stdout);
  SerializedMessage_t sermes = create_serialized_message(jsonstr);
    D(printf("createdsermes\n "));
    fflush(stdout);
  write_server_message(&sermes, client->socket);
   D(printf("wrote\n "));
    fflush(stdout);
	free(writeobj);
}


void handle_get_rooms(json_t * recieved_obj, clients_t *client){
  json_t *writeobj, *rooms, *json_success;
  bool success = false;
  char *strroom;
  writeobj = json_object();
  D(printf("getting rooms\n"));
  fflush(stdout);
  if((rooms=find_existing_rooms(MAX_ROOMS))!=NULL){
    json_object_set_new(writeobj,"roomsArr", rooms);
    json_success = json_boolean(1);
      json_object_set_new(writeobj,"get rooms", json_success);
      }
  else{
    json_success = json_boolean(0);
    json_object_set_new(writeobj,"get rooms", json_success);
  }
  char * jsonstr = json_dumps(writeobj, 0);
  SerializedMessage_t sermes = create_serialized_message(jsonstr);
  write_server_message(&sermes, client->socket);
	free(writeobj);
 }



void handle_add_room(json_t * recieved_obj, clients_t *client){

  json_t *writeobj, *room;
  bool success = false;
  char *strroom;
  room = json_object_get(recieved_obj, "room");
  if(room!=NULL){
    strroom = json_string_value(room);
    add_room(strroom);
   // switch_room( strroom, client);

  }

}

void handle_delete_room(json_t * recieved_obj, clients_t *client){

  json_t *writeobj, *room;
  bool success = false;
  char *strroom;
  room = json_object_get(recieved_obj, "room");
  if(room!=NULL){
    strroom = json_string_value(room);
    delete_room(strroom);
  }

}
void handle_add_call(json_t * recieved_obj, clients_t *client){

	json_t* audio, *room, *call;
	char* strroom;
	room=json_object_get(recieved_obj, "room");
	call = json_string("audio");
	if(room!=NULL){
		strroom = json_string_value(room);
		json_object_del(recieved_obj, "cmd");
		json_object_set_new(recieved_obj,"call", call);
		SerializedMessage_t sermes = create_serialized_message(json_dumps(recieved_obj, 0));
		D(printf("gonna write this %s, n", sermes.jsonstring));
		write_to_room(strroom, &sermes, client);

	}



}

void handle_send_file(json_t * recieved_obj, clients_t *client){

  //TODO IMPLEMENT


}
void handle_add_user(json_t *recieved_obj, clients_t *client){
	json_t *writeobj, *password, *username, *json_created_val;
	char * strpass, *strusn;
	bool success;

	username = json_object_get(recieved_obj, "username");
	password = json_object_get(recieved_obj,"password");
	if(username!=NULL && password!=NULL){
		strusn = json_string_value(username);
		strpass = json_string_value(password);
		success =add_user(strusn, strpass);
	}
	writeobj = json_object();
	json_created_val = json_boolean(success);
	json_object_set_new(writeobj, "add user", json_created_val);
	json_object_set_new(writeobj, "username",username);
	char * jsonString = json_dumps(writeobj, 0);

	D(printf("gonnar write tis response to client:%s\n", jsonString));
	SerializedMessage_t sermes = create_serialized_message(jsonString);
	write_server_message(&sermes, client->socket);
	free(writeobj);

}

void handle_del_user(json_t *recieved_obj, clients_t *client){
	json_t *writeobj, *password, *username, *json_deleted_val;
	char * strpass, *strusn;
	bool success=false;
	username = json_object_get(recieved_obj, "username");
	password = json_object_get(recieved_obj,"password");
	//printf("username befor login func is:  %s \n", strusn);
	if(username!=NULL && password!=NULL){
		strusn = json_string_value(username);
		strpass = json_string_value(password);
		success =del_user(strusn, strpass);
	}
	writeobj = json_object();
	json_deleted_val = json_boolean(success);
	json_object_set_new(writeobj, "del user", json_deleted_val);
	json_object_set_new(writeobj, "username",username);
	char * jsonString = json_dumps(writeobj, 0);
	SerializedMessage_t sermes = create_serialized_message(jsonString);
	write_server_message(&sermes, client->socket);
	free(writeobj);

}

void handle_logout(clients_t *client){
  client->loggin=false;
  leave_room( client);
  remove_Client(client);

    /*TODO MAKE CLIENT SEND RESPONS BEFORE LOGGING OUT*/

}

SerializedMessage_t  create_serialized_message(char *json_string){
  SerializedMessage_t sermes;
  strcpy(sermes.jsonstring, json_string);
  sermes.size = strlen(json_string);
  return sermes;

}



int write_to_client(void *args){

  D(printf("gonna write\n"));

	//printf("got message");
  SerializableMessage_t *p  = (SerializableMessage_t *)args;
  char roomname[ROOM_NAME_SIZE+1];
// printf("in write_to_client before copy roomname is %s\n", p->roomname);
 strcpy(roomname, p->roomname); //for some reason when the roomname json pointer is created p->roomname is emptied
  //this is a short term solution
  json_int_t x = 1;
  json_t *messageobj = json_object();
  json_t *usn = json_string((p->client->username));
  json_t *mes = json_string((p->message));
  json_t *chrom = json_string(p->roomname);
 // printf("in write_to_client before json roomname is %s\n", roomname);

  json_object_set_new(messageobj,"username", usn );
  json_object_set_new(messageobj, "chroom", chrom );
  json_object_set_new(messageobj, "message", mes );

  D(printf("in write_to_client roomane is %s\n", roomname));
  D(printf("created jsonobj\n"));
  fflush(stdout);
  char *json_string = json_dumps(messageobj, 0);

  D(printf("jsonstr size %d\ncontains %s\n", sizeof(json_string), json_string));
  SerializedMessage_t sermes = create_serialized_message(json_string);
  write_to_room(roomname, &sermes, p->client);
  //printf("wrote %s\n", sermes.jsonstring);
  free(p);
  //write_server_message(&sermes, p->client->socket);

	return 1;
}
void write_to_room(char* roomname, SerializedMessage_t * sermes, clients_t * sender){ //write to everyone in a room except sender
  //KRYPTERA
  //encrypt_Handler(sermes);
  int index=find_index_of_room(roomname, THREAD_COUNT);
  D(printf("found room index %d\n", index));
  if(index>-1){
    int i =0;
    D(printf("currentCons of room %d is %d\n", index, roomsArr[index].nrOfCurrentConns));
    SDL_LockMutex(roomsStackMutex);
    for(i =0; i<roomsArr[index].nrOfCurrentConns; i++){
      if(roomsArr[index].connected[i]!=sender){
		D(printf("%p and %p are not same", sender, roomsArr[index].connected[i]));
		write_server_message(sermes,roomsArr[index].connected[i]->socket ); //ändra sermes till krypterad text variabel
      }
      else{
	D(printf("same as sendere\n"));
      }
    }
    SDL_UnlockMutex(roomsStackMutex);
  }
}



char* read_client_message( TCPsocket socket){
	//inspired by http://stackoverflow.com/questions/21579867/variable-length-message-over-tcp-socket-in-c answer 1 written by user John Dibling
 int tmp_buf=0, max = JSON_MAX_LENGTH;
  char* p;
  D(printf("reading\n"));
  fflush(stdout);
  if( SDLNet_TCP_Recv(socket, &tmp_buf, sizeof(int))>0){
    if(tmp_buf>0){
		if(tmp_buf>max){
			tmp_buf=max;
		}
      p = (char *) malloc(tmp_buf+1);
      if(p==NULL){
		  printf("failed to allocate mamory for p");
		exit(1);
	  }
	  D(printf("gonna read %d bytes\n", tmp_buf));
      tmp_buf = SDLNet_TCP_Recv(socket, p, tmp_buf);
      D(printf("read %d bytes\n", tmp_buf));
      decrypt_Handler(p, tmp_buf);
      *(p+tmp_buf)='\0';
      return p;
    }
  }
  D(printf("returning null\n"));
  fflush(stdout);
  return NULL;
}

 void write_server_message( SerializedMessage_t *message, TCPsocket socket){
  encrypt_Handler(message);
	//inspired by http://stackoverflow.com/questions/21579867/variable-length-message-over-tcp-socket-in-c answer 1 written by user John Dibling
	//printf("trying to lock mutex\n");
	SDL_LockMutex(writeMutex);
	//printf("locked mutex\n");
	//printf("gonna write, size of message = %d,  %s\n", message->size,message->jsonstring);
  SDLNet_TCP_Send(socket, &(message->size), sizeof(int));
  SDLNet_TCP_Send(socket, message->jsonstring, message->size);
	SDL_UnlockMutex(writeMutex);
 }
