#ifndef CLIENTHANDLER  

#ifndef JSON_MAX_LENGTH
#define JSON_MAX_LENGTH 1000
#endif
struct Clients {
  IPaddress ip;
  TCPsocket socket;
  SDL_Thread *handleThreadId;
  char username[255];
  
};
typedef struct Clients clients_t;

struct SerializableMessage{
  clients_t *client;
  char message[255], roomname[255] ;
  
};typedef struct SerializableMessage SerializableMessage_t;

struct SerializedMessage{
  char jsonstring[JSON_MAX_LENGTH];
  int size;
  
};typedef struct SerializedMessage SerializedMessage_t;

int init_clientHandler(); // shall be run once uppon a thread entering handle
void add_Client(TCPsocket socket, stack *s); 
int remove_Client(); //pushes disconnected client index on to free space stack
int write_to_client(void *args); //writeloop, takes a struct containing a jsonstring and the length of that string plus controll options. then writes to specific client

char* read_client_message(TCPsocket socket);
void write_server_message(SerializedMessage_t *message, TCPsocket socket);
int handle( void *args ); //main handle loop, a thread will read continously from the appropriate socket specified in args until there is a message and then act accordingly.
void write_to_room(char* roomname, SerializedMessage_t * sermes, clients_t * sender);


extern clients_t clientsArr[]; 
extern stack availableClientNr;
extern SDL_mutex *clientsStackMutex;
extern SDL_Thread *threadIds;

#define CLIENTHANDLER
#endif
