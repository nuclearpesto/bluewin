//
//  client.cpp
//  bluewinclient
//
//  Created by Rasmus Jansson on 17/04/15.
//  Copyright (c) 2015 Grupp 2 individer. All rights reserved.
//

#include "client.h"

#define READ_BUF_SIZE 1000

bool loginCheck = false;

IPaddress ip;

user_s usr;
//json_t *masterobj = json_object();


/*int mainTest(int argc, char *argv[])
{
        IPaddress ip;
        TCPsocket sd;
        if(argc<3){
            //printf("too few args , specify inetaddr and port as args");
            exit(1);
        }

        int t, len, i, e=0, port=atoi(argv[2]);

         char inet_adr[16]=argv[1];

        char str[READ_BUF_SIZE]={0};
        char r[READ_BUF_SIZE]={0};


        if(SDLNet_Init() < 0){
            //printf("stderr, SDLNet_Init: %s\n", SDLNet_GetError());
            exit(1);
        }


        //printf("Trying to connect...\n");
        if(SDLNet_ResolveHost(&ip, argv[1], port) < 0){
            f//printf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
            exit(1);
        }

        if(!(sd = SDLNet_TCP_Open(&ip))){
            f//printf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
            exit(1);
        }

        //printf("Connected.\n");
        fflush(stdout);
        char *string;
        user_s usr;
        int choice;

        int size = sizeof(string);
        Message_s msg;
        json_t *masterobj;

        masterobj = json_object();
        SDL_CreateThread(readThread, "reader", &sd);

        //printf("0: login\n 1: new user\n:");
         fscanf(stdin, "%d", &choice);
         switch(choice){
         case 0:
         clear_input();
         send_login(masterobj, &usr, sd);
         break;
         case 1:
         clear_input();
         add_user(masterobj, &usr, sd);
         send_login(masterobj, &usr, sd);
         break;
         }
        mainGui(nrRooms, &inputUsernameText, &inputPasswordText);
        string_convert(inputUsernameText, usr.username);
        string_convert(inputPasswordText, usr.password);
        send_login(masterobj, &usr, sd);
        //printf("Welcome!\n");
        while(1){
            mainGui(nrRooms, &inputUsernameText, &inputPasswordText);
            user_input(msg.message);
            if(msg.message == "exit\n"){
                exit(0);
            }
            serialize_message(masterobj, msg);
            serialize_cmd(masterobj, "msg");
            //printf("%s\n", string);
            write_to_server(masterobj, sd);
        }
    }
}*/

