/*
Author David Boeryd
*/
#ifndef USERS
#define USERS
#define USERPASS_LEN 255
#define USERS_DB_FILE "users.db"


extern SDL_mutex *UsersDbMutex;
bool login(char *username, char *pw); //returns true if login was successfull false if not
bool add_user(char *username, char *pw); //returns true if createing a user was successfull
bool del_user(char * username, char *pw); //returns true if deleting a user was sucecssfull
bool users_init(); /*returns true if it either found an existing database and read it or 
					or it successfully created a new one with default user, false if existing db is corrupt*/
					
#endif
