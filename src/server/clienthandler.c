#include "clienthandler.h"
#include <pthread.h>
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
pthread_mutex_t clientArrayMutex = PTHREAD_MUTEX_INITIALIZER;

int init_clientHandler( ){
  
}

int add_Client(){
  
  
}

int remove_Client(){
  
  
}

void *handle( void *args ){

  clients_t *client =  (clients_t * )args ;
  char* messagepointer;
  json_tokener *json_tok;
  json_object *recieved_obj;
  printf("started handling\n");
  fflush(stdout);
  messagepointer=ReadClientMessage(client->socket);
  if((recieved_obj = json_tokener_parse(messagepointer))!=NULL){
    /*TODO handle recieved json   */
    
    printf("recieved json:  %s\n", messagepointer);
    fflush(stdout);
    SerializableMessage_t response= {client, "server response"}; 
    pthread_t id;
    printf("creating writethread\n");
    pthread_create(&id, NULL, &write_to_client, &response); //create writethread  
    pthread_join(id,NULL );
    
    free(messagepointer); 
  }
  else{
    #if DEBUG
    printf("p is :%p \n recieved not json:  %s", messagepointer,messagepointer);
    fflush(stdout); 
    #endif
    free(messagepointer); 
  
  }
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
  
  
  WriteServerMessage(&sermes, p->client->socket);
  pthread_exit(&val);

}


char* ReadClientMessage( int socket){
  int tmp_buf=0;
  char* p;
  read(socket, &tmp_buf, sizeof(int)); 
  p = (char *) malloc(tmp_buf+1);
  read(socket, p, tmp_buf);
  fflush(stdout);
  return p;
}

 void WriteServerMessage( SerializedMessage_t *message, int socket){

  write(socket, &(message->size), sizeof(int));
  write(socket, message->jsonstring, message->size);
}
