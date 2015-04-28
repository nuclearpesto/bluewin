#ifndef CLIENTHANDLER  

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
  char message[255], roomname[255] ;
  
};typedef struct SerializableMessage SerializableMessage_t;

struct SerializedMessage{
  char jsonstring[JSON_MAX_LENGTH];
  int size;
  
};typedef struct SerializedMessage SerializedMessage_t;

int init_clientHandler(); // shall be run once uppon a thread entering handle
void add_Client(int socket, stack *s); 
int remove_Client(); //pushes disconnected client index on to free space stack
void *write_to_client(void *args); //writeloop, takes a struct containing a jsonstring and the length of that string plus controll options. then writes to specific client

char* read_client_message(int socket);
void write_server_message(SerializedMessage_t *message, int socket);
void *handle( void *args ); //main handle loop, a thread will read continously from the appropriate socket specified in args until there is a message and then act accordingly.
void write_to_room(char* roomname, SerializedMessage_t * sermes, clients_t * sender);


extern clients_t clientsArr[]; 
extern stack availableClientNr;
extern pthread_mutex_t clientsStackMutex;
extern pthread_t threadIds [];

#define CLIENTHANDLER
#endif
