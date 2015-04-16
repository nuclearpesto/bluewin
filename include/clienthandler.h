#ifndef JSON_MAX_LENGTH
#define JSON_MAX_LENGTH 1000
#endif
struct Clients {
  int inet_addr, socket;
  char username[255];
  

};
typedef struct Clients clients_t;

struct SerializableMessage{
  clients_t *client;
  char message[255] ;
  
};typedef struct SerializableMessage SerializableMessage_t;

struct SerializedMessage{
  char jsonstring[JSON_MAX_LENGTH];
  int size;
  
};typedef struct SerializedMessage SerializedMessage_t;

int init_clientHandler(); // shall be run once uppon a thread entering handle
int add_Client(); //unused
int remove_Client(); //shall be called on exit recieve form client
void *write_to_client(void *args); //writeloop, takes a struct containing a jsonstring and the length of that string plus controll options. then writes to specific client

char* ReadClientMessage(int socket);
void WriteServerMessage(SerializedMessage_t *message, int socket);
void *handle( void *args ); //main handle loop, a thread will read continously from the appropriate socket specified in args until there is a message and then act accordingly.
extern clients_t clients_arr[]; 


#ifndef CLIENTHANDLER  
#define CLIENTHANDLER
#endif