TCPsocket initClient( audiostruct_t * audiostruct, bool * createCheck, bool *loginCheck, json_t * globalUsersInRoomArr, json_t *globalRoomArr, json_t *messageArr, SDL_mutex *messageArrMutex, SDL_mutex * writeMutex){
    bool success = true;
    SDL_Thread *thread;
	TCPsocket sd = NULL;
	*loginCheck = false;
	*createCheck  =false;
	Readstruct readstruct;
	readstruct.globalUsersInRoomArr = globalUsersInRoomArr;
	readstruct.globalRoomArr = globalRoomArr;
	readstruct.messageArr = messageArr;
	readstruct.messageArrMutex = messageArrMutex;
	readstruct.createCheck = createCheck;
	readstruct.audiostruct	= audiostruct;
    //IPaddress ip;
    //TCPsocket sd;

    int port=5000;
    //char ipen[20]=("130.237.84.200");
    char ipen[20]=("127.0.0.1");

    printf("%s\n",ipen);
    if(SDLNet_Init() < 0){                                      //SDL Tutorial taken from
        printf("stderr, SDLNet_Init: %s\n", SDLNet_GetError()); //http://content.gpwiki.org/index.php/SDL:Tutorial:Using_SDL_net
        success=false;
    }else{
        printf("Trying to connect...\n");
        if(SDLNet_ResolveHost(&ip, ipen, port) < 0){
            fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
            success=false;
        }else{
            if(!(sd = SDLNet_TCP_Open(&ip))){
                fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
                success=false;
            }else{
				printf("socket before is : %d\n",sd);
				Readstruct *readstruct = (Readstruct*) malloc(sizeof(readstruct));
				readstruct->globalUsersInRoomArr = globalUsersInRoomArr;
				readstruct->messageArr = globalUsersInRoomArr;
				readstruct->globalRoomArr = globalRoomArr;
				readstruct->sd = sd;
				readstruct->loginCheck = loginCheck;
				readstruct->messageArrMutex = messageArrMutex;
				readstruct->audiostruct = audiostruct;
				thread=SDL_CreateThread(readThread, "reader", readstruct);
				//initialize audio ;
				init_sound(audiostruct);
				audiothreadstruct_t * audiothreadstruct = (audiothreadstruct_t*) malloc(sizeof(audiothreadstruct_t));
				audiothreadstruct->audiostruct = audiostruct;
				audiothreadstruct->socket = sd;
				audiothreadstruct->loggedInCheck = loginCheck;
				audiothreadstruct->writeMutex = writeMutex;
				//start microphone sender thread;
				thread=SDL_CreateThread(readthread, "audio mic reader", audiothreadstruct);
				//printf("socket is after thread : %d\n",sd);
			  if(thread==NULL){
                    printf("SDL_CreateThread: %s\n",SDL_GetError());
                    success=false;
                }
            }
        }
    }
    printf("Connected.\n");
    return sd;
}

int readThread (void * p){
   Readstruct *r = (Readstruct *) p;
    TCPsocket socket = r->sd;
	bool *loginCheck = r->loginCheck;
	bool *createCheck = r->createCheck;
	char *response;
    char *string;
    json_t *masterobj;
    json_t *keycheckobj;
	json_t *current;
	json_t *buildingblock;
	json_t *globalUsersInRoomArr = r->globalUsersInRoomArr;
	json_t *globalRoomArr=r->globalRoomArr;
	json_t *messageArr=r->messageArr;
	audiostruct_t *audiostruct = r->audiostruct;
	int numBytesRead = 1;
	SDL_mutex * mesageArrMutex = r->messageArrMutex;
	json_error_t error;
    masterobj = json_object();
	//printf("socket is %d", *sd);
    while(numBytesRead>0){

        string = read_from_server(socket, response, &numBytesRead);
        //D(printf("recieved %s\n", response));
        masterobj = json_loads(string, 0, &error);
        if(masterobj == NULL){
			//printf("jsonerror %s\n",error.text);
            free(string);
        }
        else{
            free(string);
			
            if((keycheckobj = json_object_get(masterobj, "audio")) != NULL){
				playaudio(masterobj,audiostruct->writeBlock, audiostruct->writestream );
            }
            if((keycheckobj = json_object_get(masterobj, "login")) != NULL){
                if(json_is_true(keycheckobj)==1){
                    *(loginCheck) = true;
                }
            }

           else if((keycheckobj = json_object_get(masterobj, "message")) != NULL){
                //add mesage to global message arr
				buildingblock = json_object();
				current = json_object_get(masterobj, "username");
				json_object_set_new(buildingblock, "username" ,current);
				current = json_object_get(masterobj, "message");
				json_object_set_new(buildingblock, "message" ,current);
				SDL_LockMutex(mesageArrMutex);
				json_array_append_new(messageArr, buildingblock);
				SDL_UnlockMutex(mesageArrMutex);
            }

           else if((keycheckobj = json_object_get(masterobj, "get users in room")) != NULL){
				if(json_is_true(keycheckobj)){

					//printf("found users arr\n");
					buildingblock= json_object_get(masterobj, "usersArr");
					json_array_clear(globalUsersInRoomArr);
					json_array_extend(globalUsersInRoomArr, buildingblock);
					free(buildingblock);
					}
            }

            else if((keycheckobj = json_object_get(masterobj, "get rooms")) != NULL){
				//printf("collect rooms response\n");
				if(json_is_true(keycheckobj)){
					//printf("found rooms arr\n");
					buildingblock= json_object_get(masterobj, "roomsArr");
					json_array_clear(globalRoomArr);
					json_array_extend(globalRoomArr, buildingblock);
					free (buildingblock);
				}
            }

            else if((keycheckobj = json_object_get(masterobj, "add user")) != NULL){
			if(json_is_true(keycheckobj)){
					*createCheck = true;
				}
			}

        }
        message_printer(masterobj);
        //printf("gonna free");
        //printf("freed\n");
    }
	printf("disconnecting\n");
	//disconnected
	SDLNet_TCP_Close(socket);
	return 1;
}



