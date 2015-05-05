#ifndef USERS
#define USERS
#define USERPASS_LEN 255
#define USERS_DB_FILE "users.db"


extern SDL_mutex *UsersDbMutex;
bool login(char *username, char *pw);
bool add_user(char *username, char *pw);
bool del_user(char * username, char *pw);
bool users_init();

#endif
