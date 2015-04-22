#ifndef JSON_MAX_LENGTH
#define JSON_MAX_LENGTH 1000
#endif
struct Clients {
  struct Clients *next, *prev;
  int inet_addr, socket;
  char username[255];
  pthread_t thread_id;
  pthread_mutex_t mutex;
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

void add_Client(int socket);// adds a new client at the end of the global clientlist 
int remove_Client(); //frees disconnected client and adjusts previous client in list pointer to the next pointer
void *write_to_client(void *args); //writeloop, takes a struct containing a jsonstring and the length of that string plus controll options. then writes to specific client
void *new_list_item(unsigned int size); //allocates memory for a new listitem with mutex
char* read_client_message(int socket); // reads a message from the clientsocket
void write_server_message(SerializedMessage_t *message, int socket); //writes a serialized message to the lcient socket
void *handle( void *args ); //main handle loop, a thread will read continously from the appropriate socket specified in args until there is a message and then act accordingly.

clients_t *find_last_client(clients_t *client); // returns pointer to the last client in the global client list;
clients_t *find_first_client(clients_t *client); // returns pointer to the first client in the clientlist
extern clients_t clientsArr[]; 
extern stack availableClientNr;
extern pthread_mutex_t clientsStackMutex;
extern pthread_t threadIds [];
extern pthread_mutex_t allocationMutex;
extern clients_t clientList;
#ifndef CLIENTHANDLER  
#define CLIENTHANDLER
#endif
