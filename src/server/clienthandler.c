#include <pthread.h>
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "server.h"
#include "clienthandler.h"
int init_clientHandler( ){
  
}

void * new_list_item(unsigned int size){
  void *p;
  printf("allocating new client\n");
  fflush(stdout);
  pthread_mutex_lock(&allocationMutex);
  p= malloc(size);
  pthread_mutex_unlock(&allocationMutex);
  return p;
}

void add_Client(int socket){
  
  clients_t *p = (clients_t *)new_list_item(sizeof(clients_t));
  printf("allocated new client\n");
  fflush(stdout);
  p->next = NULL;
  p->socket = socket;
  p->inet_addr = 0;
  p->thread_id = 0;
  printf("initialized clientt\n");
  fflush(stdout);
  pthread_mutex_lock(&clientsStackMutex);
  clients_t *prev = find_last_client(&clientList);
  prev->next = p;
  p->prev = prev;
  pthread_mutex_unlock(&clientsStackMutex);
  pthread_create(&(p->thread_id), NULL, handle, p);
  
}

clients_t *find_last_client(clients_t *client){
  clients_t *current = client;
  while(current->next!=NULL){
    current = current->next;
  }
  return current;
}

clients_t *find_first_client(clients_t *client){
  clients_t * current = client;
  while(current->prev!=NULL){
    current = current->prev;
  }
  return current;
}

int remove_Client(clients_t *client){
  clients_t *prev, *next;
  int done = 0;
  pthread_mutex_lock(&clientsStackMutex);
  prev = client->prev;
  next = client->next;
  prev->next = next;
  pthread_mutex_unlock(&clientsStackMutex);
  while(!done){
    pthread_mutex_lock(&allocationMutex);
    if( pthread_mutex_trylock(&(client->mutex))==0){
      done=1;
    }
    else{
      pthread_mutex_unlock(&allocationMutex);
    }
  }
  free(client);
  pthread_mutex_unlock(&allocationMutex);
}

void *handle( void *args ){

  clients_t *client =  (clients_t * )args ;
  char* messagepointer;
  json_tokener *json_tok;
  json_object *recieved_obj;
  json_object *recv_json_cmd, *recv_json_message;
  printf("started handling\n");
  fflush(stdout);
  while( (messagepointer=read_client_message(client->socket))!=NULL){
      if((recieved_obj = json_tokener_parse(messagepointer))!=NULL){
	
	printf("recieved json:  %s\n", messagepointer);
	fflush(stdout);
	if(json_object_object_get_ex(recieved_obj, "cmd", &recv_json_cmd)){
	  if(strcmp("exit", json_object_get_string(recv_json_cmd))==0){
	    
	    remove_Client(client);  
	    pthread_exit(0);
	  }
	  else if(strcmp("msg", json_object_get_string(recv_json_cmd)) == 0){
	    if(json_object_object_get_ex(recieved_obj, "message", &recv_json_cmd)){
	      SerializableMessage_t response;
	      response.client = client;
	      strcpy(response.message, json_object_get_string(recv_json_cmd));
	      pthread_t id;
	      printf("creating writethread\n");
	      pthread_create(&id, NULL, &write_to_client, &response); //create writethread  
	      pthread_join(id,NULL );
	      free(messagepointer);
	    }
	  }
	}	
      }
  }
  remove_Client(client);
  int retval = 1;
  pthread_exit(&retval);
}

void *write_to_client(void *args){

    printf("gonna write\n");
    
  SerializableMessage_t *p  = (SerializableMessage_t *)args;
  json_object *messageobj = json_object_new_object();
   
  json_object *usn = json_object_new_string((p->client->username)); 
  json_object *mes = json_object_new_string((p->message)); 
  json_object *chrom = json_object_new_string("null");
  json_object *fromserv = json_object_new_int(1); 
  
  json_object_object_add(messageobj, "fromserver", fromserv );
  json_object_object_add(messageobj, "username", usn );
  json_object_object_add(messageobj, "chroom", chrom );
  json_object_object_add(messageobj, "message", mes );
  
  printf("created jsonobj\n");
  fflush(stdout);
  const char *json_string = json_object_to_json_string(messageobj);
		   
  int val = 1;
  
  printf("jsonstr size %d\ncontains %s", sizeof(json_string), json_string);
  SerializedMessage_t sermes;
  strcpy (sermes.jsonstring, json_string);
  sermes.size= strlen(sermes.jsonstring)+1;
  
  
  write_server_message(&sermes, p->client->socket);
  pthread_exit(&val);

}


char* read_client_message( int socket){
  int tmp_buf=0;
  char* p;
  printf("reading");
  fflush(stdout);
  if( read(socket, &tmp_buf, sizeof(int))>0){ 
    p = (char *) malloc(tmp_buf+1);
    read(socket, p, tmp_buf);
    
    return p;
  }
  printf("returning null\n");
  fflush(stdout);
  return NULL;
}

 void write_server_message( SerializedMessage_t *message, int socket){

  write(socket, &(message->size), sizeof(int));
  write(socket, message->jsonstring, message->size);
}