void clear_str(char *str, int size){
    int i;
    for(i=0; i<size; i++){
        str[i]=0;
    }
    return;
}
void clear_input(){
    char c='\0';
    while(c!='\n'){
        scanf("%c",&c);
    }
}


void send_login(json_t *masterobj_old,std::string* inputUsernameText,std::string* inputPasswordText, TCPsocket *sd, SDL_mutex *writeMutex){
    json_t *masterobj = json_object();
    usr.username=(char*)inputUsernameText->c_str();
    usr.password=(char*)inputPasswordText->c_str();
    //printf("%s\n",inputUsernameText->c_str());
        //printf("%s", usr.username);
        serialize_username(masterobj,&usr);
        //printf("user\n");
        fflush(stdout);
        serialize_password(masterobj,&usr);
        //printf("pass\n");
        fflush(stdout);
        serialize_cmd(masterobj, "login");
        //printf("sd: %d\n", sd);
        fflush(stdout);
        write_to_server(masterobj,sd ,writeMutex);


}



void write_to_server(json_t *masterobj,TCPsocket *socket,SDL_mutex *writeMutex){
    char *json_s;
	int len, result=0;
    json_s = json_dumps(masterobj, 0);
    //printf("%d\n",*socket);
    //json_s = "hej\0";
    //kryptera
    len = strlen(json_s);
    //len=encrypt_Handler(json_s);
    //puts(json_s);//kontroll
    //printf("encrypted string: %s\n",json_s);
    //decrypt_Handler(json_s, len);
    //printf("decrypted string: %s\n",json_s);
    //printf("%s\n",json_s);
    //len = strlen(json_s);
    SDL_LockMutex(writeMutex);
	SDLNet_TCP_Send(*socket, &len, sizeof(int));
    result= SDLNet_TCP_Send(*socket, json_s, len);
	SDL_UnlockMutex(writeMutex);
	if( result < len ) {
    printf( "SDLNet_TCP_Send: %s, result is %d\n", SDLNet_GetError(), result );
    // It may be good to disconnect sock because it is likely invalid now.
	}
    //printf("SENT %s \n", json_s);
}

char* read_from_server( TCPsocket socket, char *response, int *numBytesRead){

    int temp=0,  res;
	printf("reading\n");
    *numBytesRead = SDLNet_TCP_Recv(socket, &temp, sizeof(int));
	//printf("gonna read %d bytes : %d\n",temp);
	if(temp > 10000){
	temp = 10000;
	}
    response = (char *)malloc(temp+1);
	*numBytesRead =SDLNet_TCP_Recv(socket,response, temp );
    response[temp] = '\0';
    //decrypt_Handler(response,*numBytesRead);
    //printf("read response : %s\n",response);
    //dekryptera

    return response;
}

void user_input(char* msg){
    fgets(msg, 254, stdin);
    msg[strlen(msg)-1]='\0';
    //printf("%s", msg);

}


void message_printer(json_t *masterobj){
    json_t *rec_message, *rec_username;
    char string_resp[1000];
    char username[255];
    if((rec_message = json_object_get(masterobj, "message"))!= NULL){
        rec_username = json_object_get(masterobj, "username");
        strcpy(string_resp, json_string_value(rec_message));
        strcpy(username, json_string_value(rec_username));
        //printf("%s : %s\n",username, string_resp);
        fflush(stdout);
    }
}

