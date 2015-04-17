#include <pthread.h>
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "server.h"
#include "clienthandler.h"
int init_clientHandler( ){
  
}

void add_Client(int socket, stack *s){

  pthread_mutex_lock(&clientsStackMutex);
  int count = pop(s);
  clientsArr[count].inet_addr = 0;
  clientsArr[count].socket = socket;
  pthread_create(&threadIds[count], NULL, &handle, &clientsArr[count]);
  pthread_mutex_unlock(&clientsStackMutex);
  
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
  printf("index is : %d\n", index);
  pthread_mutex_lock(&clientsStackMutex);
  push(&availableClientNr, index);
  pthread_mutex_unlock(&clientsStackMutex);
  return 1;
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
	  if(strcmp("exit", json_object_get_string(recv_json_cmd))){
	    remove_Client(client);  
	    pthread_exit(0);
	  }
	  else if(strcmp("msg", json_object_get_string(recv_json_cmd))){
	    	SerializableMessage_t response= {client, "server response"}; 
		pthread_t id;
		printf("creating writethread\n");
		pthread_create(&id, NULL, &write_to_client, &response); //create writethread  
		pthread_join(id,NULL );
		free(messagepointer); 
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
  sermes.size= sizeof(sermes.jsonstring);
  
  
  write_server_message(&sermes, p->client->socket);
  pthread_exit(&val);

}


char* read_client_message( int socket){
  int tmp_buf=0;
  char* p;
  if( read(socket, &tmp_buf, sizeof(int))<-1){ 
    p = (char *) malloc(tmp_buf+1);
    read(socket, p, tmp_buf);
    
    return p;
  }
  return NULL;
}

 void write_server_message( SerializedMessage_t *message, int socket){

  write(socket, &(message->size), sizeof(int));
  write(socket, message->jsonstring, message->size);
}
