#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <string.h>
#include "users.h"
#include "debug.h"

// loads db file and returns true if login successfull else false
bool login(char *username, char *pw){
	SDL_LockMutex(UsersDbMutex);
	FILE *fp = fopen(USERS_DB_FILE, "r");
	json_t *masterObj;
	json_t *currentObj;
	char *encpass, *jsonstr;
	json_error_t error;
	bool success = false;
	int jsonlen;
	if(fscanf(fp,"%d",&jsonlen)>0){
	  D(printf("read size of json it is %d\n", jsonlen));
		jsonstr = (char *)malloc(jsonlen+1);
		int reads = 0;
		if((reads = fread(jsonstr, jsonlen, 1, fp))>0){
		  D(printf("read json %s\n",jsonstr));
			jsonstr[jsonlen]='\0';
			if((masterObj=json_loads(jsonstr, JSON_REJECT_DUPLICATES,&error ))!=NULL){
			  D(printf("loaded db string\n"));
			  D(printf("username is %s\n", username));
				if((currentObj=json_object_get(masterObj, username))!=NULL){
					encpass =json_string_value(currentObj);
					/*TODO ENCRYPT RECIEVED PASSWORD
					*/
					
					D(printf("found user"));
					if(strcmp(encpass, pw)==0){
						success=true;
						D(printf("succesfull login\n"));
					}
				}		
			}
			else{
			  D(printf("NULLPTR\n"));
			  D(printf("err: %s\n",error.text ));
			}
		}
	}
	SDL_UnlockMutex(UsersDbMutex);
	return success;
}

bool add_user(char *UserName, char *pw){
	SDL_LockMutex(UsersDbMutex);
	FILE * fp = fopen(USERS_DB_FILE, "r");
	bool success = false;
	json_t *masterObj,*UserPass;	
	json_error_t error;
	char *dbstr;
	int jsonlen;
	
	D(printf("gonna add a user\n"));	
	if(fscanf(fp, "%d", &jsonlen)>0){
	  D(printf("read size"));
	  fflush(stdout);
	  dbstr = (char *)malloc(jsonlen+1);
	  if(fread(dbstr, jsonlen , 1, fp)>0){
	    D(printf(" read db\n"));
	    fflush(stdout);
	    fclose(fp);
	    masterObj= json_loads(dbstr, JSON_REJECT_DUPLICATES, &error);
	    if(json_object_get(masterObj,UserName)==NULL){
	      /*TODO encrypt pass*/
	      D( printf("no duplicate\n"));
	      UserPass=json_string(pw);
	      json_object_set(masterObj, UserName, UserPass);
	      dbstr = json_dumps(masterObj, 0);
	      fp = fopen(USERS_DB_FILE, "w");
	      fprintf(fp,"%d", strlen(dbstr) );
	      fprintf(fp,"%s", dbstr );
	      success=true;
	      fclose(fp);
	    }
	  }
	}
	
	SDL_UnlockMutex(UsersDbMutex);
	return success;
}

bool del_user(char * UserName, char* pw){ //verify password of the user being deleted and then delete
	bool success = false;
	json_t *masterObj,*UserPass;;	
	json_error_t error;
	char *dbstr;
	int jsonlen;
	FILE * fp;
	
	if(login(UserName, pw)){
		SDL_LockMutex(UsersDbMutex);
		fp = fopen(USERS_DB_FILE, "r");

		if(fscanf(fp, "%d", &jsonlen)){
		  D(printf("read int from file\n"));
			fflush(stdout);
			if(fread(dbstr, jsonlen , 1, fp)>0){
				masterObj= json_loads(dbstr, JSON_REJECT_DUPLICATES, &error);
				/*TODO encrypt pass*/
				json_object_del(masterObj, UserName);
				dbstr = json_dumps(masterObj, 0);
				close(fp);
				fp = fopen(USERS_DB_FILE, "w");
				fprintf(fp,"%d", strlen(dbstr) );
				fprintf(fp,"%s", dbstr );
				success=true;
				
			}
		}

		fclose(fp);
		SDL_UnlockMutex(UsersDbMutex);
	}
	return success;
}

//load users file and try to load it as json returns -1 if file is corrupt, 
//returns 1 if successfull, creates file with default user if file not found;
bool users_init(){
	FILE * fp = fopen(USERS_DB_FILE, "r");
	json_t *masterObj,*UserPass;
	char UserName[] ={"root"};
	bool success = false;
		
	if(fp!=NULL){
	  D(printf("found file\n"));
		char *jsonstr;
		int jsonlen;
		json_error_t error;
		if(fscanf(fp, "%d", &jsonlen)){
			jsonstr = (char *)malloc(jsonlen+1);
			D(printf("read int from file\n"));
			fflush(stdout);
			if(fgets(jsonstr, jsonlen+1, fp)>0)
			{
				//jsonstr[jsonlen]='\0';
			  D(printf("read json %s from file\n", jsonstr));
				
			if((masterObj=json_loads(jsonstr, JSON_REJECT_DUPLICATES, &error))!=NULL){
			  D(printf("loaded json\n"));
					success = true;
					free(masterObj);
					
				}
				
			free(jsonstr);
			}
		}
		fclose(fp);
		D(printf("closed file\n"));
	}
	else{
	  D(printf("no file found, creating file"));
		fflush(stdout);
		fclose(fp);
		char *dbstr;
		masterObj= json_object();
		UserPass=json_string("root");
		json_object_set(masterObj, UserName, UserPass);
		dbstr = json_dumps(masterObj, 0);
		fp = fopen(USERS_DB_FILE, "w");
		D(printf("writing %s to file\n", dbstr));
		fprintf(fp,"%d", strlen(dbstr) );
		fprintf(fp,"%s", dbstr );
		D(printf("wrote file"));
		fclose(fp);
	}
	D(printf("returning %d\n", success));
	return success;
}