void serialize_username(json_t *masterobj,user_s *usr ){
    //printf("tudili\n");
    json_t *string;
    string = json_string(usr->username);
    //printf("json string\n");
    json_object_set_new(masterobj, "username", string);
}

void serialize_password(json_t *masterobj,user_s *usr ){
    json_t *string;
    char tmpPassword[PASSWORDSIZE]={'\0'};
    strcpy(tmpPassword,usr->password);
    printf("copied password : %s\n",tmpPassword);
    //int len=encrypt_Handler(tmpPassword);
    //printf("encrypted password : %s\n",tmpPassword);
    //decrypt_Handler(tmpPassword,len);
    //printf("decrypted password : %s\n",tmpPassword);
    string = json_string(tmpPassword);
    json_object_set_new(masterobj, "password", string);
}

void serialize_cmd(json_t *masterobj, char *cmd ){
    json_t *string;
    string = json_string(cmd);
    json_object_set_new(masterobj, "cmd", string);
}

void serialize_room(json_t *masterobj, char* room){
	json_t *string;
    string = json_string(room);
    json_object_set_new(masterobj, "room", string);
}

void serialize_message(json_t *masterobj, Message_s msg){
    json_t *string;
    string = json_string(msg.message);
    json_object_set_new(masterobj, "message", string);
}

void collect_rooms(json_t *masterobj, TCPsocket *socket, SDL_mutex *writeMutex){
    serialize_cmd(masterobj, "get rooms");
	write_to_server(masterobj, socket, writeMutex);
	json_object_clear(masterobj);
}

void logout(json_t *masterobj, TCPsocket *socket, SDL_mutex *writeMutex){
    serialize_cmd(masterobj, "logout");
	write_to_server(masterobj, socket, writeMutex);
	json_object_clear(masterobj);
}

void get_users_in_room(json_t *masterobj, char *room, TCPsocket *socket, SDL_mutex *writeMutex){
    serialize_cmd(masterobj, "get users in room");
    serialize_room(masterobj, room);
	write_to_server(masterobj, socket, writeMutex);
	json_object_clear(masterobj);
}

void add_room(json_t *masterobj, char *room, TCPsocket *socket, SDL_mutex *writeMutex){
    serialize_room(masterobj, room);
    serialize_cmd(masterobj, "add room");
	write_to_server(masterobj, socket, writeMutex);
	json_object_clear(masterobj);
}

void delete_room(json_t *masterobj, char *room, TCPsocket *socket, SDL_mutex *writeMutex){
    serialize_room(masterobj, room);
    serialize_cmd(masterobj, "delete room");
	write_to_server(masterobj, socket, writeMutex);
	json_object_clear(masterobj);
}

void switch_room(json_t *masterobj, char *room ,TCPsocket *socket, SDL_mutex *writeMutex){
    serialize_room(masterobj, room);
    serialize_cmd(masterobj, "switch room");
	write_to_server(masterobj, socket, writeMutex);
	json_object_clear(masterobj);
}

void add_user(json_t * masterobj, user_s *usr, TCPsocket *sd, SDL_mutex *writeMutex){
    serialize_username(masterobj,usr);
    serialize_password(masterobj,usr);
    serialize_cmd(masterobj, "add user");
	write_to_server(masterobj, sd, writeMutex);
	json_object_clear(masterobj);
}

void write_message(json_t *masterobj, user_s *usr, Message_s msg, TCPsocket *socket, SDL_mutex *writeMutex){
	serialize_username(masterobj, usr);
	serialize_room(masterobj, msg.room);
	serialize_message(masterobj, msg);
    serialize_cmd(masterobj, "msg");
	write_to_server(masterobj, socket, writeMutex);
	json_object_clear(masterobj);
}
