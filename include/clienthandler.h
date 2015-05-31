/*Author David Boeryd*/
#ifndef CLIENTHANDLER  

#ifndef JSON_MAX_LENGTH
#define JSON_MAX_LENGTH 100000
#endif
#define USERNAME_LEN 255
#define ROOM_NAME_SIZE 255

struct Clients {
IPaddress ip;
TCPsocket socket;
bool loggin;
char username[USERNAME_LEN], currentRoom[ROOM_NAME_SIZE];

};
typedef struct Clients clients_t;

struct SerializableMessage{
  clients_t *client;
  char message[JSON_MAX_LENGTH], roomname[ROOM_NAME_SIZE] ;
  
};typedef struct SerializableMessage SerializableMessage_t;

struct SerializedMessage{
  char jsonstring[JSON_MAX_LENGTH];
  int size;
  
};typedef struct SerializedMessage SerializedMessage_t;

void add_Client(TCPsocket socket, stack *s); //get the next available place in the array and set client->socket to socket if no available places prints error and closes connection;
int remove_Client(); //pushes disconnected client index on to free space stack and closes client socket also calls leave_room() for client
int write_to_client(void *args); // takes a serializable message, serializes to json then calls write to room

char* read_client_message(TCPsocket socket); //read a jsonstring from a client socket
void write_server_message(SerializedMessage_t *message, TCPsocket socket); //write a jsonstring to socket
int handle( void *args ); //main handle loop, a thread will read continously from the appropriate socket specified in args until there is a message and then act accordingly.
void write_to_room(char* roomname, SerializedMessage_t * sermes, clients_t * sender); // find a room and write to everyone except the sender
void handle_login(json_t * recieved_obj, clients_t *client); //runs login, serializes message to clint and writes it
void handle_message(json_t *recv_obj, clients_t *client);//runs write_server_message, serializes message to client and calls write_server_message;
void handle_add_user(json_t *recieved_obj, clients_t *client);//runs add user and serializes a messsage to the client indicating if it was successfull
void handle_del_user(json_t *recieved_obj, clients_t *client);//runs del user and serializes a messsage to the client indicating if it was successfull
void handle_get_rooms(json_t * recieved_obj, clients_t *client);//runs get_rooms and serializes a result to the client and wirtes it
void handle_send_file(json_t * recieved_obj, clients_t *client);//currently does nothing
void handle_logout(clients_t *client);//sets client->loggedin to false and closes the connection to the client. also runs leave room for the client.
void handle_add_call(json_t * recieved_obj, clients_t *client);//checks for the room to send data and checks if there is a key named audio the runs write_to_room. if either room or audio is not found doea nothing
void handle_switch_room(json_t * recieved_obj, clients_t *client);// checks for the room key and runs switch room with the value taken from room
void handle_add_room(json_t * recieved_obj, clients_t *client);//checks for room key and runs create_room if found
void handle_delete_room(json_t * recieved_obj, clients_t *client);//checks for room key and runs delete room if found
void handle_get_users_in_room(json_t * recieved_obj, clients_t *client); //runs get_clients in room if room key is found
SerializedMessage_t  create_serialized_message(char *json_string); //returns a new struct serialized message

extern clients_t clientsArr[]; 
extern stack availableClientNr;
extern SDL_mutex *clientsStackMutex;
extern SDL_Thread *threadIds;
extern SDL_mutex* writeMutex;

#define CLIENTHANDLER
#endif
