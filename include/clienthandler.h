#ifndef CLIENTHANDLER  

#ifndef JSON_MAX_LENGTH
#define JSON_MAX_LENGTH 1000
#endif
struct Clients {
  IPaddress ip;
  TCPsocket socket;
  bool loggin;
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

void add_Client(TCPsocket socket, stack *s); //get the next available place in the array and connect the new client 
int remove_Client(); //pushes disconnected client index on to free space stack and closes client socket also cleeans up the rooms for clients
int write_to_client(void *args); //writeloop, takes a struct containing a jsonstring and the length of that string plus controll options. then writes to specific client

char* read_client_message(TCPsocket socket); //read a jsonstring from a client socket
void write_server_message(SerializedMessage_t *message, TCPsocket socket); //write a jsonstring to socket
int handle( void *args ); //main handle loop, a thread will read continously from the appropriate socket specified in args until there is a message and then act accordingly.
void write_to_room(char* roomname, SerializedMessage_t * sermes, clients_t * sender); // find a room and write to everyone except the sender
void handle_login(json_t * recieved_obj, clients_t *client);
void handle_message(json_t *recv_obj, clients_t *client);
void handle_add_user(json_t *recieved_obj, clients_t *client);
void handle_del_user(json_t *recieved_obj, clients_t *client);
void handle_get_rooms(json_t * recieved_obj, clients_t *client);
void handle_send_file(json_t * recieved_obj, clients_t *client);
void handle_logout(clients_t *client);
void handle_add_call(json_t * recieved_obj, clients_t *client);
void handle_switch_room(json_t * recieved_obj, clients_t *client);
void handle_add_rooms(json_t * recieved_obj, clients_t *client);
void handle_delete_room(json_t * recieved_obj, clients_t *client);

extern clients_t clientsArr[]; 
extern stack availableClientNr;
extern SDL_mutex *clientsStackMutex;
extern SDL_Thread *threadIds;

#define CLIENTHANDLER
#endif
